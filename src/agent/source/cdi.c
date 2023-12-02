#include "global.h"

#pragma pack(1)
typedef struct
{
    CHAR NameLen[2];
    CHAR Extent[4];
    CHAR Parent[2];
    CHAR Name[1];
} PATH_TABLE, * PPATH_TABLE;
typedef struct
{
    CHAR Len;
    CHAR ExtLen;
    CHAR Reserved0[4];
    CHAR Sector[4];
    CHAR Reserved1[4];
    CHAR Size[4];
    CHAR Date[6];
    CHAR Reserved2[1];
    CHAR Flags;
    CHAR Interleave[2];
    CHAR Reserved3[2];
    CHAR Album[2];
    CHAR NameLen;
    CHAR Name[1];
} DIRRECORD, * PDIRRECORD;
typedef struct
{
    CHAR Owner[4];
    CHAR Attr[2];
    CHAR Reserved0[2];
    CHAR FileNo;
    CHAR Reserved1[1];
} DIRRECORDEXT, * PDIRRECORDEXT;

#pragma pach()

BOOL cdiGetInfo(HFILE hfile, PCDTRACKINFO track)
{
    CHAR buffer[SECTOR_SIZE];
    ULONG t;
    if (track->Mode!=2 ||
        track->Len<17)
        return FALSE;
    if (!cdromReadLong(hfile, track->Start+16, 1, buffer))
        return FALSE;
    if (memcmp(&buffer[track->SectorOffset+1], "CD-I ", 5) ||
        memcmp(&buffer[track->SectorOffset+8], "CD-RTOS", 7))
        return FALSE;
    track->Type=TRACK_TYPE_CDI;
    t=Extract4(&buffer[track->SectorOffset+148]);
    //printf("path table=%u\n", t);
    if (!cdromReadLong(hfile, track->Start+t, 1, buffer))
        return FALSE;
    {
        PPATH_TABLE pathtable=(PPATH_TABLE)&buffer[track->SectorOffset];
        if (pathtable->NameLen[0]!=1 ||
            Extract2(pathtable->Parent)!=1)
            return FALSE;
        t=Extract4(pathtable->Extent);
        //printf("Root=%u\n", t);
    }
    track->RootSector=t;
    return TRUE;
}

static PDIRELEMENT MakeDir(HFILE hfile, ULONG offset,
                           ULONG start, ULONG sector)
{
    CHAR buffer[SECTOR_SIZE];
    PDIRELEMENT dir;
    PDIRRECORD record;
    ULONG totalsize;
    ULONG pointer;
    ULONG num;
    ULONG maxnum;
    if (!cdromReadLong(hfile, start+sector, 1, buffer))
        return NULL;
    record=(PDIRRECORD)&buffer[offset];
    if (record->NameLen!=1)
        return NULL;
    totalsize=Extract4(record->Size);
    maxnum=totalsize/(sizeof(DIRRECORDEXT)+sizeof(DIRRECORDEXT))+1+3;
    dir=(PDIRELEMENT)M_ALLOC(sizeof(DIRELEMENT)*maxnum);
    num=0;
    FillDirElement(&dir[num], ".", "", 0, 0, ATTR_DIR);
    dir[num++].Type=ELEMENT_UNKNOWN;
    FillDirElement(&dir[num], "..", "", 0, 0, ATTR_DIR);
    dir[num++].Type=ELEMENT_UNKNOWN;
    pointer=0;
    record=(PDIRRECORD)&buffer[offset];
    for(;;)
    {
        if (record->NameLen!=1 ||
            record->Name[0]>=2)
        {
            PDIRRECORDEXT ext=(PDIRRECORDEXT)((PCHAR)record+record->Len-sizeof(DIRRECORDEXT));
            BOOL isdir;
            ULONG sector=Extract4(record->Sector);
            if (num+1<maxnum &&
                record->NameLen+1<=FILE_NAME)
            {
                if (ext->Attr[0] & 0x80)
                    isdir=TRUE;
                else
                    isdir=FALSE;
                if (isdir)
                {
                    PDIRELEMENT tmp;
                    tmp=MakeDir(hfile, offset, start, sector);
                    if (tmp)
                    {
                        FillDirElement(&dir[num], "", "",
                                       0, 0, ATTR_DIR);
                        memcpy(dir[num].Name, record->Name, record->NameLen);
                        dir[num].Name[record->NameLen]='\0';
                        dir[num].Dir=tmp;
                        dir[num++].Type=ELEMENT_DIR;
                    }
                }
                else
                {
                    ULONG size=Extract4(record->Size);
                    /*printf("%s\t%u\t%u\t%u\n\t%x, %x, %x\n", record->Name, sector,
                           size, ext->FileNo, record->Flags, ext->Attr[0], ext->Attr[1]);*/
                    /*{
                        CHAR buffer[SECTOR_SIZE];
                        cdromReadLong(hfile, start+sector, 1, buffer);
                        printf("\t%u\n",
                               (buffer[0x12] & 0x20) ? 2324 : 2048);
                    }*/
                    FillDirElement(&dir[num], "", "",
                                   size,
                                   size, ATTR_FILE);
                    memcpy(dir[num].Name, record->Name, record->NameLen);
                    dir[num].Name[record->NameLen]='\0';
                    dir[num].Sector=sector;
                    dir[num++].Type=ELEMENT_CDIFILE;
                }
            }
        }
        pointer+=record->Len;
        if (pointer>=totalsize)
            break;
        if (pointer>2048)
            break;
        record=(PDIRRECORD)&buffer[offset+pointer];
        if (pointer==2048 ||
            record->Len==0)
        {
            sector++;
            if (!cdromReadLong(hfile, start+sector, 1, buffer))
                break;
            totalsize-=2048;
            pointer=0;
            record=(PDIRRECORD)&buffer[offset];
        }
    }
    dir[num].Name[0]='\0';
    //printf("RETURN (%u)!\n", num+1);
    for(num=0;;num++)
    {
        if (dir[num].Name[0]=='\0')
            break;
        if (dir[num].Type==ELEMENT_CDIFILE)
        {
            if (cdromReadLong(hfile, start+dir[num].Sector, 1, buffer))
            {
                if (buffer[0x12] & 0x20)
                    dir[num].Size=
                        (dir[num].Size/2048)*2324+
                        dir[num].Size%2048;
                /*printf("%s :", dir[num].Name);
                if (buffer[0x12] & 0x02)
                {
                    printf("V ");
                }
                if (buffer[0x12] & 0x04)
                {
                    printf("A ");
                }
                if (buffer[0x12] & 0x08)
                {
                    printf("D ");
                }
                printf("\n");*/
            }
        }
    }
    return dir;
}

PDIRELEMENT cdiMakeDirectory(PTDISK disk)
{
    PDIRELEMENT ret=NULL;
    if (disk->Handle!=(HFILE)-1 &&
        disk->Tracks[0].Type==TRACK_TYPE_CDI)
    {
        ret=MakeDir(disk->Handle,
                    disk->Tracks[0].SectorOffset,
                    disk->Tracks[0].Start,
                    disk->Tracks[0].RootSector);
    }
    //printf("ret=%u\n", ret);
    return ret;
    /*if (!ret)
    {
        ULONG num=0;
        //printf("Generate...\n");
        ret=(PDIRELEMENT)M_ALLOC(sizeof(DIRELEMENT)*3);
        FillDirElement(&ret[num], ".", "", 0, 0, ATTR_DIR);
        ret[num++].Type=ELEMENT_UNKNOWN;
        FillDirElement(&ret[num], "..", "", 0, 0, ATTR_DIR);
        ret[num++].Type=ELEMENT_UNKNOWN;
        ret[num].Name[0]='\0';
    }
    return ret;*/
}

APIRET cdiOpen(PMAINDATA maindata, PTDISK disk, PIFS_OPENCREATE p,
               ULONG sector, ULONG size, PCSZ eaapp)
{
    APIRET rc;
    PCHAR data;
    PTFILE file;
    CHAR buffer[SECTOR_SIZE];
    ULONG sectorsize;
    rc=openVerifyFlags(p->mode, p->flag);
    if (rc)
        return rc;
    if (!cdromReadLong(disk->Handle, disk->Tracks[0].Start+sector, 1, buffer))
        return TONIGY_NOT_READY;
    sectorsize=(buffer[0x12] & 0x20) ? 2324 : 2048;
    data=M_ALLOC(SECTOR_SIZE*SECTORS);
    if (!data)
        return ERROR_NOT_ENOUGH_MEMORY;
    memcpy(data, buffer, SECTOR_SIZE);
    rc=openAllocFile(disk, &file);
    if (rc)
    {
        M_FREE(data);
        return rc;
    }
    strcpy(file->EAApp, eaapp);
    file->Type=FILE_TYPE_DATA;
    file->data.Data.Index=0;
    file->data.Data.Number=disk->Tracks[0].Number;
    file->data.Data.Start=disk->Tracks[0].Start+sector;
    file->data.Data.Sectors=(size+sectorsize-1)/sectorsize;
    file->data.Data.SectorSize=sectorsize;
    file->data.Data.SectorOffset=disk->Tracks[0].SectorOffset;
    file->data.Data.Sector=disk->Tracks[0].Start+sector;
    file->data.Data.Len=1;
    file->data.Data.Data=data;
    p->sffsi.sfi_size=size;
    openSetup(file, p);
    return NO_ERROR;
}
