#include "global.h"

static APIRET SetInfo(PMAINDATA maindata, PIFS_PATHINFO p)
{
    APIRET rc;
    PTDISK disk;
    PDIRELEMENT element;
    if (!commonFindDisk(&disk, maindata, p->diskid))
        return ERROR_INVALID_PARAMETER;
    if (p->level==FIL_STANDARD)
        return ERROR_WRITE_PROTECT;
    if (p->level!=FIL_QUERYEASIZE)
        return ERROR_INVALID_LEVEL;
    cdromQuery(maindata, disk);
    element=FindFile(disk, p->name+3);
    if (!element)
        return ERROR_FILE_NOT_FOUND;
    //printf("  Set(appname=%s)\n", app);
    rc=eaAdd(maindata->HiniEA, element->EAApp, (PFEALIST)maindata->Data);
    return rc;
}

static APIRET GetInfo(PMAINDATA maindata, PIFS_PATHINFO p)
{
    PTDISK disk;
    PDIRELEMENT element;
    if (!commonFindDisk(&disk, maindata, p->diskid))
        return ERROR_INVALID_PARAMETER;
    cdromQuery(maindata, disk);
    element=FindFile(disk, p->name+3);
    if (!element)
        return ERROR_FILE_NOT_FOUND;
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
        rc=eaGet(maindata->HiniEA, element->EAApp,
                 gealist, maindata->Data, sizeof(FSDATA));
        if (gealist)
            M_FREE(gealist);
        if (rc==ERROR_BUFFER_OVERFLOW)
            return ERROR_NOT_ENOUGH_MEMORY;
        return rc;
    }
    if (p->level==FIL_STANDARD ||
        p->level==FIL_QUERYEASIZE)
    {
        PSZ name;
        FILEINFO file;
        USHORT tmp;
        name=p->name+3;
        file.Name[0]='\0';
        if (p->level==FIL_QUERYEASIZE)
        {
            file.EASize=eaGetSize(maindata->HiniEA, element->EAApp);
        }
        file.Size=element->Size;
        file.Alloc=element->Alloc;
        file.Attr=element->Attr;
        return fileGenInfo(maindata->Data, p->cb,
                           p->level, &file,
                           FALSE, 0, &tmp);
    }
    return ERROR_INVALID_LEVEL;
}

/*static VOID PrintDebug(PIFS_PATHINFO p)
{
    printf("PATHINFO(name='%s', level=%u, flag=%u)\n",
           p->name,
           (ULONG)p->level,
           (ULONG)p->flag);
}*/

APIRET DoPathInfo(PMAINDATA maindata, PIFS_PATHINFO p)
{
    //PrintDebug(p);
    if (!VERIFY_FIXED(p->name) ||
        !commonIsPath(p->name))
        return ERROR_INVALID_PARAMETER;
    if (p->flag!=PI_RETRIEVE)
    {
        return SetInfo(maindata, p);
    }
    return GetInfo(maindata, p);
}

