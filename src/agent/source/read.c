#include "global.h"

static APIRET ReadReadme(PMAINDATA maindata, USHORT len, ULONG position)
{
    memcpy(maindata->Data, maindata->Readme+position, len);
    return NO_ERROR;
}

static APIRET ReadCdinfo(PCHAR data, USHORT len, ULONG position, PTFILE file)
{
    memcpy(data, file->data.Info+position, len);
    return NO_ERROR;
}

static APIRET ReadLog(PCHAR data, USHORT len, ULONG position, PTFILE file)
{
    memcpy(data, file->data.Log+position, len);
    return NO_ERROR;
}

static APIRET ReadUnreg(PMAINDATA maindata, USHORT len, ULONG position)
{
    memcpy(maindata->Data, maindata->Unreg+position, len);
    return NO_ERROR;
}

/*static VOID PrintDebug(PIFS_READ p)
{
    printf("READ(len=%u)\n", (ULONG)p->cb);
}*/

APIRET DoRead(PMAINDATA maindata, PIFS_READ p)
{
    APIRET rc;
    PTDISK disk;
    PTFILE file;
    USHORT len;
    //PrintDebug(p);
    if (!commonFindDisk(&disk, maindata, p->diskid) ||
        !commonFindFile(&file, disk, p->fileid))
        return ERROR_INVALID_PARAMETER;
    if (!file->Ready)
    {
        //printf("  not ready!\n");
        return TONIGY_NOT_READY;
    }
    len=p->cb;
    if (p->sffsi.sfi_position>p->sffsi.sfi_size)
        len=0;
    else
        len=min(len, p->sffsi.sfi_size-p->sffsi.sfi_position);
    //printf("  len=%u\n", (ULONG)len);
    switch (file->Type)
    {
    case FILE_TYPE_README:
        rc=ReadReadme(maindata, len, p->sffsi.sfi_position);
        break;
    case FILE_TYPE_CDINFO:
        rc=ReadCdinfo(maindata->Data, len, p->sffsi.sfi_position, file);
        break;
    case FILE_TYPE_LOG:
        rc=ReadLog(maindata->Data, len, p->sffsi.sfi_position, file);
        break;
    case FILE_TYPE_UNREG:
        rc=ReadUnreg(maindata, len, p->sffsi.sfi_position);
        break;
    case FILE_TYPE_AUDIO:
        rc=audioRead(maindata, maindata->Data, len, p->sffsi.sfi_position,
                     disk, file);
        break;
    case FILE_TYPE_DATA:
        rc=dataRead(maindata->Data, len, p->sffsi.sfi_position,
                     disk, file);
        break;
    default:
        rc=TONIGY_NOT_READY;
    }
    if (rc)
        return rc;
    p->cb=len;
    p->sffsi.sfi_position+=len;
    return NO_ERROR;
};

