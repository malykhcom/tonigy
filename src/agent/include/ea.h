#ifndef __EA_H__
#define __EA_H__

/*APIRET eaAppFromName(PCHAR, ULONG, PTDISK, PSZ);
 APIRET eaAppFromFile(PCHAR, ULONG, PTDISK, PTFILE);*/

_inline APIRET eaMake(PCHAR buffer, ULONG len, PCSZ name, PTDISK disk)
{
    _snprintf(buffer, len, "%s_%u", name, disk->Drive);
    return NO_ERROR;
}

APIRET eaAdd(HINI, PSZ, PFEALIST);
_inline APIRET eaCopyGealist(PGEALIST* gealist, PCHAR data)
{
    PGEALIST tmp;
    tmp=(PGEALIST)data;
    *gealist=M_ALLOC(tmp->cbList);
    if (*gealist)
    {
        memcpy(*gealist, data, tmp->cbList);
        return NO_ERROR;
    }
    return ERROR_NOT_ENOUGH_MEMORY;
}
APIRET eaGet(HINI, PSZ, PGEALIST, PCHAR, ULONG);
ULONG eaGetSize(HINI, PSZ);

#endif
