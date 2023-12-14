#include "global.h"

APIRET DoDelete(PMAINDATA maindata, PIFS_DELETE p)
{
    PTDISK disk;
    PDIRELEMENT file;
    if (!VERIFY_FIXED(p->name) ||
        !commonIsPath(p->name))
        return ERROR_INVALID_PARAMETER;
    if (!commonFindDisk(&disk, maindata, p->diskid))
        return ERROR_INVALID_PARAMETER;
    file=FindFile(disk, &p->name[3]);
    if (file && file->Type==ELEMENT_LOG)
    {
        LogFree(disk);
        file->Name[0]='\0';
        return NO_ERROR;
    }
    return ERROR_WRITE_PROTECT;
}
