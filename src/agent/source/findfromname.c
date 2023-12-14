#include "global.h"

/*static VOID PrintDebug(PIFS_FINDFROMNAME p)
{
    printf("FINDFROMNAME(pos=%u, match=%u)\n",
           p->position,
           (ULONG)p->match);
}*/

APIRET DoFindFromName(PMAINDATA maindata, PIFS_FINDFROMNAME p)
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
                       1+p->position, &match);
    if (rc)
        return rc;
    if (!match)
        return ERROR_NO_MORE_FILES;
    search->NextPos=1+p->position+match;
    p->match=match;
    return NO_ERROR;
}
