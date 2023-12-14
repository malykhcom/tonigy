#ifndef __COMMON_H__
#define __COMMON_H__

extern CHAR commonTracksRaw[];
extern CHAR commonTracksWav[];
extern CHAR commonFilesCdi[];
extern CHAR commonBootImg[];
extern CHAR commonBlocksRaw[];
extern CHAR commonReadmeName[];
extern CHAR commonCdinfoName[];
extern CHAR commonLogName[];
extern LANGSTRS commonUnregName;
extern CHAR commonWavExt[];
extern CHAR commonRawExt[];
extern CHAR commonDatExt[];
extern CHAR commonMpgExt[];
extern CHAR commonIsoExt[];
extern CHAR commonUnkExt[];

CHAR commonCharToUpper(CHAR);
_inline VOID commonStrToUpper(PSZ str)
{
    while (str[0])
    {
        str[0]=commonCharToUpper(str[0]);
        str++;
    }
}
BOOL commonMemCompare(PCHAR, PCHAR, ULONG);
BOOL commonStrCompare(PCSZ, PCSZ);


BOOL commonVerifyString(PCSZ, ULONG);
BOOL commonIsPath(PCSZ);

_inline ULONG commonGenerateID(VOID)
{
    DATETIME datetime;
    ULONG id;
    DosGetDateTime(&datetime);
    id=datetime.hours;
    id<<=8;
    id+=datetime.minutes;
    id<<=8;
    id+=datetime.seconds;
    id<<=8;
    id+=datetime.hundredths;
    return id;
}

_inline BOOL commonFindDisk(PTDISK* disk, PMAINDATA maindata, ULONG drive)
{
    if (drive>=maindata->Drives)
        return FALSE;
    if (maindata->Disks[drive].Letter)
    {
        *disk=&maindata->Disks[drive];
        return TRUE;
    }
    return FALSE;
}

_inline BOOL commonFindFile(PTFILE* file, PTDISK disk, ULONG id)
{
    ULONG t;
    for(t=0;t<T_FILES;t++)
    {
        if (disk->Files[t].ID==id)
        {
            *file=&disk->Files[t];
            return TRUE;
        }
    }
    return FALSE;
}

_inline BOOL commonFindSearch(PTSEARCH* search, PTDISK disk, ULONG id)
{
    ULONG t;
    for(t=0;t<T_SEARCHES;t++)
    {
        if (disk->Searches[t].ID=id)
        {
            *search=&disk->Searches[t];
            return TRUE;
        }
    }
    return FALSE;
}

VOID commonAddLogRecord(PTDISK, PLOGRECORD);

#define VERIFY_FIXED(str) commonVerifyString(str, sizeof(str))

#endif
