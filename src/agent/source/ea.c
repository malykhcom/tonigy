#include "global.h"

/*APIRET readmeMakeEA(PCHAR buffer, ULONG len, PTDISK disk)
{
    _snprintf(buffer, len, "%s_%u",
              commonReadmeName, disk->Drive);
    return NO_ERROR;
}

APIRET cdinfoMakeEA(PCHAR buffer, ULONG len, PTDISK disk)
{
    _snprintf(buffer, len, "%s_%u",
              commonCdinfoName, disk->Drive);
    return NO_ERROR;
}

APIRET logMakeEA(PCHAR buffer, ULONG len, PTDISK disk)
{
    _snprintf(buffer, len, "%s_%u",
              commonLogName, disk->Drive);
    return NO_ERROR;
}

APIRET unregMakeEA(PCHAR buffer, ULONG len, PTDISK disk)
{
    _snprintf(buffer, len, "unreg_%u", disk->drive);
    return NO_ERROR;
}

APIRET MakeRoot(ULONG drive, PCHAR buffer, ULONG len)
{
    _snprintf(buffer, len, "root_%u", drive);
    return NO_ERROR;
}

static APIRET MakeTracksRaw(ULONG drive, PCHAR buffer, ULONG len)
{
    _snprintf(buffer, len, "%s_%u",
              commonTracksRaw, drive);
    return NO_ERROR;
}

static APIRET MakeBlocksRaw(ULONG drive, PCHAR buffer, ULONG len)
{
    _snprintf(buffer, len, "%s_%u",
              commonBlocksRaw, drive);
    return NO_ERROR;
}*/

APIRET eaAdd(HINI hini, PSZ app, PFEALIST fealist)
{
    ULONG left;
    PFEA fea;
    left=fealist->cbList;
    if (left<sizeof(ULONG))
        return ERROR_EA_LIST_INCONSISTENT;
    if (left==sizeof(ULONG))
        return NO_ERROR;
    if (app[0]=='\0')
        return NO_ERROR;
    fea=fealist->list;
    left-=sizeof(ULONG);
    while (left)
    {
        ULONG len;
        PSZ key;
        PCHAR data;
        if (left<sizeof(FEA))
            return ERROR_EA_LIST_INCONSISTENT;
        len=sizeof(FEA)+fea->cbName+1+fea->cbValue;
        if (left<len)
            return ERROR_EA_LIST_INCONSISTENT;
        key=(PCHAR)fea+sizeof(FEA);
        data=(PCHAR)fea+sizeof(FEA)+fea->cbName+1;
        //printf("  adding '%s'\n", key);
        if (fea->cbValue)
        {
            PrfWriteProfileData(hini, app, key, data, fea->cbValue);
        }
        else
        {
            PrfWriteProfileData(hini, app, key, NULL, 0);
        }
        left-=len;
        fea=(PFEA)((PCHAR)fea+len);
    }
    return NO_ERROR;
}

static APIRET SaveEA(PSZ name, PCHAR ea, ULONG ealen, PULONG cb, PFEA* fea)
{
    ULONG namelen;
    ULONG len;
    namelen=strlen(name);
    len=sizeof(FEA)+namelen+1+ealen;
    if (*cb<len)
        return ERROR_BUFFER_OVERFLOW;
    (*fea)->fEA=0;
    (*fea)->cbName=namelen;
    (*fea)->cbValue=ealen;
    memcpy((PCHAR)*fea+sizeof(FEA), name, namelen+1);
    if (ealen)
        memcpy((PCHAR)*fea+sizeof(FEA)+namelen+1, ea, ealen);
    *fea=(PFEA)((PCHAR)*fea+len);
    *cb=*cb-len;
    return NO_ERROR;
}

static APIRET GetEA(HINI hini, PSZ app, PSZ name, PULONG cb, PFEA* fea)
{
    static CHAR ea[USHRT_MAX];
    ULONG ealen;
    //printf("  getting '%s'\n", name);
    if (app[0]!='\0' &&
        PrfQueryProfileSize(hini, app, name, &ealen) &&
        ealen<sizeof(ea) &&
        PrfQueryProfileData(hini, app, name, ea, &ealen))
    {
        //printf("    loaded!\n");
        return SaveEA(name, ea, ealen, cb, fea);
    }
    return SaveEA(name, 0, 0, cb, fea);
}

static CHAR Keys[USHRT_MAX];

APIRET eaGet(HINI hini, PSZ app, PGEALIST gealist, PCHAR data, ULONG cb)
{
    APIRET rc;
    PFEALIST fealist;
    PFEA fea;
    PGEA gea;
    ULONG left;
    if (cb<sizeof(ULONG))
        return ERROR_BUFFER_OVERFLOW;
    fealist=(PFEALIST)data;
    fea=fealist->list;
    if (gealist)
    {
        left=gealist->cbList;
        if (left<sizeof(ULONG))
            return ERROR_EA_LIST_INCONSISTENT;
        gea=gealist->list;
        left-=sizeof(ULONG);
        while (left)
        {
            ULONG namelen;
            if (left<sizeof(GEA))
                return ERROR_EA_LIST_INCONSISTENT;
            namelen=gea->cbName;
            if (left<namelen+2 ||
                gea->szName[namelen])
                return ERROR_EA_LIST_INCONSISTENT;
            rc=GetEA(hini, app, gea->szName, &cb, &fea);
            if (rc)
                return rc;
            left-=namelen+2;
            gea=(PGEA)((PCHAR)gea+namelen+2);
        }
    }
    else
    {
        ULONG len=sizeof(Keys);
        if (app[0]!='\0' &&
            PrfQueryProfileData(hini, app, NULL, Keys, &len))
        {
            PSZ key;
            key=Keys;
            while (key[0])
            {
                rc=GetEA(hini, app, key, &cb, &fea);
                if (rc)
                    return rc;
                key+=strlen(key)+1;
            }
        }
    }
    fealist->cbList=(PCHAR)fea-data;
    return NO_ERROR;
}

ULONG eaGetSize(HINI hini, PSZ app)
{
    ULONG len;
    PSZ key;
    if (app[0]=='\0')
        return 0;
    len=sizeof(Keys);
    if (!PrfQueryProfileData(hini, app, NULL, Keys, &len))
        return 0;
    len=0;
    key=Keys;
    while (key[0])
    {
        ULONG ealen;
        if (PrfQueryProfileSize(hini, app, key, &ealen))
            len+=ealen;
        key+=strlen(key)+1;
    }
    return len;
}
