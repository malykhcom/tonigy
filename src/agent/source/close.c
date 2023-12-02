#include "global.h"

/*static VOID PrintDebug(PIFS_CLOSE p)
{
    printf("CLOSE(type=%u)\n", (ULONG)p->type);
}*/

APIRET DoClose(PMAINDATA maindata, PIFS_CLOSE p)
{
    PTDISK disk;
    PTFILE file;
    //PrintDebug(p);
    if (p->type!=FS_CL_FORSYS)
        return NO_ERROR;
    if (!commonFindDisk(&disk, maindata, p->diskid) ||
        !commonFindFile(&file, disk, p->fileid))
        return ERROR_INVALID_PARAMETER;
    FileFree(file);
    file->ID=ID_EMPTY;
    return NO_ERROR;
}
