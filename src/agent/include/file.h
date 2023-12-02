#ifndef __FILE_H__
#define __FILE_H__

BOOL fileNameMask(PCSZ, PCSZ);
BOOL fileAttrMask(USHORT, USHORT);
APIRET fileGenInfo(PCHAR, USHORT,
                USHORT, PFILEINFO,
                BOOL, ULONG,
                PUSHORT);
APIRET fileGenFindInfo(PTSEARCH,
                    PCHAR, USHORT,
                    USHORT, USHORT,
                    ULONG, PUSHORT);

_inline VOID SearchFree(PTSEARCH search)
{
    ULONG t;
    for(t=0;t<search->FilesNum;t++)
        if (search->Files[t].EA)
            M_FREE(search->Files[t].EA);
    M_FREE(search->Files);
    search->ID=ID_EMPTY;
}
_inline VOID FileFree(PTFILE file)
{
    switch (file->Type)
    {
    case FILE_TYPE_CDINFO:
        M_FREE(file->data.Info);
        break;
    case FILE_TYPE_LOG:
        M_FREE(file->data.Log);
        break;
    case FILE_TYPE_AUDIO:
        audioFileFree(file);
        break;
    case FILE_TYPE_DATA:
        dataFileFree(file);
        break;
    }
}

_inline VOID LogFree(PTDISK disk)
{
    PLOGRECORD record;
    record=disk->LogFirst;
    while (record)
    {
        PLOGRECORD tmp;
        tmp=record;
        record=record->Next;
        M_FREE(tmp);
    }
    disk->LogFirst=NULL;
    disk->LogLast=NULL;
    disk->LogRecords=0;
}

_inline VOID DirElementFree(PDIRELEMENT element)
{
    ULONG t;
    if (element==NULL)
        return;
    for(t=0;;t++)
    {
        if (element[t].Name[0]=='\0')
            break;
        if (element[t].Type==ELEMENT_DIR)
        {
            DirElementFree(element[t].Dir);
        }
    }
    M_FREE(element);
}

_inline VOID FillDirElement(PDIRELEMENT element, PCSZ name, PCSZ eaapp,
                            ULONG size, ULONG alloc,
                            USHORT attr)
{
    strcpy(element->Name, name);
    strcpy(element->EAApp, eaapp);
    element->Size=size;
    element->Alloc=alloc;
    element->Attr=attr;
}

PDIRELEMENT FindDirectory(PTDISK, PCSZ, PCSZ);

PDIRELEMENT FindFile(PTDISK, PCSZ);

#endif
