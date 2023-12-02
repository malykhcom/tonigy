#include "global.h"

/*static VOID PrintDebug(PIFS_FILEATTRIBUTE p)
{
    printf("FILEATTRIBUTE(name='%s', flag=%u)\n",
           p->name,
           (ULONG)p->flag);
}*/

APIRET DoFileAttribute(PMAINDATA maindata, PIFS_FILEATTRIBUTE p)
{
    PTDISK disk;
    PDIRELEMENT element;
    //PrintDebug(p);
    if (p->flag!=FA_RETRIEVE)
        return ERROR_WRITE_PROTECT;
    if (!commonFindDisk(&disk, maindata, p->diskid))
        return ERROR_INVALID_PARAMETER;
    if (!VERIFY_FIXED(p->name) ||
        !commonIsPath(p->name))
        return ERROR_INVALID_PARAMETER;
    cdromQuery(maindata, disk);
    element=FindFile(disk, p->name+3);
    if (!element)
        return ERROR_FILE_NOT_FOUND;
    p->attr=element->Attr;
    //return element->Attr;
    return NO_ERROR;
}
