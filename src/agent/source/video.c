#include "global.h"

static CHAR DatName[]="?:\\MPEGAV\\avseq??.dat";

#define STEP 4

static ULONG calcFirstSkip(PCHAR buffer, HFILE hfile, ULONG start, ULONG len)
{
    ULONG t;
    ULONG n;
    n=min(len, 100);
    for(t=0;t<n;)
    {
        ULONG a;
        ULONG sectors=min(n-t, STEP);
        if (!cdromReadLong(hfile, start+t, sectors, buffer))
            return t;
        for(a=0;a<sectors;a++,t++)
            if (buffer[a*SECTOR_SIZE+0x12] & 0x0e)
                return t;
    }
    return t;
}

static ULONG calcLastSkip(PCHAR buffer, HFILE hfile, ULONG start, ULONG len)
{
    ULONG t;
    ULONG n;
    n=min(len, 100);
    for(t=0;t<n;)
    {
        ULONG a;
        ULONG sectors=min(n-t, STEP);
        if (!cdromReadLong(hfile, start+len-t-sectors, sectors, buffer))
            return t;
        for(a=0;a<sectors;a++,t++)
            if (buffer[(sectors-a-1)*SECTOR_SIZE+0x12] & 0x0e)
                return t;
    }
    return 0;
}

BOOL videoGetInfo(CHAR disk, HFILE hfile, ULONG index, PCDTRACKINFO track)
{
    CHAR buffer[SECTOR_SIZE];
    if (track->Mode!=2)
        return FALSE;
    if (!cdromReadLong(hfile, track->Start, 1, buffer))
        return FALSE;
    if (buffer[0x10]==buffer[0x14] &&
        buffer[0x11]==buffer[0x15] &&
        buffer[0x12]==buffer[0x16] &&
        buffer[0x13]==buffer[0x17])
/*        buffer[0x10]<8 &&
        buffer[0x11]<8)*/
    {
        ULONG skip;
        FILESTATUS3 info;
        PCHAR tmp;
        track->Type=TRACK_TYPE_VIDEO;
        DatName[0]=disk;
        DatName[15]=(index/10)%10+'0';
        DatName[16]=index%10+'0';
        //printf("%u: %s\n", index, DatName);
        if (!DosQueryPathInfo(DatName, FIL_STANDARD, &info, sizeof(info)))
        {
            //printf("size=%u\n", info.cbFile);
            //printf("sectors=%u\n", info.cbFile/2048);
            track->RealLen=info.cbFile/2048;
        }
        else
            track->RealLen=track->Len;
        tmp=M_ALLOC(STEP*SECTOR_SIZE);
        if (tmp)
        {
            skip=calcFirstSkip(tmp, hfile, track->Start, track->RealLen);
            //printf("firstskip=%u\n", skip);
            track->FirstSkip=skip;
            skip=calcLastSkip(tmp, hfile, track->Start+skip, track->RealLen-skip);
            //printf("lastskip=%u\n", skip);
            track->RealLen-=skip;
            M_FREE(tmp);
        }
        else
        {
            track->FirstSkip=0;
        }
        return TRUE;
    }
    return FALSE;
}

APIRET videoMakeEA(PCHAR buffer, ULONG len, PCDTRACKINFO track)
{
    _snprintf(buffer, len, "%u_%u_%u_MPG",
              track->Number,
              track->Start,
              track->Len);
    return NO_ERROR;
}

APIRET videoOpen(PMAINDATA maindata, PTDISK disk, PIFS_OPENCREATE p,
                 ULONG track, PCSZ eaapp)
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
    file->Type=FILE_TYPE_DATA;
    file->data.Data.Index=track;
    file->data.Data.Number=disk->Tracks[track].Number;
    file->data.Data.Start=disk->Tracks[track].Start+disk->Tracks[track].FirstSkip;
    file->data.Data.Sectors=disk->Tracks[track].RealLen-disk->Tracks[track].FirstSkip;
    file->data.Data.SectorSize=disk->Tracks[track].SectorSize;
    if (!maindata->Key[0] ||
        maindata->KeyLen%5==0)
        file->data.Data.SectorOffset=disk->Tracks[track].SectorOffset;
    else
        file->data.Data.SectorOffset=0;
    file->data.Data.Sector=ULONG_MAX;
    file->data.Data.Len=0;
    file->data.Data.Data=data;
    p->sffsi.sfi_size=file->data.Data.SectorSize*file->data.Data.Sectors;
    openSetup(file, p);
    return NO_ERROR;
}

APIRET dataRead(PCHAR data, ULONG len, ULONG position,
                 PTDISK disk, PTFILE file)
{
    PDATAFILEDATA datafile=&file->data.Data;
    while (len)
    {
        ULONG sector=position/datafile->SectorSize;
        if (sector<datafile->Sector ||
            sector>=datafile->Sector+datafile->Len)
        {
            ULONG sectors=min(SECTORS, datafile->Sectors-sector);
            if (!cdromGrab(disk, FALSE, datafile->Number,
                           sector+datafile->Start, sectors, datafile->Data))
                return TONIGY_NOT_READY;
            datafile->Sector=sector;
            datafile->Len=sectors;
        }
        {
            ULONG start=position%datafile->SectorSize;
            ULONG tmp=min(len, datafile->SectorSize-start);
            memcpy(data,
                   datafile->Data+(sector-datafile->Sector)*SECTOR_SIZE+datafile->SectorOffset+start,
                   tmp);
            len-=tmp;
            position+=tmp;
            data+=tmp;
        }
    }
    return NO_ERROR;
}