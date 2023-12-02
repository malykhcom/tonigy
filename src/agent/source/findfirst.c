#include "global.h"

static BOOL IsBusy(PTDISK disk, ULONG id)
{
    ULONG t;
    for(t=0;t<T_SEARCHES;t++)
        if (disk->Searches[t].ID==id)
            return TRUE;
    return FALSE;
}

static APIRET AllocSearch(PTDISK disk, PTSEARCH* search)
{
    ULONG t;
    for(t=0;t<T_SEARCHES;t++)
        if (disk->Searches[t].ID==ID_EMPTY)
        {
            ULONG r;
            for(r=0;r<10;r++)
            {
                ULONG id;
                id=commonGenerateID();
                if (!IsBusy(disk, id))
                {
                    disk->Searches[t].ID=id;
                    *search=&disk->Searches[t];
                    return NO_ERROR;
                }
                DosSleep(1);
            }
            return ERROR_TS_DATETIME;
        }
    return ERROR_NO_MORE_SEARCH_HANDLES;
}

static CHAR TmpEA[65536];

static CHAR BadSymbols[]="|+=:;,.\\/[]<>\"";

static BOOL is83(PCSZ name)
{
    PCHAR p;
    BOOL ext=FALSE;
    ULONG i=0;
    if (!strcmp(name, ".") ||
        !strcmp(name, ".."))
        return TRUE;
    for(p=name;*p;p++)
    {
        i++;
        if (ext)
        {
            if (*p=='.')
                return FALSE;
            if (i>3)
                return FALSE;
            if (strchr(BadSymbols, *p))
                return FALSE;
        }
        else
        {
            if (*p=='.')
            {
                ext=TRUE;
                i=0;
            }
            else
            {
                if (i>8)
                    return FALSE;
                if (strchr(BadSymbols, *p))
                    return FALSE;
            }
        }
    }
    return TRUE;
}

static BOOL AddFile(PFILEINFO files, PULONG num,
                    PCSZ filemask, USHORT attrmask,
                    PCSZ name, ULONG size, ULONG alloc, USHORT attr)
{
    if (!is83(name))
        attr|=FILE_NON83;
    files=&files[*num];
    //printf("mask: %s, name=%s\n", filemask, name);
    //printf("attrmask: %xh, name=%xh\n", (ULONG)attrmask, (ULONG)attr);
    if (fileNameMask(filemask, name) && fileAttrMask(attrmask, attr))
    {
        strcpy(files->Name, name);
        if (!(attrmask & FILE_NON83))
            strupr(files->Name);
        files->Size=size;
        files->Alloc=alloc;
        files->Attr=attr;
        files->EASize=0;
        files->EA=NULL;
        *num=*num+1;
        //printf("TRUE!\n");
        return TRUE;
    }
    //printf("FALSE!\n");
    return FALSE;
}

/*static VOID PrintDebug(PIFS_FINDFIRST p)
{
    printf("FINDFIRST(name='%s', level=%u, flags=%u, match=%u, attr=%u)\n",
           p->name,
           (ULONG)p->level,
           (ULONG)p->flags,
           (ULONG)p->match,
           (ULONG)p->attr);
}*/

APIRET DoFindFirst(PMAINDATA maindata, PIFS_FINDFIRST p)
{
    APIRET rc;
    PTDISK disk;
    PTSEARCH search;
    PDIRELEMENT direlement;
    PCHAR dirstart;
    PCHAR dirend;
    PCHAR mask;
    //PrintDebug(p);
    if (p->level!=FIL_STANDARD &&
        p->level!=FIL_QUERYEASIZE &&
        p->level!=FIL_QUERYEASFROMLIST)
        return ERROR_INVALID_LEVEL;
    if (!p->match)
        return ERROR_INVALID_PARAMETER;
    if (!commonFindDisk(&disk, maindata, p->diskid))
        return ERROR_INVALID_PARAMETER;
    rc=AllocSearch(disk, &search);
    if (rc)
        return rc;
    dirstart=p->name+3;
    dirend=strrchr(dirstart, '\\');
    if (dirend==NULL)
    {
        dirend=dirstart;
        mask=dirstart;
    }
    else
    {
        mask=dirend+1;
    }
    cdromQuery(maindata, disk);
    direlement=FindDirectory(disk, dirstart, dirend);
    if (direlement==NULL)
        return ERROR_INVALID_PATH;
    {
        ULONG t;
        ULONG num=0;
        for(t=0;;t++)
            if (direlement[t].Name[0]=='\0')
                break;
        if (!t)
            return ERROR_NO_MORE_FILES;
        search->Files=M_ALLOC(t*sizeof(FILEINFO));
        if (!search->Files)
        {
            search->ID=ID_EMPTY;
            return ERROR_NOT_ENOUGH_MEMORY;
        }
        //printf("%u files\n", t);
        for(t=0;;t++)
        {
            if (direlement[t].Name[0]=='\0')
                break;
            if (AddFile(search->Files, &num, mask, p->attr,
                        direlement[t].Name,
                        direlement[t].Size,
                        direlement[t].Alloc,
                        direlement[t].Attr) &&
                (p->level==FIL_QUERYEASIZE ||
                 p->level==FIL_QUERYEASFROMLIST))
            {
                PCSZ app;
                PFILEINFO fileinfo;
                fileinfo=&search->Files[num-1];
                app=direlement[t].EAApp;
                fileinfo->EASize=eaGetSize(maindata->HiniEA,
                                           app);
                if (p->level==FIL_QUERYEASFROMLIST &&
                    !eaGet(maindata->HiniEA, app, (PGEALIST)maindata->Data,
                           TmpEA, sizeof(TmpEA)))
                {
                    ULONG len;
                    PFEALIST fealist;
                    fealist=(PFEALIST)TmpEA;
                    len=fealist->cbList;
                    if (len>sizeof(ULONG))
                    {
                        fileinfo->EA=M_ALLOC(len);
                        if (fileinfo->EA)
                            memcpy(fileinfo->EA, fealist, len);
                    }
                }
            }
        }
        search->FilesNum=num;
    }
    {
        APIRET rc;
        USHORT match;
        match=p->match;
        rc=fileGenFindInfo(search, maindata->Data, p->cb,
                           p->level, p->flags,
                           0, &match);
        if (rc)
        {
            SearchFree(search);
            return rc;
        }
        //printf("  match=%u\n", (ULONG)match);
        if (!match)
        {
            SearchFree(search);
            return ERROR_NO_MORE_FILES;
        }
        search->NextPos=match;
        p->match=match;
        p->searchid=search->ID;
    }
    return NO_ERROR;
}

