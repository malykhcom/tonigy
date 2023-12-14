#include "global.h"

APIRET Alloc(PMAINDATA maindata, PIFS_FSINFO p)
{
    PFSALLOCATE pallocate;
    // Watcom Bug?
    // if (fsinfo->cbData<sizeof(FSALLOCATE))
    if (p->cb<18)
        return ERROR_BUFFER_OVERFLOW;
    // fsinfo->cbData=sizeof(FSINFO);
    p->cb=18;
    pallocate=(PFSALLOCATE)maindata->Data;
    pallocate->idFileSystem=0;
    pallocate->cSectorUnit=1;
    pallocate->cUnit=0;
    {
        PTDISK disk;
        ULONG size=0;
        if (commonFindDisk(&disk, maindata, p->diskid) &&
            cdromQuery(maindata, disk))
        {
            /*ULONG t;
            ULONG tracksnum;
            tracksnum=disk->TracksNum;
            for(t=0;t<tracksnum;t++)
                if (disk->Tracks[t].Type==TRACK_TYPE_AUDIO)
                size+=disk->Tracks[t].Len;*/
            size=disk->Sectors;
        }
        pallocate->cUnit=size;
    }
    pallocate->cUnitAvail=0;
    pallocate->cbSector=SECTOR_SIZE;
    return NO_ERROR;
}

static CHAR label[]="Tonigy %u";

static APIRET VolSer(PMAINDATA maindata, PIFS_FSINFO p)
{
    PFSINFO pinfo;
    // Watcom Bug?
    // if (fsinfo->cbData<sizeof(FSINFO))
    if (p->cb<17)
        return ERROR_BUFFER_OVERFLOW;
    // fsinfo->cbData=sizeof(FSINFO);
    p->cb=17;
    pinfo=(PFSINFO)maindata->Data;
    // memset(pinfo, 0, sizeof(FSINFO));
    memset(pinfo, 0, 17);
    pinfo->vol.cch=strlen(label);
    _snprintf(pinfo->vol.szVolLabel, sizeof(pinfo->vol.szVolLabel),
              label, p->diskid);
    return NO_ERROR;
}

/*static VOID PrintDebug(PIFS_FSINFO p)
{
    printf("FSINFO(flag=%u, level=%u)\n",
           p->flag, p->level);
}*/

APIRET DoFSInfo(PMAINDATA maindata, PIFS_FSINFO p)
{
    //PrintDebug(p);
    if (p->flag!=INFO_RETRIEVE)
        return ERROR_NOT_SUPPORTED;
    switch (p->level)
    {
    case FSIL_ALLOC:
        return Alloc(maindata, p);
    case FSIL_VOLSER:
        return VolSer(maindata, p);
    }
    return ERROR_NOT_SUPPORTED;
}
