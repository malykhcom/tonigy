#include "global.h"

#pragma pack(1)
typedef struct
{
  ULONG  Create;
  ULONG  Access;
  ULONG  Write;
  ULONG  cbFile;
  ULONG  cbFileAlloc;
  USHORT fDOSattr;
} COMMON1, * PCOMMON1;

typedef struct
{
  UCHAR  cbName;
  UCHAR  szName[1];
} COMMON2, * PCOMMON2;
#pragma pack(0)

BOOL fileNameMask(PCSZ mask, PCSZ name)
{
    switch (*mask)
    {
    case '\0':
        while (*name=='.')
            name++;
        return
            !*name;
    case '*':
        return
            (*name && fileNameMask(mask, name+1)) ||
            fileNameMask(mask+1, name);
    case '?':
        return
            (!*name) ||
            ((*name=='.') && fileNameMask(mask+1, name)) ||
            ((*name!='.') &&
             fileNameMask(mask+1, name+1));
    case '.':
        return
            ((!*name) && fileNameMask(mask+1, name)) ||
            ((*name=='.') && fileNameMask(mask+1, name+1));
    default:
        return
            (commonCharToUpper(*mask)==commonCharToUpper(*name)) &&
            fileNameMask(mask+1, name + 1);
    }
}

BOOL fileAttrMask(USHORT mask, USHORT attr)
{
    USHORT musthave;
    USHORT mayhave;
    musthave=mask>>8;
    mayhave=mask & 0x00FF;
    //  if (!musthave)
    /*printf("attr: %xh, may: %xh, must: %xh (",
           (ULONG)attr,
           (ULONG)mayhave,
           (ULONG)musthave);*/
    mayhave|=(FILE_READONLY | FILE_ARCHIVED);
    /*printf("may: %xh)\n",
           (ULONG)mayhave);*/
    if (attr & ~mayhave)
        return FALSE;
    if (musthave & ~attr)
        return FALSE;
    return TRUE;
}

APIRET fileGenInfo(PCHAR data, USHORT len,
                   USHORT level, PFILEINFO file,
                   BOOL isposition, ULONG position,
                   PUSHORT bsize)
{
    PCOMMON1 common1;
    PCOMMON2 common2;
    USHORT namelen=strlen(file->Name);
    USHORT buffersize;
    
    buffersize=0;
    if (isposition)
        buffersize+=sizeof(ULONG);
    buffersize+=sizeof(COMMON1);
    if (level==FIL_QUERYEASFROMLIST)
        if (file->EA)
            buffersize+=file->EA->cbList;
        else
            buffersize+=sizeof(ULONG);
    if (level==FIL_QUERYEASIZE)
        buffersize+=sizeof(ULONG);
    if (namelen)
        buffersize+=namelen+sizeof(COMMON2);
    if (len<buffersize)
        return ERROR_BUFFER_OVERFLOW;
    *bsize=buffersize;
    if (isposition)
    {
        *(PULONG)data=position;
        data+=sizeof(ULONG);
    }
    common1=(PCOMMON1)data;
    memset(common1, 0, sizeof(COMMON1));
    common1->Write=GLOBAL_DATE+(GLOBAL_TIME<<16);
    common1->cbFile=file->Size;
    common1->cbFileAlloc=file->Alloc;
    common1->fDOSattr=file->Attr;
    data+=sizeof(COMMON1);
    if (level==FIL_QUERYEASFROMLIST)
    {
        if (file->EA)
        {
            memcpy(data, file->EA, file->EA->cbList);
            data+=file->EA->cbList;
        }
        else
        {
            *(PULONG)data=sizeof(ULONG);
            data+=sizeof(ULONG);
        }
    }
    if (level==FIL_QUERYEASIZE)
    {
        *(PULONG)data=file->EASize;
        data+=sizeof(ULONG);
    }
    if (namelen)
    {
        common2=(PCOMMON2)data;
        common2->cbName=namelen;
        strcpy(common2->szName, file->Name);
    }
    return NO_ERROR;
}

APIRET fileGenFindInfo(PTSEARCH search,
                       PCHAR data, USHORT len,
                       USHORT level, USHORT flags,
                       ULONG startpos, PUSHORT match)
{
    USHORT t;
    USHORT counter;
    counter=0;
    for(t=0;t<*match;t++)
    {
        ULONG pos;
        APIRET rc;
        USHORT bsize;
        pos=t+startpos;
        if (pos>=search->FilesNum)
            break;
        rc=fileGenInfo(data, len,
                       level, &search->Files[pos],
                       flags, pos,
                       &bsize);
        if (rc)
            return rc;
        data+=bsize;
        len-=bsize;
        counter++;
    }
    *match=counter;
    return NO_ERROR;
}

PDIRELEMENT FindDirectory(PTDISK disk, PCSZ name, PCSZ lastchar)
{
    PCHAR dirstart;
    PDIRELEMENT dir;
    if (name>lastchar)
        return NULL;
    if (name==lastchar)
        return disk->Root;
    dirstart=name;
    dir=disk->Root;
    for(;;)
    {
        if (name>lastchar)
            return dir;
        if (name==lastchar ||
            *name=='\\')
        {
            ULONG t;
            if (name==dirstart)
                return NULL;
            //printf("%s, %u\n", dirstart, name-dirstart);
            for(t=0;;t++)
            {
                if (!dir[t].Name[0])
                    return NULL;
                if (dir[t].Type==ELEMENT_DIR &&
                    strlen(dir[t].Name)==name-dirstart &&
                    commonMemCompare(dir[t].Name, dirstart, name-dirstart))
                {
                    dir=dir[t].Dir;
                    name++;
                    dirstart=name;
                    break;
                }
            }
        }
        else
            name++;
    }
}

static DIRELEMENT root=
{
    "",
    "",
    0,
    0,
    ATTR_DIR,
    ELEMENT_UNKNOWN
};

PDIRELEMENT FindFile(PTDISK disk, PCSZ name)
{
    PCHAR file;
    PCHAR dirend;
    PDIRELEMENT dir;
    ULONG t;
    //printf("fullname: %s\n", name);
    if (name[0]=='\0')
        return &root;
    dirend=strrchr(name, '\\');
    if (dirend==NULL)
    {
        dirend=name;
        file=name;
    }
    else
    {
        file=dirend+1;
    }
    //printf("name: %s\n", file);
    dir=FindDirectory(disk, name, dirend);
    if (dir)
    {
        for(t=0;;t++)
        {
            if (dir[t].Name[0]=='\0')
                break;
            if (/*dir[t].Type!=ELEMENT_DIR &&*/
                commonStrCompare(dir[t].Name, file))
                return &dir[t];
        }
    }
    return NULL;
}
