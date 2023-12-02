#include "global.h"

static CHAR Registered[]=
{
    'R'^ 1^0x7f, 'e'^ 3^0x7f, 'g'^ 5^0x7f, 'i'^7,
    's'^ 9^0x7f, 't'^11^0x7f, 'e'^13^0x7f, 'r'^15,
    'e'^17^0x7f, 'd'^19^0x7f, ' '^21^0x7f, 't'^23,
    'o'^25^0x7f, ':'^27^0x7f, ' '^29^0x7f
};

static CHAR Tmp1[]="½½½½}";

static CHAR Unregistered[]=
{
    'U' ^2^0x7f, 'N'^ 4^0x7f, 'R'^ 6^0x7f, 'E'^8,
    'G'^10^0x7f, 'I'^12^0x7f, 'S'^14^0x7f, 'T'^16,
    'E'^18^0x7f, 'R'^20^0x7f, 'E'^22^0x7f, 'D'^24,
    '!'^26^0x7f/*, 'c'^28^0x7f, 'o'^30^0x7f, 'p'^32,
    'y'^34^0x7f, '.'^36^0x7f*/
};

static CHAR Tmp2[]="\0\0\0\0";

static APIRET Attach(PMAINDATA maindata, PIFS_ATTACH p)
{
    ULONG drive;
    ULONG t;
    PTDISK disk;
    drive=*(PULONG)maindata->Data;
    if (p->cb!=sizeof(ULONG) ||
        drive>=maindata->Drives)
    {
        return ERROR_INVALID_PARAMETER;
    }
    disk=&maindata->Disks[drive];
    if (disk->Letter)
        return ERROR_BUSY_DRIVE;
    disk->Drive=drive;
    disk->Letter=commonCharToUpper(p->dev[0]);
    disk->StartLetter=maindata->StartLetter;
    disk->Handle=(HFILE)-1;
    for(t=0;t<T_FILES;t++)
        disk->Files[t].ID=ID_EMPTY;
    disk->Root=NULL;
    if (!maindata->Readme[0])
    {
        PCHAR endline;
        ULONG len;
        memmove(maindata->Readme, &maindata->Readme[1],
                strlen(&maindata->Readme[1])+1);
        endline=strchr(maindata->Readme, '\n')+1;
        if (!endline)
            endline=maindata->Readme;
        len=strlen(endline);
        if (maindata->Key[0])
        {
            ULONG keylen;
            ULONG t;
            keylen=strlen(maindata->Key);
            t=keylen+sizeof(Registered)+2;
            memmove(&endline[t], endline, len+1);
            for(t=0;t<=sizeof(Registered)-1;t++)
                endline[t]=Registered[t]^(t*2+1)^((t%4==3)?0x0:0x7f);
            memcpy(&endline[sizeof(Registered)], maindata->Key, keylen);
            endline[sizeof(Registered)+keylen]='\r';
            endline[sizeof(Registered)+keylen+1]='\n';
        }
        else
        {
            ULONG t;
            t=sizeof(Unregistered)+2;
            memmove(&endline[t], endline, len+1);
            for(t=0;t<=sizeof(Unregistered)-1;t++)
                endline[t]=Unregistered[t]^(t*2+2)^((t%4==3)?0x0:0x7f);
            endline[sizeof(Unregistered)]='\r';
            endline[sizeof(Unregistered)+1]='\n';
        }
    }
    for(t=0;t<T_SEARCHES;t++)
        disk->Searches[t].ID=ID_EMPTY;
    disk->OptionsMutex=maindata->OptionsMutex;
    disk->Log=&maindata->Options[drive].Log;
    disk->LogRecords=0;
    disk->LogFirst=NULL;
    disk->LogLast=NULL;
    p->diskid=drive;
    //cdromQuery(disk);
    //printf("Attached!\n");
    {
        LOGRECORD tmp;
        tmp.Type=LOG_ATTACHED;
        commonAddLogRecord(disk, &tmp);
    }
    return NO_ERROR;
}

static APIRET Detach(PMAINDATA maindata, PIFS_ATTACH p)
{
    PTDISK disk;
    if (commonFindDisk(&disk, maindata, p->diskid))
    {
        ULONG t;
        disk->Letter='\0';
        cdromClose(disk);
        for(t=0;t<T_FILES;t++)
            if (disk->Files[t].ID!=ID_EMPTY)
                FileFree(&disk->Files[t]);
        for(t=0;t<T_SEARCHES;t++)
            if (disk->Searches[t].ID!=ID_EMPTY)
                SearchFree(&disk->Searches[t]);
        LogFree(disk);
        DirElementFree(disk->Root);
    }
    return NO_ERROR;
}

static APIRET AttachInfo(PMAINDATA maindata, PIFS_ATTACH p)
{
    PTDISK disk;
    ULONG len;
    PSZ info;
    if (!commonFindDisk(&disk, maindata, p->diskid))
        return ERROR_INVALID_PARAMETER;
    info=localeGetMsg("attach.info");
    len=strlen(info);
    if (p->cb<len+3)
    {
        p->cb=len+3;
        return ERROR_BUFFER_OVERFLOW;
    }
    p->cb=len+3;
    *(PUSHORT)maindata->Data=len+1;
    strcpy(&maindata->Data[2], info);
    {
        PCHAR drive;
        drive=strstr(&maindata->Data[2], "?:");
        if (drive)
            *drive=(CHAR)(disk->Drive+disk->StartLetter);
    }
    return NO_ERROR;
}

/*static VOID PrintDebug(PIFS_ATTACH p)
{
    printf("ATTACH(flag=%u, dev='%s')\n",
           (ULONG)p->flag,
           p->dev);
}*/

APIRET DoAttach(PMAINDATA maindata, PIFS_ATTACH p)
{
    APIRET rc;
    //PrintDebug(p);
    if (p->flag!=FSA_ATTACH &&
        p->flag!=FSA_DETACH &&
        p->flag!=FSA_ATTACH_INFO)
        return ERROR_NOT_SUPPORTED;
    if (!VERIFY_FIXED(p->dev) ||
        strlen(p->dev)!=2 ||
        p->dev[1]!=':')
        return ERROR_INVALID_PARAMETER;
    switch (p->flag)
    {
    case FSA_ATTACH:
        rc=Attach(maindata, p);
        break;
    case FSA_DETACH:
        rc=Detach(maindata, p);
        break;
    case FSA_ATTACH_INFO:
        rc=AttachInfo(maindata, p);
        break;
    }
    return rc;
}
