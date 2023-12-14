#include "global.h"

/*static VOID PrintDebug(PIFS_FINDNEXT p)
{
    printf("FINDNEXT(%u)\n", (ULONG)p->match);
}*/

APIRET DoFindNext(PMAINDATA maindata, PIFS_FINDNEXT p)
{
    APIRET rc;
    USHORT match;
    PTDISK disk;
    PTSEARCH search;
    //PrintDebug(p);
    if (!commonFindDisk(&disk, maindata, p->diskid) ||
        !commonFindSearch(&search, disk, p->searchid))
        return ERROR_INVALID_PARAMETER;
    match=p->match;
    rc=fileGenFindInfo(search,
                       maindata->Data, p->cb,
                       p->level, p->flags,
                       search->NextPos, &match);
    if (rc)
        return rc;
    if (!match)
        return ERROR_NO_MORE_FILES;
    search->NextPos+=match;
    p->match=match;
    return NO_ERROR;
}
