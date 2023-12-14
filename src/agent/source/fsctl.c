#include "global.h"

#pragma pack(1)
typedef struct  _MY_EASIZEBUF
{
    USHORT  EASize;
    ULONG   ListSize;
} MY_EASIZEBUF, * PMY_EASIZEBUF;
#pragma pack()

/*static VOID PrintDebug(PIFS_FSCTL p)
{
    printf("FSCTL(func=%u)\n", (ULONG)p->func);
}*/

APIRET DoFSCtl(PMAINDATA maindata, PIFS_FSCTL p)
{
    PMY_EASIZEBUF easizebuf;
    //PrintDebug(p);
    if (p->func==FSCTL_MAX_EASIZE)
    {
        p->cb=sizeof(MY_EASIZEBUF);
        if (p->cbmax<sizeof(MY_EASIZEBUF))
            return ERROR_BUFFER_OVERFLOW;
        easizebuf=(PMY_EASIZEBUF)maindata->Data;
        easizebuf->EASize=65535;
        easizebuf->ListSize=65535;
        return NO_ERROR;
    }
    return ERROR_NOT_SUPPORTED;
}
