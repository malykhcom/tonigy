#include "global.h"

#define SUM (778)

#define NOT_ALLOW (audio->Index+3>12 && sum!=SUM)

APIRET audioMakeEA(PCHAR buffer, ULONG len,
                   BOOL block, PCDTRACKINFO track, BOOL raw)
{
    if (!block)
        _snprintf(buffer, len, "%u_%u_%u_%s",
                  track->Number,
                  track->Start,
                  track->Len,
                  raw?"RAW":"WAV");
    else
        _snprintf(buffer, len, "%u_%u_%u_%s_BLOCK",
                  track->Number,
                  track->Start,
                  track->Len,
                  raw?"RAW":"WAV");
    return NO_ERROR;
}

APIRET audioOpen(PMAINDATA maindata, PTDISK disk, PIFS_OPENCREATE p,
                BOOL block, ULONG track, BOOL raw, PCSZ eaapp)
{
    APIRET rc;
    PCHAR data;
    PTFILE file;
    rc=openVerifyFlags(p->mode, p->flag);
    if (rc)
        return rc;
    data=M_ALLOC(SECTOR_SIZE*SECTORS);
    if (!data)
        return ERROR_NOT_ENOUGH_MEMORY;
    rc=openAllocFile(disk, &file);
    if (rc)
    {
        M_FREE(data);
        return rc;
    }
    strcpy(file->EAApp, eaapp);
    file->Type=FILE_TYPE_AUDIO;
    file->data.Audio.Inited=FALSE;
    file->data.Audio.Block=block;
    file->data.Audio.Raw=raw;
    DosRequestMutexSem(maindata->OptionsMutex, SEM_INDEFINITE_WAIT);
    file->data.Audio.Options=maindata->Options[disk->Drive].Options;
    DosReleaseMutexSem(maindata->OptionsMutex);
    file->data.Audio.Index=track;
    if (!block)
    {
        file->data.Audio.Number=disk->Tracks[track].Number;
        file->data.Audio.Start=disk->Tracks[track].Start;
    }
    else
    {
        file->data.Audio.Number=disk->Blocks[track].Number;
        file->data.Audio.Start=disk->Blocks[track].Start;
    }
    file->data.Audio.Data=data;
    if (!raw)
    {
        PWAVHEADER wavheader=&file->data.Audio.WavHeader;
        ULONG len;
        if (!block)
            len=disk->Tracks[track].Len*SECTOR_SIZE;
        else
            len=disk->Blocks[track].Len*SECTOR_SIZE;
        memcpy(wavheader->rID, "RIFF", 4);
        wavheader->rLen=sizeof(WAVHEADER)-8+len;
        memcpy(wavheader->wID, "WAVE", 4);
        memcpy(wavheader->fID, "fmt ", 4);
        wavheader->fLen=16;
        wavheader->Tag=1;
        wavheader->Channels=2;
        wavheader->Rate=44100;
        wavheader->Bytes=44100*2*16/8;
        wavheader->Align=2*16/8;
        wavheader->Bits=16;
        memcpy(wavheader->dID, "data", 4);
        wavheader->dLen=len;
    }
    if (!block)
        if (maindata->KeyRes ||
            (maindata->Key[maindata->KeyLen-5] &&
             (track%maindata->Key[maindata->KeyLen-5]==0)))
            p->sffsi.sfi_size=disk->Tracks[track].Len*SECTOR_SIZE+
                (raw?0:sizeof(WAVHEADER));
        else
            p->sffsi.sfi_size=0;
    else
        p->sffsi.sfi_size=disk->Blocks[track].Len*SECTOR_SIZE+
            (raw?0:sizeof(WAVHEADER));
    openSetup(file, p);
    return NO_ERROR;
}

static BOOL IsJitter(PAUDIOFILEDATA audio, PTDISK disk, ULONG offset)
{
    CHAR tmp[SECTOR_SIZE];
    if (cdromGrab(disk, audio->Block, audio->Number, audio->Start+offset, 1,
                  audio->Data) &&
        cdromGrab(disk, audio->Block, audio->Number, audio->Start+offset, 1,
                  tmp))
    {
        if (memcmp(tmp, audio->Data, SECTOR_SIZE))
        {
            disk->AJNeed=AJ_NEED;
            return TRUE;
        }
        else
            disk->AJNeed=AJ_NOT_NEED;
    }
    return FALSE;
}

static APIRET InitTrack(ULONG sum, PAUDIOFILEDATA audio, PTDISK disk)
{
    ULONG t;
    for(t=0;t<SPIN_UP;t++)
        cdromGrab(disk, audio->Block, audio->Number, audio->Start, 1, audio->Data);
    t=disk->Tracks[audio->Index].Len;
    if (t>200)// && disk->AJNeed==AJ_UNKNOWN)
    {
        if (IsJitter(audio, disk, 100) ||
            IsJitter(audio, disk, 200))
        {
            if (!audio->Options.Antijitter)
            {
                LOGRECORD tmp;
                tmp.Type=LOG_JITTER;
                commonAddLogRecord(disk, &tmp);
            }
        }
    }
    if (!cdromGrab(disk, audio->Block, audio->Number, audio->Start, SECTORS, audio->Data))
        return TONIGY_NOT_READY;
    audio->Inited=TRUE;
    audio->Sector=audio->Start;
    audio->Len=SECTORS;
    audio->Preread=5;
    audio->Pointer=3*SECTOR_SIZE;
    if NOT_ALLOW
        memset(audio->Data, 0, SECTORS*SECTOR_SIZE);
    return NO_ERROR;
}

static APIRET ReadSectorsE(PAUDIOFILEDATA audio, PTDISK disk, ULONG sector)
{
    ULONG len;
    len=min(SECTORS, disk->Sectors-sector);
    memset(audio->Data, 0, len*SECTOR_SIZE);
    audio->Sector=sector;
    audio->Len=len;
    return NO_ERROR;
}

static APIRET ReadSectorsWAJ(PAUDIOFILEDATA audio, PTDISK disk, ULONG sector)
{
    ULONG len;
    len=min(SECTORS, disk->Sectors-sector);
    if (!cdromGrab(disk, audio->Block, audio->Number, sector, len, audio->Data))
        return TONIGY_NOT_READY;
    audio->Sector=sector;
    audio->Len=len;
    return NO_ERROR;
}

static CHAR Data[SECTOR_SIZE*SECTORS];
static CHAR LastData[COMPARE];

_inline BOOL Compare(PCHAR data1, PCHAR data2, ULONG jitter)
{
    ULONG cnt=0;
    ULONG t;
    for(t=0;t<COMPARE;t++)
    {
        if (data1[t]!=data2[t])
        {
            cnt++;
            if (cnt>jitter)
                return FALSE;
        }
    }
    return TRUE;
}

static VOID CalcData(PAUDIOFILEDATA audio, ULONG pointer, ULONG sector)
{
    ULONG readed;
    LONG preread;
    pointer+=COMPARE;
    readed=(SECTORS*SECTOR_SIZE-pointer)/SECTOR_SIZE;
    {
        LONG delta;
        delta=pointer-audio->Preread*SECTOR_SIZE;
        //printf("   %d, %u\n", delta, readed);
        if (delta<0)
            audio->Pointer=(audio->Pointer-delta)/2;
        else
            audio->Pointer=audio->Pointer*6/7;
    }
    preread=audio->Pointer/SECTOR_SIZE+2;
    memcpy(audio->Data, &Data[pointer], readed*SECTOR_SIZE);
    audio->Sector=sector;
    audio->Len=readed;
    audio->Preread=preread;
}


static APIRET ReadSectorsAJ(PAUDIOFILEDATA audio, PTDISK disk, ULONG sector)
{
    ULONG pointer;
    ULONG t;
    if (sector>=disk->Sectors-SECTORS)
    {
        memset(audio->Data, 0, SECTORS*SECTOR_SIZE);
        audio->Sector=sector;
        audio->Len=SECTORS;
        return NO_ERROR;
    }
    memcpy(LastData, audio->Data+audio->Len*SECTOR_SIZE-COMPARE, COMPARE);
    for(t=0;t<audio->Options.Tries;t++)
    {
        ULONG tmp;
        if (audio->Options.Preread)
            tmp=audio->Options.Preread;
        else
            tmp=audio->Preread;
/*        printf("  try %u/%u (pre %u/%u) SECTORS=%u\n",
               t, track->Options.Tries,
               tmp, track->Options.Preread,
               SECTORS);*/
        memset(Data, 0, SECTORS*SECTOR_SIZE);
        if (!cdromGrab(disk, audio->Block, audio->Number, sector-tmp, SECTORS, Data))
            return TONIGY_NOT_READY;
        for(pointer=tmp*SECTOR_SIZE-COMPARE;pointer;pointer--)
            if (Compare(&Data[pointer], LastData, t*JITTER))
            {
                CalcData(audio, pointer, sector);
                return NO_ERROR;
            }
        for(pointer=tmp*SECTOR_SIZE-COMPARE+1;
            pointer<=(SECTORS-1)*SECTOR_SIZE-COMPARE;pointer++)
            if (Compare(&Data[pointer], LastData, t*JITTER))
            {
                CalcData(audio, pointer, sector);
                return NO_ERROR;
            }
        if (audio->Preread<10)
            audio->Preread=audio->Preread+1;
        audio->Pointer=audio->Preread*SECTOR_SIZE;
        {
            LOGRECORD tmp;
            tmp.Type=LOG_AJ;
            tmp.Block=audio->Block;
            tmp.Number=audio->Number;
            tmp.Sector=sector;
            tmp.Fatal=FALSE;
            commonAddLogRecord(disk, &tmp);
        }
    }
    {
        LOGRECORD tmp;
        tmp.Type=LOG_AJ;
        tmp.Block=audio->Block;
        tmp.Number=audio->Number;
        tmp.Sector=sector;
        tmp.Fatal=TRUE;
        commonAddLogRecord(disk, &tmp);
    }
    //printf("    FAILED :((((\n");
    return ReadSectorsWAJ(audio, disk, sector);
}

_inline APIRET ReadSectors(ULONG sum, PAUDIOFILEDATA audio, PTDISK disk, ULONG sector)
{
    if NOT_ALLOW
        return ReadSectorsE(audio, disk, sector);
    if (audio->Options.Antijitter &&
        (sector==audio->Sector+audio->Len))
        return ReadSectorsAJ(audio, disk, sector);
    return ReadSectorsWAJ(audio, disk, sector);
}

APIRET audioRead(PMAINDATA maindata, PCHAR data, ULONG len, ULONG position,
                 PTDISK disk, PTFILE file)
{
    APIRET rc;
    PAUDIOFILEDATA audio=&file->data.Audio;
    ULONG t, sum=0;
    for(t=0; t<10; t++)
        sum+=maindata->Key[30+t];
    //printf("Sum: %d\n", sum);
    if (!audio->Raw)
    {
        if (position<sizeof(WAVHEADER))
        {
            ULONG tmp;
            PCHAR header=(PCHAR)&audio->WavHeader;
            tmp=min(sizeof(WAVHEADER)-position, len);
            memcpy(data, header+position, tmp);
            len-=tmp;
            position+=tmp;
            data+=tmp;
        }
        position-=sizeof(WAVHEADER);
        if (!len)
            return NO_ERROR;
    }
    if (!audio->Inited)
    {
        rc=InitTrack(sum, audio, disk);
        if (rc)
            return rc;
    }
    while (len)
    {
        ULONG sector=position/SECTOR_SIZE+audio->Start;
        if (sector<audio->Sector ||
            sector>=audio->Sector+audio->Len)
        {
            rc=ReadSectors(sum, audio, disk, sector);
            if (rc)
                return rc;
        }
        {
            ULONG start=(sector-audio->Sector)*SECTOR_SIZE+
                        position%SECTOR_SIZE;
            ULONG tmp=min(len, SECTOR_SIZE*audio->Len-start);
            memcpy(data, audio->Data+start, tmp);
            len-=tmp;
            position+=tmp;
            data+=tmp;
        }
    }
    return NO_ERROR;
}

PDIRELEMENT blocksRawMakeDirectory(PTDISK disk)
{
    PDIRELEMENT blocks;
    ULONG num, t;
    CHAR eaapp[EA_APP];
    if (disk->Handle==(HFILE)-1)
        return NULL;
    num=0;
    for(t=0;t<disk->BlocksNum;t++)
    {
        PCDTRACKINFO block=&disk->Blocks[t];
        if (block->Type==TRACK_TYPE_AUDIO)
            num++;
    }
    if (num==0)
        return NULL;
    blocks=(PDIRELEMENT)M_ALLOC(sizeof(DIRELEMENT)*(3+disk->BlocksNum));
    num=0;
    FillDirElement(&blocks[num], ".", "", 0, 0, ATTR_DIR);
    blocks[num++].Type=ELEMENT_UNKNOWN;
    FillDirElement(&blocks[num], "..", "", 0, 0, ATTR_DIR);
    blocks[num++].Type=ELEMENT_UNKNOWN;
    for(t=0;t<disk->BlocksNum;t++)
    {
        PCDTRACKINFO block=&disk->Blocks[t];
        if (block->Type==TRACK_TYPE_AUDIO)
        {
            audioMakeEA(eaapp, sizeof(eaapp), TRUE, block, TRUE);
            FillDirElement(&blocks[num],block->Name, eaapp,
                           SECTOR_SIZE*block->Len,
                           SECTOR_SIZE*block->Len, ATTR_FILE);
            blocks[num].Type=ELEMENT_AUDIO_BLOCK_RAW;
            blocks[num].Index=t;
            strcat(blocks[num++].Name, commonRawExt);
        }
    }
    blocks[num].Name[0]='\0';
    return blocks;
}

PDIRELEMENT tracksRawMakeDirectory(PTDISK disk)
{
    PDIRELEMENT tracks;
    ULONG num, t;
    CHAR eaapp[EA_APP];
    if (disk->Handle==(HFILE)-1)
        return NULL;
    num=0;
    for (t=0; t<disk->TracksNum; t++)
    {
        PCDTRACKINFO track=&disk->Tracks[t];
        if (track->Type==TRACK_TYPE_AUDIO)
            num++;
    }
    if (num==0)
        return NULL;
    tracks=(PDIRELEMENT)M_ALLOC(sizeof(DIRELEMENT)*(3+disk->TracksNum));
    num=0;
    FillDirElement(&tracks[num], ".", "", 0, 0, ATTR_DIR);
    tracks[num++].Type=ELEMENT_UNKNOWN;
    FillDirElement(&tracks[num], "..", "", 0, 0, ATTR_DIR);
    tracks[num++].Type=ELEMENT_UNKNOWN;
    for (t=0;t<disk->TracksNum;t++)
    {
        PCDTRACKINFO track=&disk->Tracks[t];
        if (track->Type==TRACK_TYPE_AUDIO)
        {
            audioMakeEA(eaapp, sizeof(eaapp), FALSE, track, TRUE);
            FillDirElement(&tracks[num], track->Name, eaapp,
                           SECTOR_SIZE*track->Len,
                           SECTOR_SIZE*track->Len, ATTR_FILE);
            tracks[num].Type=ELEMENT_AUDIO_TRACK_RAW;
            tracks[num].Index=t;
            strcat(tracks[num++].Name, commonRawExt);
        }
    }
    tracks[num].Name[0]='\0';
    return tracks;
}

PDIRELEMENT tracksWavMakeDirectory(PTDISK disk)
{
    PDIRELEMENT tracks;
    ULONG num, t;
    CHAR eaapp[EA_APP];
    if (disk->Handle==(HFILE)-1)
        return NULL;
    num=0;
    for (t=0; t<disk->TracksNum; t++)
    {
        PCDTRACKINFO track=&disk->Tracks[t];
        if (track->Type==TRACK_TYPE_AUDIO)
            num++;
    }
    if (num==0)
        return NULL;
    tracks=(PDIRELEMENT)M_ALLOC(sizeof(DIRELEMENT)*(3+disk->TracksNum));
    num=0;
    FillDirElement(&tracks[num], ".", "", 0, 0, ATTR_DIR);
    tracks[num++].Type=ELEMENT_UNKNOWN;
    FillDirElement(&tracks[num], "..", "", 0, 0, ATTR_DIR);
    tracks[num++].Type=ELEMENT_UNKNOWN;
    for (t=0;t<disk->TracksNum;t++)
    {
        PCDTRACKINFO track=&disk->Tracks[t];
        if (track->Type==TRACK_TYPE_AUDIO)
        {
            ULONG alloc=SECTOR_SIZE*track->Len;
            audioMakeEA(eaapp, sizeof(eaapp), FALSE, track, FALSE);
            FillDirElement(&tracks[num], track->Name, eaapp,
                           alloc+sizeof(WAVHEADER),
                           alloc, ATTR_FILE);
            tracks[num].Type=ELEMENT_AUDIO_TRACK_WAV;
            tracks[num].Index=t;
            strcat(tracks[num++].Name, commonWavExt);
        }
    }
    tracks[num].Name[0]='\0';
    return tracks;
}