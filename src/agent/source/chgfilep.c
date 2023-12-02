#include "global.h"

/*static VOID PrintDebug(PIFS_CHGFILEPTR p)
{
    printf("CHGFILEPTR(type=%u offset=%d)\n",
           (ULONG)p->type,
           p->offset);
}*/

APIRET DoChgFilePtr(PMAINDATA maindata, PIFS_CHGFILEPTR p)
{
    //PrintDebug(p);
    switch (p->type)
    {
    case FILE_BEGIN:
        p->sffsi.sfi_position=p->offset;
        break;
    case FILE_CURRENT:
        p->sffsi.sfi_position+=p->offset;
        break;
    case FILE_END:
        p->sffsi.sfi_position=p->sffsi.sfi_size+p->offset;
        break;
    }
    return NO_ERROR;
}
