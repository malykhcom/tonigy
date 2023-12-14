#include "global.h"

static APIRET SetInfo(PMAINDATA maindata, PIFS_FILEINFO p,
                      PTDISK disk, PTFILE file)
{
    APIRET rc;
    if (p->level==FIL_STANDARD)
        return ERROR_WRITE_PROTECT;
    if (p->level!=FIL_QUERYEASIZE)
        return ERROR_INVALID_LEVEL;
    //printf("  Set(appname=%s)\n", app);
    rc=eaAdd(maindata->HiniEA, file->EAApp, (PFEALIST)maindata->Data);
    return rc;
}

static APIRET GetInfo(PMAINDATA maindata, PIFS_FILEINFO p,
                      PTDISK disk, PTFILE file)
{
    if (p->level==FIL_QUERYEASFROMLIST ||
        p->level==4)
    {
        APIRET rc;
        PGEALIST gealist;
        //printf("  Get(appname=%s)\n", app);
        if (p->level==FIL_QUERYEASFROMLIST)
        {
            rc=eaCopyGealist(&gealist, maindata->Data);
            if (rc)
                return rc;
        }
        else
            gealist=NULL;
        rc=eaGet(maindata->HiniEA, file->EAApp, gealist,
                 maindata->Data, sizeof(FSDATA));
        if (gealist)
            M_FREE(gealist);
        return rc;
    }
    if (p->level==FIL_STANDARD ||
        p->level==FIL_QUERYEASIZE)
    {
        FILEINFO fileinfo;
        USHORT tmp;
        fileinfo.Name[0]='\0';
        fileinfo.Size=p->sffsi.sfi_size;
        switch (file->Type)
        {
        case FILE_TYPE_AUDIO:
            if (file->data.Audio.Raw)
                fileinfo.Alloc=fileinfo.Size;
            else
                fileinfo.Alloc=fileinfo.Size-sizeof(WAVHEADER);
            break;
        case FILE_TYPE_DATA:
            fileinfo.Alloc=file->data.Data.Sectors*SECTOR_SIZE;
            break;
        default:
            fileinfo.Alloc=0;
        }
        fileinfo.Attr=ATTR_FILE;
        if (p->level==FIL_QUERYEASIZE)
        {
            fileinfo.EASize=eaGetSize(maindata->HiniEA, file->EAApp);
        }
        return fileGenInfo(maindata->Data, p->cb,
                           p->level, &fileinfo,
                           FALSE, 0, &tmp);
    }
    return ERROR_INVALID_LEVEL;
}


/*static VOID PrintDebug(PIFS_FILEINFO p)
{
    printf("FILEINFO(flag=%u, level=%u)\n",
           (ULONG)p->flag,
           (ULONG)p->level);
}*/

APIRET DoFileInfo(PMAINDATA maindata, PIFS_FILEINFO p)
{
    PTDISK disk;
    PTFILE file;
    //PrintDebug(p);
    if (!commonFindDisk(&disk, maindata, p->diskid) ||
        !commonFindFile(&file, disk, p->fileid))
        return ERROR_INVALID_PARAMETER;
    if (!file->Ready)
    {
        //printf("  not ready!\n");
        return TONIGY_NOT_READY;
    }
    if (p->flag!=FI_RETRIEVE)
    {
        return SetInfo(maindata, p, disk, file);
    }
    return GetInfo(maindata, p, disk, file);
}
