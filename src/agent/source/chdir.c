#include "global.h"

/*static VOID PrintDebug(PIFS_CHDIR p)
{
    printf("CHDIR(flag=%u, dir='%s')\n",
           (ULONG)p->flag,
           (p->flag==CD_EXPLICIT)?p->dir:"none");
}*/

APIRET DoChDir(PMAINDATA maindata, PIFS_CHDIR p)
{
    //PrintDebug(p);
    switch (p->flag)
    {
    case CD_EXPLICIT:
        {
            PTDISK disk;
            PSZ dir;
            if (!VERIFY_FIXED(p->dir) ||
                !commonIsPath(p->dir))
                return ERROR_INVALID_PARAMETER;
            if (!commonFindDisk(&disk, maindata, p->diskid))
                return ERROR_INVALID_PARAMETER;
            dir=p->dir+3;
            if (FindDirectory(disk, dir, dir+strlen(dir)))
                return NO_ERROR;
            /*if (!dir[0] ||
                commonIsTracksRaw(dir) ||
                commonIsBlocksRaw(dir))
                return NO_ERROR;*/
            return ERROR_PATH_NOT_FOUND;
        }
    case CD_VERIFY:
    case CD_FREE:
        return NO_ERROR;
    }
    return ERROR_NOT_SUPPORTED;
}
