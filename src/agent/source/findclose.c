#include "global.h"

/*static VOID PrintDebug(PIFS_FINDCLOSE p)
{
    printf("FINDCLOSE()\n");
}*/

APIRET DoFindClose(PMAINDATA maindata, PIFS_FINDCLOSE p)
{
    PTDISK disk;
    PTSEARCH search;
    //PrintDebug(p);
    if (!commonFindDisk(&disk, maindata, p->diskid) ||
        !commonFindSearch(&search, disk, p->searchid))
        return ERROR_INVALID_PARAMETER;
    SearchFree(search);
    return NO_ERROR;
}
