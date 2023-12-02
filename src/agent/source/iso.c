#include "global.h"

static ULONG BootMediaSize[16]=
{
    0, 1200*1024, 1440*1024, 2880*1024, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

BOOL isoGetInfo(HFILE hfile, PCDTRACKINFO track)
{
    CHAR buffer[SECTOR_SIZE];
    PULONG size;
    if (track->Len<17)
        return FALSE;
    if (!cdromReadLong(hfile, track->Start+16, 1, buffer))
        return FALSE;
    if (memcmp(&buffer[track->SectorOffset+1], "CD001", 5))
        return FALSE;
    size=(PULONG)&buffer[track->SectorOffset+80];
    track->RealLen=*size;
    if (track->Len>=18 &&
        cdromReadLong(hfile, track->Start+17, 1, buffer) &&
        !memcmp(&buffer[track->SectorOffset+7], "EL TORITO", 9))
    {
        track->Type=TRACK_TYPE_BOOT;
    }
    else
    {
        track->Type=TRACK_TYPE_ISO;
    }
    return TRUE;
}

#define MAXBOOT 100

static PDIRELEMENT MakeDir(HFILE hfile, PCDTRACKINFO track)
{
    CHAR buffer[SECTOR_SIZE];
    ULONG sectionoffset=0;
    ULONG t;
    ULONG num;
    PDIRELEMENT dir;

    if (!cdromReadLong(hfile, track->Start+17, 1, buffer))
        return NULL;
    t=*(PULONG)&buffer[track->SectorOffset+71];
    //printf("BOOT, catalog at %d\n", t);
    if (!cdromReadLong(hfile, track->Start+t, 1, buffer))
        return NULL;
    /*printf("Catalog:\n\tHeader ID=%d, Platform ID=%d, Developer ID=%s\n",
           *(PUCHAR)(&buffer[track->SectorOffset]),
           *(PUCHAR)(&buffer[track->SectorOffset+1]),
           &buffer[track->SectorOffset+4]);
    printf("\tDefault entry: %x Bootable media: %d\n",
           *(PUCHAR)(&buffer[track->SectorOffset+32+0+sectionoffset]),
           *(PUCHAR)(&buffer[track->SectorOffset+32+1+sectionoffset]) & 15);
    printf("\tSector count: %d, Load LBA: %d\n",
           *(PUSHORT)(&buffer[track->SectorOffset+32+6+sectionoffset]),
           *(PULONG)(&buffer[track->SectorOffset+32+8+sectionoffset]));
    printf("\tID=%24s\n", &buffer[track->SectorOffset+4+sectionoffset]);
    printf("\n");*/

    dir=(PDIRELEMENT)M_ALLOC(sizeof(DIRELEMENT)*4);
    num=0;
    FillDirElement(&dir[num], ".", "", 0, 0, ATTR_DIR);
    dir[num++].Type=ELEMENT_UNKNOWN;
    FillDirElement(&dir[num], "..", "", 0, 0, ATTR_DIR);
    dir[num++].Type=ELEMENT_UNKNOWN;

    t=BootMediaSize[buffer[track->SectorOffset+32+1+sectionoffset] & 15];
    if (!t)
    {
        t=*(PUSHORT)(&buffer[track->SectorOffset+32+6+sectionoffset]);
        //t=t*track->SectorSize;
        t=t*512;
    }
    FillDirElement(&dir[num], "boot01.img", "", t, t, ATTR_FILE);
    dir[num].Sector=*(PULONG)(&buffer[track->SectorOffset+32+8+sectionoffset]);
    dir[num++].Type=ELEMENT_CDIFILE;
    dir[num].Name[0]='\0';
    return dir;
}

PDIRELEMENT isoMakeDirectory(PTDISK disk)
{
    PDIRELEMENT ret=NULL;
    if (disk->Handle!=(HFILE)-1 &&
        disk->Tracks[0].Type==TRACK_TYPE_BOOT)
    {
        ret=MakeDir(disk->Handle, &disk->Tracks[0]);
    }
    return ret;
    /*if (!ret)
    {
        ULONG num=0;
        ret=(PDIRELEMENT)M_ALLOC(sizeof(DIRELEMENT)*3);
        FillDirElement(&ret[num], ".", "", 0, 0, ATTR_DIR);
        ret[num++].Type=ELEMENT_UNKNOWN;
        FillDirElement(&ret[num], "..", "", 0, 0, ATTR_DIR);
        ret[num++].Type=ELEMENT_UNKNOWN;
        ret[num].Name[0]='\0';
    }
    return ret;*/
}

APIRET isoMakeEA(PCHAR buffer, ULONG len, PCDTRACKINFO track)
{
    _snprintf(buffer, len, "%u_%u_%u_ISO",
              track->Number,
              track->Start,
              track->Len);
    return NO_ERROR;
}

APIRET isoOpen(PMAINDATA maindata, PTDISK disk, PIFS_OPENCREATE p,
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
    file->data.Data.Start=disk->Tracks[track].Start;
    file->data.Data.Sectors=disk->Tracks[track].RealLen;
    file->data.Data.SectorSize=disk->Tracks[track].SectorSize;
    file->data.Data.SectorOffset=disk->Tracks[track].SectorOffset;
    file->data.Data.Sector=ULONG_MAX;
    file->data.Data.Len=0;
    file->data.Data.Data=data;
    p->sffsi.sfi_size=file->data.Data.SectorSize*file->data.Data.Sectors;
    openSetup(file, p);
    return NO_ERROR;
}

