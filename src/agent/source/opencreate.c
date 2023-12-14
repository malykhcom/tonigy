#include "global.h"

APIRET openVerifyFlags(ULONG openmode, USHORT openflag)
{
    switch (openflag & 0xf)
    {
    case OPEN_ACTION_FAIL_IF_EXISTS:
        return ERROR_OPEN_FAILED;
    case OPEN_ACTION_OPEN_IF_EXISTS:
        break;
    case OPEN_ACTION_REPLACE_IF_EXISTS:
        return ERROR_WRITE_PROTECT;
    default:
        return ERROR_INVALID_PARAMETER;
    }
    switch (openmode & 0x7)
    {
    case OPEN_ACCESS_READONLY:
    case OPEN_ACCESS_READWRITE:
        break;
    case OPEN_ACCESS_WRITEONLY:
        //break;
        return ERROR_WRITE_PROTECT;
    default:
        return ERROR_INVALID_PARAMETER;
    }
    return NO_ERROR;
}

static BOOL IsBusy(PTDISK disk, ULONG id)
{
    ULONG t;
    for(t=0;t<T_FILES;t++)
        if (disk->Files[t].ID==id)
            return TRUE;
    return FALSE;
}

APIRET openAllocFile(PTDISK disk, PTFILE* file)
{
    ULONG t;
    for(t=0;t<T_FILES;t++)
        if (disk->Files[t].ID==ID_EMPTY)
        {
            ULONG r;
            for(r=0;r<10;r++)
            {
                ULONG id;
                id=commonGenerateID();
                if (!IsBusy(disk, id))
                {
                    disk->Files[t].ID=id;
                    *file=&disk->Files[t];
                    return NO_ERROR;
                }
                DosSleep(1);
            }
            return ERROR_TS_DATETIME;
        }
    return ERROR_TOO_MANY_OPEN_FILES;
}

VOID openSetup(PTFILE file, PIFS_OPENCREATE p)
{
    file->Ready=TRUE;
    p->fileid=file->ID;
    p->action=FILE_EXISTED;
    p->sffsi.sfi_mtime=GLOBAL_TIME;
    p->sffsi.sfi_mdate=GLOBAL_DATE;
    p->sffsi.sfi_position=0;
    p->sffsi.sfi_type&=STYPE_FCB;
    p->sffsi.sfi_type|=STYPE_FILE;
    p->sffsi.sfi_DOSattr=ATTR_FILE;
}

static APIRET OpenReadme(PMAINDATA maindata, PTDISK disk, PIFS_OPENCREATE p,
                         PCSZ eaapp)
{
    APIRET rc;
    PTFILE file;
    rc=openVerifyFlags(p->mode, p->flag);
    if (rc)
        return rc;
    rc=openAllocFile(disk, &file);
    if (rc)
        return rc;
    strcpy(file->EAApp, eaapp);
    file->Type=FILE_TYPE_README;
    p->sffsi.sfi_size=strlen(maindata->Readme);
    openSetup(file, p);
    return NO_ERROR;
}

static APIRET OpenCdinfo(PTDISK disk, PIFS_OPENCREATE p, PCSZ eaapp)
{
    APIRET rc;
    PSZ info;
    PTFILE file;
    rc=openVerifyFlags(p->mode, p->flag);
    if (rc)
        return rc;
    info=cdromInfo(disk);
    if (!info)
        return ERROR_NOT_ENOUGH_MEMORY;
    rc=openAllocFile(disk, &file);
    if (rc)
    {
        M_FREE(info);
        return rc;
    }
    strcpy(file->EAApp, eaapp);
    file->Type=FILE_TYPE_CDINFO;
    file->data.Info=info;
    p->sffsi.sfi_size=strlen(info);
    openSetup(file, p);
    return NO_ERROR;
}

static APIRET OpenLog(PTDISK disk, PIFS_OPENCREATE p, PCSZ eaapp)
{
    APIRET rc;
    PSZ log;
    PTFILE file;
    rc=openVerifyFlags(p->mode, p->flag);
    if (rc)
        return rc;
    if (!disk->LogFirst)
        return ERROR_FILE_NOT_FOUND;
    log=cdromLog(disk);
    if (!log)
        return ERROR_NOT_ENOUGH_MEMORY;
    rc=openAllocFile(disk, &file);
    if (rc)
    {
        M_FREE(log);
        return rc;
    }
    strcpy(file->EAApp, eaapp);
    file->Type=FILE_TYPE_LOG;
    file->data.Log=log;
    p->sffsi.sfi_size=strlen(log);
    openSetup(file, p);
    return NO_ERROR;
}

static APIRET OpenUnreg(PMAINDATA maindata, PTDISK disk, 
                        PIFS_OPENCREATE p, PCSZ eaapp)
{
    APIRET rc;
    PTFILE file;
    rc=openVerifyFlags(p->mode, p->flag);
    if (rc)
        return rc;
    rc=openAllocFile(disk, &file);
    if (rc)
        return rc;
    strcpy(file->EAApp, eaapp);
    file->Type=FILE_TYPE_UNREG;
    p->sffsi.sfi_size=strlen(maindata->Unreg);
    openSetup(file, p);
    return NO_ERROR;
}

/*static VOID PrintDebug(PIFS_OPENCREATE p)
{
    printf("OPENCREATE(name='%s', mode=%x, flag=%x)\n",
           p->name,
           (ULONG)p->mode,
           (ULONG)p->flag);
}*/

APIRET DoOpenCreate(PMAINDATA maindata, PIFS_OPENCREATE p)
{
    PSZ name;
    PTDISK disk;
    PDIRELEMENT file;
    //ULONG num;
    //BOOL raw;
    //PrintDebug(p);
    if (!VERIFY_FIXED(p->name) ||
        !commonIsPath(p->name))
        return ERROR_INVALID_PARAMETER;
    if (p->mode & OPEN_FLAGS_DASD)
        return ERROR_NOT_SUPPORTED;
    if (!commonFindDisk(&disk, maindata, p->diskid))
        return ERROR_INVALID_PARAMETER;
    //opencreate->fsGenFlag=0;
    //opencreate->oError=0;
    cdromQuery(maindata, disk);
    name=p->name+3;
    file=FindFile(disk, name);
    if (file)
    {
        switch (file->Type)
        {
        case ELEMENT_README:
            return OpenReadme(maindata, disk, p, file->EAApp);
        case ELEMENT_CDINFO:
            return OpenCdinfo(disk, p, file->EAApp);
        case ELEMENT_LOG:
            return OpenLog(disk, p, file->EAApp);
        case ELEMENT_UNREG:
            return OpenUnreg(maindata, disk, p, file->EAApp);
        case ELEMENT_AUDIO_TRACK_RAW:
            return audioOpen(maindata, disk, p, FALSE, file->Index, TRUE,
                             file->EAApp);
        case ELEMENT_AUDIO_TRACK_WAV:
            return audioOpen(maindata, disk, p, FALSE, file->Index, FALSE,
                             file->EAApp);
        case ELEMENT_AUDIO_BLOCK_RAW:
            return audioOpen(maindata, disk, p, TRUE, file->Index, TRUE,
                             file->EAApp);
        /*case ELEMENT_AUDIO_BLOCK_WAV:
            return audioOpen(maindata, disk, p, TRUE, file->Index, FALSE);*/
        case ELEMENT_VIDEO:
            return videoOpen(maindata, disk, p, file->Index, file->EAApp);
        case ELEMENT_ISO:
            return isoOpen(maindata, disk, p, file->Index, file->EAApp);
        case ELEMENT_CDIFILE:
            return cdiOpen(maindata, disk, p, file->Sector, file->Size, file->EAApp);
        }
        return ERROR_ACCESS_DENIED;
    }
    switch (p->flag & 0xf0)
    {
    case OPEN_ACTION_FAIL_IF_NEW:
        return ERROR_OPEN_FAILED;
    case OPEN_ACTION_CREATE_IF_NEW:
        return ERROR_WRITE_PROTECT;
    }
    return ERROR_INVALID_PARAMETER;
}
