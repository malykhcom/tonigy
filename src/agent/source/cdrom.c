#include "global.h"

#pragma pack(1)
typedef struct _CDDEVICEMAP
{
    USHORT count;
    USHORT letter;
} CDDEVICEMAPDATA;

#ifndef IOCTL_CDROMDISK
#define IOCTL_CDROMDISK                 0x80
#endif

#ifndef IOCTL_CDROMAUDIO
#define IOCTL_CDROMAUDIO                0x81
#endif

#ifndef CDROMDISK_RESETDRIVE
#define CDROMDISK_RESETDRIVE            0x40
#endif

#ifndef CDROMDISK_DEVICESTATUS
#define CDROMDISK_DEVICESTATUS          0x60
#endif

#ifndef CDROMDISK_GETDRIVER
#define CDROMDISK_GETDRIVER             0x61
#endif

#ifndef CDROMDISK_READLONG
#define CDROMDISK_READLONG              0x72
#endif

#ifndef CDROMDISK_GETVOLUMESIZE
#define CDROMDISK_GETVOLUMESIZE         0x78
#endif

#ifndef CDROMAUDIO_GETAUDIODISK
#define CDROMAUDIO_GETAUDIODISK         0x61
#endif

#ifndef CDROMAUDIO_GETAUDIOTRACK
#define CDROMAUDIO_GETAUDIOTRACK        0x62
#endif

typedef struct
{
    UCHAR signature[4];
} GENERICPARAM;

typedef struct
{
    ULONG door_open:1;
    ULONG door_unlocked:1;
    ULONG supports_raw:1;
    ULONG can_write:1;
    ULONG supports_multimedia:1;
    ULONG supports_interleaving:1;
    ULONG reserved:1;
    ULONG supports_prefetching:1;
    ULONG audio_channels_manipulation:1;
    ULONG msf_supported:1;
    ULONG mode2_supported:1;
    ULONG disk_present:1;
    ULONG is_playing:1;
    ULONG reserved2:17;
    ULONG reading_CDDA_supported:1;
    ULONG reserevd3:1;
} DEVICESTATUSDATA;

typedef struct
{
    UCHAR signature[4];
    UCHAR addressmode;
    USHORT sectors;
    //    UCHAR startsector;
    ULONG startsector;
    UCHAR reserved;
    UCHAR interleave_size;
    UCHAR interleave_skip_factor; //?
} READLONGPARAM;

typedef struct
{
    ULONG size;
} GETVOLUMESIZEDATA;

typedef struct
{
    UCHAR firsttrack;
    UCHAR lasttrack;
    ULONG leadout;
} GETAUDIODISKDATA;

typedef struct
{
    UCHAR signature[4];
    UCHAR track;
} GETAUDIOTRACKPARAM;

typedef struct
{
    ULONG address;
    UCHAR information;
} GETAUDIOTRACKDATA;

#pragma pack()

static ULONG msf2sector(ULONG loc)
{
    UCHAR min, sec, frame;

    min=(loc>>16)&0xff;
    sec=(loc>>8)&0xff;
    frame=loc&0xff;
    return (ULONG)min*75*60+(ULONG)sec*75+(ULONG)frame-150;
}

static APIRET IOCtl(HFILE hfile, ULONG category, ULONG function,
                    PVOID param, ULONG paramlen,
                    PVOID data, ULONG datalen)
{
    return DosDevIOCtl(hfile, category, function,
                       param, paramlen, &paramlen,
                       data, datalen, &datalen);
}

BOOL cdromDrives(PULONG drives, PCHAR letter)
{
    HFILE hfile;
    ULONG action;
    CDDEVICEMAPDATA map;
    BOOL ret=FALSE;
    if (DosOpen("CD-ROM2$", &hfile, &action, 0, FILE_NORMAL,
                OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE, NULL))
        return FALSE;
    if (IOCtl(hfile, 0x82, 0x60,
              NULL, 0,
              &map, sizeof(map)))
        return FALSE;
    if (map.count)
    {
        *drives=map.count;
        *letter=map.letter+'A';
        ret=TRUE;
    }
    DosClose(hfile);
    return ret;
}

static BOOL Open(ULONG drive, CHAR startletter, PHFILE hfile)
{
    CHAR name[3];
    ULONG action;
    name[0]=(CHAR)(drive+startletter);
    name[1]=':';
    name[2]='\0';
    if (DosOpen(name, hfile, &action,
                0, FILE_NORMAL,
                OPEN_ACTION_OPEN_IF_EXISTS |
                OPEN_ACTION_FAIL_IF_NEW,
                OPEN_FLAGS_DASD | OPEN_ACCESS_READONLY |
                OPEN_SHARE_DENYNONE | OPEN_FLAGS_FAIL_ON_ERROR,
                NULL))
        return FALSE;
    return TRUE;
}

static BOOL QueryDriver(HFILE hfile)
{
    GENERICPARAM param;
    GENERICPARAM data;
    memcpy(param.signature, "CD01", 4);
    if (IOCtl(hfile, IOCTL_CDROMDISK, CDROMDISK_GETDRIVER,
              &param, sizeof(param),
              &data, sizeof(data)))
        return FALSE;
    if (memcmp(&param, &data, sizeof(param)))
        return FALSE;
    return TRUE;
}

static BOOL ResetDriver(HFILE hfile)
{
    GENERICPARAM param;
    memcpy(param.signature, "CD01", 4);
    if (IOCtl(hfile, IOCTL_CDROMDISK, CDROMDISK_RESETDRIVE,
              &param, sizeof(param),
              NULL, 0))
        return FALSE;
    return TRUE;
}

static APIRET QueryVolumeSize(HFILE hfile, PULONG sectors)
{
    GENERICPARAM param;
    GETVOLUMESIZEDATA data;
    memcpy(param.signature, "CD01", 4);
    if (IOCtl(hfile, IOCTL_CDROMDISK, CDROMDISK_GETVOLUMESIZE,
              &param, sizeof(param),
              &data, sizeof(data)))
        return FALSE;
    *sectors=data.size;
    return TRUE;
}

static BOOL GetAudioDisk(HFILE hfile, PCHAR firsttrack, PCHAR lasttrack, PULONG leadout)
{
    GENERICPARAM param;
    GETAUDIODISKDATA data;
    memcpy(param.signature, "CD01", 4);
    if (IOCtl(hfile, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIODISK,
              &param, sizeof(param),
              &data, sizeof(data)))
        return FALSE;
    if (data.firsttrack>data.lasttrack)
        return FALSE;
    *firsttrack=data.firsttrack;
    *lasttrack=data.lasttrack;
    *leadout=data.leadout;
    return TRUE;
}

static BOOL GetAudioTrack(HFILE hfile, ULONG track, PULONG address, PCHAR information)
{
    GETAUDIOTRACKPARAM param;
    GETAUDIOTRACKDATA data;
    memcpy(param.signature, "CD01", 4);
    param.track=(CHAR)track;
    if (IOCtl(hfile, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIOTRACK,
              &param, sizeof(param),
              &data, sizeof(data)))
        return FALSE;
    *address=data.address;
    *information=data.information;
    return TRUE;
}

static BOOL QueryDeviceStatus(HFILE hfile, DEVICESTATUSDATA* data)
{
    GENERICPARAM param;
    memcpy(param.signature, "CD01", 4);
    if (IOCtl(hfile, IOCTL_CDROMDISK, CDROMDISK_DEVICESTATUS,
              &param, sizeof(param),
              data, sizeof(DEVICESTATUSDATA)))
        return FALSE;
    return TRUE;
}

BOOL cdromReadLong(HFILE hfile, ULONG start, ULONG num, PCHAR data)
{
    READLONGPARAM param;
    memset(&param, 0, sizeof(param));
    memcpy(param.signature, "CD01", 4);
    param.sectors=num;
    param.startsector=start;
    if (!IOCtl(hfile, IOCTL_CDROMDISK, CDROMDISK_READLONG,
               &param, sizeof(param),
               data, SECTOR_SIZE*num))
        return TRUE;
    return FALSE;
}

// voldata related

static VOID GenTrackName(PCDTRACKINFO track)
{
    if (track->Number<100)
        _snprintf(track->Name, sizeof(track->Name),
                  "track%02u", track->Number);
    else
        _snprintf(track->Name, sizeof(track->Name),
                  "track%u", track->Number);
}

static VOID GenBlockName(PCDTRACKINFO block)
{
    if (block->Number<100)
        _snprintf(block->Name, sizeof(block->Name),
                  "block%02u", block->Number);
    else
        _snprintf(block->Name, sizeof(block->Name),
                  "block%u", block->Number);
}

static BOOL cdromOpen(PTDISK disk)
{
    HFILE hfile;
    ULONG sectors;
    CHAR firsttrack, lasttrack;
    ULONG leadout;
    //printf("cdromOpen(%u)\n", disk->Drive);
    if (!Open(disk->Drive, disk->StartLetter, &hfile))
    {
        disk->Handle=(HFILE)-1;
        return FALSE;
    }
    {
        LOGRECORD tmp;
        tmp.Type=LOG_CHANGED;
        commonAddLogRecord(disk, &tmp);
    }
    if (QueryDriver(hfile) &&
        ResetDriver(hfile) &&
        QueryVolumeSize(hfile, &sectors) &&
        GetAudioDisk(hfile, &firsttrack, &lasttrack, &leadout))
    {
        ULONG num;
        PCDTRACKINFO tracks;
        PCDTRACKINFO blocks;
        num=min(lasttrack-firsttrack+1, 999);
        tracks=M_ALLOC(sizeof(CDTRACKINFO)*num);
        blocks=M_ALLOC(sizeof(CDTRACKINFO)*num);
        if (tracks && blocks)
        {
            ULONG block;
            ULONG t;
            BOOL lasttype;
            for(t=0;t<num;t++)
            {
                ULONG track;
                ULONG address;
                CHAR info;
                track=t+firsttrack;
                tracks[t].Number=track;
                tracks[t].SectorSize=SECTOR_SIZE;
                GenTrackName(&tracks[t]);
                if (!GetAudioTrack(hfile, track, &address, &info))
                {
                    if (t)
                        tracks[t].Start=tracks[t-1].Start;
                    else
                        tracks[t].Start=0;
                    tracks[t].Type=TRACK_TYPE_INVALID;
                }
                else
                {
                    tracks[t].Start=msf2sector(address);
                    if (info & 64)
                    {
                        CHAR buffer[SECTOR_SIZE];
                        if (cdromReadLong(hfile, tracks[t].Start, 1, buffer) &&
                            buffer[0xf]<=2)
                        {
                            tracks[t].Type=TRACK_TYPE_UNKNOWN;
                            tracks[t].Mode=buffer[0xf];
                            if (tracks[t].Mode==1)
                            {
                                tracks[t].SectorSize=2048;
                                tracks[t].SectorOffset=0x10;
                            }
                            else
                            {
                                tracks[t].SectorSize=(buffer[0x12] & 0x20) ? 2324 : 2048;
                                tracks[t].SectorOffset=0x18;
                            }
                        }
                        else
                        {
                            tracks[t].Type=TRACK_TYPE_INVALID;
                        }
                    }
                    else
                    {
                        tracks[t].Type=TRACK_TYPE_AUDIO;
                    }
                }
/*                if (t==2)
                    tracks[t].Audio=FALSE;*/
                if (t)
                    tracks[t-1].Len=tracks[t].Start-tracks[t-1].Start;
            }
            tracks[num-1].Len=msf2sector(leadout)-tracks[num-1].Start-1;
            for(t=0;t<num;t++)
            {
                if (tracks[t].Type==TRACK_TYPE_UNKNOWN)
                {
                    if (t==0 && cdiGetInfo(hfile, &tracks[t]))
                        continue;
                    if (isoGetInfo(hfile, &tracks[t]))
                        continue;
                    if (videoGetInfo(disk->Drive+disk->StartLetter,
                                     hfile, t, &tracks[t]))
                        continue;
                    //
                }
            }
            block=0;
            lasttype=TRACK_TYPE_INVALID;
            for(t=0;t<num;t++)
            {
                if (tracks[t].Type==TRACK_TYPE_AUDIO)
                {
                    if (lasttype==TRACK_TYPE_AUDIO)
                    {
                        blocks[block-1].Len+=tracks[t].Len;
                    }
                    else
                    {
                        blocks[block].Number=block+1;
                        GenBlockName(&blocks[block]);
                        blocks[block].Type=tracks[t].Type;
                        blocks[block].Start=tracks[t].Start;
                        blocks[block].Len=tracks[t].Len;
                        blocks[block].SectorSize=tracks[t].SectorSize;
                        blocks[block].SectorOffset=tracks[t].SectorOffset;
                        blocks[block].Mode=tracks[t].Mode;
                        block++;
                    }
                }
                lasttype=tracks[t].Type;
            }
            disk->Handle=hfile;
            disk->Sectors=sectors;
            disk->TracksNum=num;
            disk->Tracks=tracks;
            disk->BlocksNum=block;
            disk->Blocks=blocks;
            disk->AJNeed=AJ_UNKNOWN;
            //printf("  num: %u\n", num);
            return TRUE;
        }
        M_FREE(blocks);
        M_FREE(tracks);
    }
    disk->Handle=(HFILE)-1;
    DosClose(hfile);
    return FALSE;
}

VOID cdromClose(PTDISK disk)
{
    ULONG t;
    //printf("cdromClose()\n");
    if (disk->Handle!=(HFILE)-1)
    {
        //printf("  DosClose()\n");
        disk->Handle=(HFILE)-1;
        DosClose(disk->Handle);
        M_FREE(disk->Tracks);
        M_FREE(disk->Blocks);
    }
    for(t=0;t<T_FILES;t++)
    {
        if (disk->Files[t].ID!=ID_EMPTY &&
            disk->Files[t].Type!=FILE_TYPE_README &&
            disk->Files[t].Type!=FILE_TYPE_UNREG)
        {
            disk->Files[t].Ready=FALSE;
        }
    }
}

BOOL cdromQuery(PMAINDATA maindata, PTDISK disk)
{
    BOOL ret;
    PDIRELEMENT dir;
    ULONG num, t;
    CHAR eaapp[EA_APP];
    if (disk->Handle!=(HFILE)-1)
    {
        ULONG sectors;
        if (QueryVolumeSize(disk->Handle, &sectors) &&
            sectors==disk->Sectors)
        {
            //printf("TRUE!\n");
            return TRUE;
        }
        cdromClose(disk);
    }
    ret=cdromOpen(disk);
    DirElementFree(disk->Root);
    if (!ret)
        disk->Root=(PDIRELEMENT)M_ALLOC(sizeof(DIRELEMENT)*10);
    else
        disk->Root=(PDIRELEMENT)M_ALLOC(sizeof(DIRELEMENT)*(10+disk->TracksNum));
    num=0;

    dir=tracksRawMakeDirectory(disk);
    if (dir)
    {
        eaMake(eaapp, sizeof(eaapp), commonTracksRaw, disk);
        FillDirElement(&disk->Root[num], commonTracksRaw, eaapp, 0, 0, ATTR_DIR);
        disk->Root[num].Type=ELEMENT_DIR;
        disk->Root[num++].Dir=dir;
    }

    dir=tracksWavMakeDirectory(disk);
    if (dir)
    {
        eaMake(eaapp, sizeof(eaapp), commonTracksWav, disk);
        FillDirElement(&disk->Root[num], commonTracksWav, eaapp, 0, 0, ATTR_DIR);
        disk->Root[num].Type=ELEMENT_DIR;
        disk->Root[num++].Dir=dir;
    }

    dir=blocksRawMakeDirectory(disk);
    if (dir)
    {
        eaMake(eaapp, sizeof(eaapp), commonBlocksRaw, disk);
        FillDirElement(&disk->Root[num], commonBlocksRaw, eaapp, 0, 0, ATTR_DIR);
        disk->Root[num].Type=ELEMENT_DIR;
        disk->Root[num++].Dir=dir;
    }

    dir=cdiMakeDirectory(disk);
    if (dir)
    {
        eaMake(eaapp, sizeof(eaapp), commonFilesCdi, disk);
        FillDirElement(&disk->Root[num], commonFilesCdi, eaapp, 0, 0, ATTR_DIR);
        disk->Root[num].Type=ELEMENT_DIR;
        disk->Root[num++].Dir=dir;
    }

    dir=isoMakeDirectory(disk);
    if (dir)
    {
        eaMake(eaapp, sizeof(eaapp), commonBootImg, disk);
        FillDirElement(&disk->Root[num], commonBootImg, eaapp, 0, 0, ATTR_DIR);
        disk->Root[num].Type=ELEMENT_DIR;
        disk->Root[num++].Dir=dir;
    }
    
    eaMake(eaapp, sizeof(eaapp), commonReadmeName, disk);
    FillDirElement(&disk->Root[num], commonReadmeName, eaapp,
                   strlen(maindata->Readme), 0, ATTR_FILE);
    disk->Root[num++].Type=ELEMENT_README;
    {
        PCHAR info;
        eaMake(eaapp, sizeof(eaapp), commonCdinfoName, disk);
        info=cdromInfo(disk);
        if (info)
        {
            FillDirElement(&disk->Root[num], commonCdinfoName, eaapp,
                           strlen(info), 0, ATTR_FILE);
            M_FREE(info);
        }
        else
        {
            FillDirElement(&disk->Root[num], commonCdinfoName, eaapp,
                           0, 0, ATTR_FILE);
        }
        disk->Root[num++].Type=ELEMENT_CDINFO;
    }
    if (!maindata->Key[0] || maindata->KeyRes)
    {
        CHAR tmp[6];
        CHAR unreg[6];
        unreg[0]='u';
        unreg[2]='r';
        unreg[3]='e';
        unreg[1]='n';
        unreg[4]='g';
        unreg[5]='\0';
        eaMake(eaapp, sizeof(eaapp), unreg, disk);
        if (!commonUnregName[Lang][0])
        {
            tmp[0]='$';
            tmp[1]='&';
            tmp[2]='@';
            tmp[3]='%';
            tmp[4]='!';
            tmp[5]='\0';
            FillDirElement(&disk->Root[num], tmp, eaapp,
                           0, 0, ATTR_FILE);
        }
        else
            FillDirElement(&disk->Root[num], commonUnregName[Lang], eaapp,
                           strlen(maindata->Unreg), 0, ATTR_FILE);
        disk->Root[num++].Type=ELEMENT_UNREG;
    }
    if (ret)
    {
        for(t=0;t<disk->TracksNum;t++)
        {
            PCDTRACKINFO track=&disk->Tracks[t];
            if (track->Type!=TRACK_TYPE_INVALID &&
                track->Type!=TRACK_TYPE_CDI)
            {
                ULONG alloc=SECTOR_SIZE*track->Len;
                PCHAR name=disk->Root[num].Name;
                switch (track->Type)
                {
                case TRACK_TYPE_AUDIO:
                    audioMakeEA(eaapp, sizeof(eaapp), FALSE, track, FALSE);
                    FillDirElement(&disk->Root[num], track->Name, eaapp,
                                   alloc+sizeof(WAVHEADER),
                                   alloc, ATTR_FILE);
                    disk->Root[num].Type=ELEMENT_AUDIO_TRACK_WAV;
                    strcat(name, commonWavExt);
                    break;
                case TRACK_TYPE_VIDEO:
                    videoMakeEA(eaapp, sizeof(eaapp), track);
                    FillDirElement(&disk->Root[num], track->Name, eaapp,
                                   (track->RealLen-track->FirstSkip)*track->SectorSize,
                                   alloc, ATTR_FILE);
                    disk->Root[num].Type=ELEMENT_VIDEO;
                    strcat(name, commonMpgExt);
                    break;
                case TRACK_TYPE_ISO:
                case TRACK_TYPE_BOOT:
                    isoMakeEA(eaapp, sizeof(eaapp), track);
                    FillDirElement(&disk->Root[num], track->Name, eaapp,
                                   track->RealLen*track->SectorSize,
                                   alloc, ATTR_FILE);
                    disk->Root[num].Type=ELEMENT_ISO;
                    strcat(name, commonIsoExt);
                    break;
                default:
                    FillDirElement(&disk->Root[num], track->Name, "",
                                   track->Len*track->SectorSize,
                                   alloc, ATTR_FILE);
                    track->RealLen=track->Len;
                    disk->Root[num].Type=ELEMENT_ISO;
                    strcat(name, commonUnkExt);
                }
                disk->Root[num].Index=t;
                num++;
            }
        }
    }
    eaMake(eaapp, sizeof(eaapp), commonLogName, disk);
    FillDirElement(&disk->Root[num], "", eaapp,
                   0, 0, ATTR_FILE);
    disk->Root[num].Type=ELEMENT_LOG;
    if (disk->LogFirst)
    {
        strcpy(disk->Root[num].Name, commonLogName);
        disk->Root[num].Size=disk->LogRecords*LOG_LINE;
    }
    else
        disk->Root[num].Name[0]='\0';
    disk->Root[num+1].Name[0]='\0';
    return ret;
}

#define MAXSIZE 8000
#define VERIFY \
    tmp=strlen(info); \
    if (tmp+1>=MAXSIZE) return info;

static VOID PrintTrackInfo(PSZ info, PCDTRACKINFO track, PULONG audio)
{
    ULONG tmp;
    CHAR mode;
    PCSZ type;
    tmp=strlen(info);
    if (tmp+1>=MAXSIZE)
        return;
    if (track->Type==TRACK_TYPE_INVALID ||
        track->Type==TRACK_TYPE_AUDIO)
        mode='-';
    else
        if (track->Mode==1)
            mode='1';
        else
            mode='2';
    switch (track->Type)
    {
    case TRACK_TYPE_AUDIO:
        type=localeGetMsg("cd.audio");
        break;
    case TRACK_TYPE_ISO:
        type=localeGetMsg("cd.iso");
        break;
    case TRACK_TYPE_BOOT:
        type=localeGetMsg("cd.isoboot");
        break;
    case TRACK_TYPE_VIDEO:
        type=localeGetMsg("cd.xa");
        break;
    case TRACK_TYPE_CDI:
        type=localeGetMsg("cd.cdi");
        break;
    default:
        type=localeGetMsg("cd.inknown");;
    }
    _snprintf(info+tmp, MAXSIZE-tmp,
              " %3u %10u %10u     %c    %u  %s",
              track->Number,
              track->Start,
              track->Len,
              mode,
              track->SectorSize,
              type);
    tmp=strlen(info);
    if (tmp+1>MAXSIZE)
        return;
    if (track->Type==TRACK_TYPE_AUDIO)
    {
        ULONG secs;
        secs=track->Len*SECTOR_SIZE/(44100*2*2);
        _snprintf(info+tmp, MAXSIZE-tmp,
                  " (%02u:%02u)\r\n",
                  secs/60, secs%60);
        *audio=*audio+1;
    }
    else
        _snprintf(info+tmp, MAXSIZE-tmp,
                  "\r\n");
}

#define SUPPORTS(f, n) status.f?localeGetMsg("cd."n".yes"):localeGetMsg("cd."n".no")

PSZ cdromInfo(PTDISK disk)
{
    PSZ info;
    ULONG tmp;
    info=M_ALLOC(MAXSIZE);
    if (!info)
        return NULL;
    _snprintf(info, MAXSIZE, localeGetMsg("cd.info"),
              disk->Letter, disk->Drive+disk->StartLetter,
              TONIGY);
    if (disk->Handle!=(HFILE)-1)
    {
        DEVICESTATUSDATA status;
        ULONG t;
        ULONG audio;
        VERIFY;
        _snprintf(info+tmp, MAXSIZE-tmp, localeGetMsg("cd.toc"));
        audio=0;
        for(t=0;t<disk->TracksNum;t++)
        {
            PrintTrackInfo(info, &disk->Tracks[t], &audio);
        }
        VERIFY;
        _snprintf(info+tmp, MAXSIZE-tmp, localeGetMsg("cd.tail"), audio);
        VERIFY;
        _snprintf(info+tmp, MAXSIZE-tmp, localeGetMsg("cd.toc2"));
        audio=0;
        for(t=0;t<disk->BlocksNum;t++)
        {
            PrintTrackInfo(info, &disk->Blocks[t], &audio);
        }
        VERIFY;
        _snprintf(info+tmp, MAXSIZE-tmp, localeGetMsg("cd.tail2"), audio);
        if (disk->AJNeed!=AJ_UNKNOWN)
        {
            VERIFY;
            _snprintf(info+tmp, MAXSIZE-tmp,
                      "%s\r\n"
                      "\r\n",
                      (disk->AJNeed==AJ_NEED)?
                      localeGetMsg("cd.jitter.yes"):
                      localeGetMsg("cd.jitter.no"));
        }
        if (QueryDeviceStatus(disk->Handle, &status))
        {
            VERIFY;
            _snprintf(info+tmp, MAXSIZE-tmp,
                      "%s\r\n"
                      "   %s\r\n   %s\r\n   %s\r\n   %s\r\n"
                      "   %s\r\n   %s\r\n   %s\r\n   %s\r\n"
                      "   %s\r\n\r\n",
                      localeGetMsg("cd.caps"),
                      SUPPORTS(supports_raw, "raw"),
                      SUPPORTS(can_write, "write"),
                      SUPPORTS(supports_multimedia, "multimedia"),
                      SUPPORTS(supports_interleaving, "interleaving"),
                      SUPPORTS(supports_prefetching, "prefetching"),
                      SUPPORTS(audio_channels_manipulation, "audiochannels"),
                      SUPPORTS(msf_supported, "msf"),
                      SUPPORTS(mode2_supported, "mode2"),
                      SUPPORTS(reading_CDDA_supported, "longread"));
        }
    }
    else
    {
        VERIFY;
        _snprintf(info+tmp, MAXSIZE-tmp, localeGetMsg("cd.notready"));
    }
    return info;
}

PSZ cdromLog(PTDISK disk)
{
    PSZ log;
    ULONG t;
    PLOGRECORD record;
    COUNTRYCODE code;
    COUNTRYINFO info;
    log=M_ALLOC(disk->LogRecords*LOG_LINE+1);
    if (!log)
        return NULL;
    code.country=0;
    code.codepage=0;
    if (DosQueryCtryInfo(sizeof(info), &code, &info, &t))
    {
        info.fsDateFmt=2;
        strcpy(info.szDateSeparator, ".");
        strcpy(info.szTimeSeparator, ":");
    }
    record=disk->LogFirst;
    for(t=0;t<disk->LogRecords;t++)
    {
        PCHAR line;
        CHAR tmp[LOG_LINE];
        line=&log[t*LOG_LINE];
        memset(line, ' ', LOG_LINE-2);
        line[LOG_LINE-2]='\r';
        line[LOG_LINE-1]='\n';
        /*switch (info.fsDateFmt)
        {
        case 0:
            _snprintf(tmp, sizeof(tmp), "%02u%s%02u%s%04u",
                      record->DateTime.month,
                      info.szDateSeparator,
                      record->DateTime.day,
                      info.szDateSeparator,
                      record->DateTime.year);
            break;
        case 1:
            _snprintf(tmp, sizeof(tmp), "%02u%s%02u%s%04u",
                      record->DateTime.day,
                      info.szDateSeparator,
                      record->DateTime.month,
                      info.szDateSeparator,
                      record->DateTime.year);
            break;
        default:
            _snprintf(tmp, sizeof(tmp), "%04u%s%02u%s%02u",
                      record->DateTime.year,
                      info.szDateSeparator,
                      record->DateTime.month,
                      info.szDateSeparator,
                      record->DateTime.day);
        }
        memcpy(line, tmp, strlen(tmp));*/
        _snprintf(tmp, sizeof(tmp), "%02u%s%02u%s%02u",
                  record->DateTime.hours,
                  info.szTimeSeparator,
                  record->DateTime.minutes,
                  info.szTimeSeparator,
                  record->DateTime.seconds);
        //memcpy(&line[11], tmp, strlen(tmp));
        memcpy(line, tmp, strlen(tmp));
        line[9]='|';
        switch (record->Type)
        {
        case LOG_ATTACHED:
            _snprintf(tmp, sizeof(tmp), localeGetMsg("cd.log.attached"),
                      disk->Letter);
            break;
        case LOG_CHANGED:
            strcpy(tmp, localeGetMsg("cd.log.header"));
            break;
        case LOG_JITTER:
            strcpy(tmp, localeGetMsg("cd.log.jitter"));
            break;
        case LOG_READ:
            _snprintf(tmp, sizeof(tmp),
                      record->Block?
                      localeGetMsg("cd.log.readblock"):
                      localeGetMsg("cd.log.readtrack"),
                      record->Fatal?
                      localeGetMsg("cd.log.error"):
                      localeGetMsg("cd.log.warning"),
                      record->Number,
                      record->Sector);
            break;
        case LOG_AJ:
            _snprintf(tmp, sizeof(tmp),
                      record->Block?
                      localeGetMsg("cd.log.ajblock"):
                      localeGetMsg("cd.log.ajtrack"),
                      record->Fatal?
                      localeGetMsg("cd.log.error"):
                      localeGetMsg("cd.log.warning"),
                      record->Number,
                      record->Sector);
            break;
        default:
            strcpy(tmp, "???");
        }
        memcpy(&line[11], tmp, min(LOG_LINE-13, strlen(tmp)));
        record=record->Next;
    }
    log[disk->LogRecords*LOG_LINE]='\0';
    return log;
}

BOOL cdromGrab(PTDISK disk, BOOL block, ULONG number, ULONG start, ULONG num, PCHAR data)
{
    ULONG t;
    if (disk->Handle==(HFILE)-1)
    {
        LOGRECORD tmp;
        tmp.Type=LOG_READ;
        tmp.Block=block;
        tmp.Number=number;
        tmp.Sector=start;
        tmp.Fatal=TRUE;
        commonAddLogRecord(disk, &tmp);
        return FALSE;
    }
    for(t=0;t<REREADS;t++)
    {
        LOGRECORD tmp;
        if (cdromReadLong(disk->Handle, start, num, data))
            return TRUE;
        tmp.Type=LOG_READ;
        tmp.Block=block;
        tmp.Number=number;
        tmp.Sector=start;
        if (t+1==REREADS)
            tmp.Fatal=TRUE;
        else
            tmp.Fatal=FALSE;
        commonAddLogRecord(disk, &tmp);
    }
    return FALSE;
}
