#include "global.h"

LANGSTRS commonUnregName=
{
    "UNREGISTERED!",
    "UNREGISTRIERT!",
    "çÖáÄêÖÉàëíêàêéÇÄçé!"
};

CHAR commonTracksRaw[]="TRACKS.RAW";
CHAR commonTracksWav[]="TRACKS.WAV";
CHAR commonBlocksRaw[]="BLOCKS.RAW";
CHAR commonFilesCdi[]="FILES.CDI";
CHAR commonBootImg[]="BOOT.IMG";
CHAR commonReadmeName[]="readme.txt";
CHAR commonCdinfoName[]="cdinfo.txt";
CHAR commonLogName[]="tonigy.log";
CHAR commonWavExt[]=".wav";
CHAR commonRawExt[]=".raw";
CHAR commonDatExt[]=".dat";
CHAR commonMpgExt[]=".mpg";
CHAR commonIsoExt[]=".iso";
CHAR commonUnkExt[]=".unk";

static BOOL UpperInited=FALSE;
static CHAR UpperTable[256+1];

CHAR commonCharToUpper(CHAR symbol)
{
    if (!UpperInited)
    {
        ULONG t;
        COUNTRYCODE country;
        country.country=0;
        country.codepage=0;
        for(t=0;t<256;t++)
        {
            UpperTable[t]=(CHAR)t;
        }
        UpperTable[256]='\0';
        if (DosMapCase(sizeof(UpperTable), &country, UpperTable))
        {
            for(t='a';t<='z';t++)
            {
                UpperTable[t]=(CHAR)(t-'a'+'A');
            }
        }
        UpperInited=TRUE;
    }
    return UpperTable[symbol];
}

BOOL commonMemCompare(PCHAR mem1, PCHAR mem2, ULONG len)
{
    ULONG t;
    for(t=0;t<len;t++)
        if (commonCharToUpper(mem1[t])!=commonCharToUpper(mem2[t]))
            return FALSE;
    return TRUE;
}

BOOL commonStrCompare(PCSZ str1, PCSZ str2)
{
    ULONG len=strlen(str1);
    if (strlen(str2)!=len)
        return FALSE;
    return commonMemCompare(str1, str2, len);
}

BOOL commonVerifyString(PCSZ str, ULONG len)
{
    while (len--)
        if (!*str++)
            return TRUE;
    return FALSE;
}

BOOL commonIsPath(PCSZ path)
{
    if (strlen(path)<3 ||
        path[1]!=':' ||
        path[2]!='\\')
        return FALSE;
    return TRUE;
}

VOID commonAddLogRecord(PTDISK disk, PLOGRECORD logrecord)
{
    BOOL log;
    PLOGRECORD tmp;
    DosRequestMutexSem(disk->OptionsMutex, SEM_INDEFINITE_WAIT);
    log=*disk->Log;
    DosReleaseMutexSem(disk->OptionsMutex);
    if (!log)
        return;
    tmp=M_ALLOC(sizeof(LOGRECORD));
    if (tmp)
    {
        ULONG t;
        *tmp=*logrecord;
        DosGetDateTime(&tmp->DateTime);
        tmp->Next=NULL;
        if (disk->LogFirst)
        {
            disk->LogLast->Next=tmp;
            disk->LogLast=tmp;
        }
        else
        {
            disk->LogFirst=tmp;
            disk->LogLast=tmp;
        }
        disk->LogRecords=disk->LogRecords+1;
        if (disk->Root)
        {
            for(t=0;;t++)
                if (disk->Root[t].Type==ELEMENT_LOG)
                    break;
            if (disk->Root[t].Name[0]=='\0')
            {
                strcpy(disk->Root[t].Name, commonLogName);
            }
            disk->Root[t].Size=disk->LogRecords*LOG_LINE;
        }
    }
}
