#include "global.h"

#define IOCTL_DISK              0x0008
#define DSK_UNLOCKEJECTMEDIA    0x0040

#pragma pack(1)
typedef struct _NULLHANDLEPARAMS
{
    UCHAR ucCommand;
    USHORT usDrive;
} NULLHANDLEPARAMS;
#pragma pack()

BOOL utilIsRunning(PHMTX mutex)
{
    APIRET rc;
    rc=DosCreateMutexSem("\\SEM32\\TonigyRunning", mutex, 0, FALSE);
    if (rc)
        return TRUE;
    return FALSE;
}

VOID utilDetachAll(VOID)
{
    CHAR name[]="?:";
    CHAR drive;
    for(drive='A';drive<='Z';drive++)
    {
        name[0]=drive;
        DosFSAttach(name, IFS_NAME, NULL, 0, FS_DETACH);
    }
}

BOOL utilAttach(PMAINDATA maindata, ULONG drive)
{
    CHAR name[]="?:";
    PIOPTIONS options;
    options=&maindata->Options[drive];
    if (options->Letter &&
        !options->Attached &&
        (options->Letter<maindata->StartLetter ||
         options->Letter>=maindata->StartLetter+maindata->Drives))
    {
        name[0]=options->Letter;
        if (DosFSAttach(name, IFS_NAME, &drive, sizeof(ULONG), FS_ATTACH))
            return FALSE;
        options->Attached=TRUE;
        return TRUE;
    }
    return FALSE;
}

static CHAR TimeApp[]="Tonigy";
static CHAR TimeKey[]="First Run";

VOID utilAutoAttach(PMAINDATA maindata)
{
    CHAR tmp[512];
    ULONG t;
    time_t last;
    ULONG cb;
    ULONG days=0;

    if (PrfQueryProfileSize(HINI_USERPROFILE, TimeApp, TimeKey, &cb) &&
        cb==sizeof(last) &&
        PrfQueryProfileData(HINI_USERPROFILE, TimeApp, TimeKey, &last, &cb))
    {
        time_t now=time(NULL);
        if (now>last)
            days=difftime(now, last)/60.0/60.0/24.0;
    }
    else
    {
        time_t now;
        now=time(NULL);
        PrfWriteProfileData(HINI_USERPROFILE, TimeApp, TimeKey, &now, sizeof(now));
    }
    if (!maindata->Key[0])
    {
        _snprintf(tmp, sizeof(tmp), maindata->MsgText, days);
        DosBeep(1200, 20);
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                      tmp,
                      maindata->MsgTitle,
                      0, MB_OK | MB_MOVEABLE | MB_SYSTEMMODAL);
    }
    for(t=0;t<maindata->Drives;t++)
    {
        PIOPTIONS options;
        options=&maindata->Options[t];
        if (options->Letter &&
            options->AutoAttach)
        {
            if (!utilAttach(maindata, t))
            {
                //printf("Autoattach %u failed!\n", t);
            }
        }
    }
}

BOOL utilDetach(PMAINDATA maindata, ULONG drive)
{
    CHAR name[]="?:";
    PIOPTIONS options;
    options=&maindata->Options[drive];
    if (options->Letter &&
        options->Attached)
    {
        name[0]=options->Letter;
        if (DosFSAttach(name, IFS_NAME, NULL, 0, FS_DETACH))
            return FALSE;
        options->Attached=FALSE;
        return TRUE;
    }
    return FALSE;
}

BOOL utilEject(PMAINDATA maindata, ULONG drive)
{
    NULLHANDLEPARAMS nhp;
    ULONG len = sizeof(nhp);
    APIRET rc;
    nhp.ucCommand = 2;
    nhp.usDrive = (USHORT)(drive + maindata->StartLetter - 'A');
    rc = DosDevIOCtl((HFILE)-1, IOCTL_DISK, DSK_UNLOCKEJECTMEDIA,
                     &nhp, len, &len, NULL, 0, NULL);
    //printf("%u: %u\n", drive, rc);
    if (rc)
        return FALSE;
    return TRUE;
}

static BOOL AllocBuffers(PMAINDATA maindata)
{
    APIRET rc;
    rc=DosAllocMem((PVOID)&maindata->Request, sizeof(FSREQUEST),
                   PAG_COMMIT | OBJ_TILE | PAG_READ | PAG_WRITE);
    if (rc)
        return FALSE;
    rc=DosAllocMem((PVOID)&maindata->Data, sizeof(FSDATA),
                   PAG_COMMIT | OBJ_TILE | PAG_READ | PAG_WRITE);
    if (rc)
        return FALSE;
    return TRUE;
}

static CHAR App[]="Tonigy";

#define LOAD(app, key, var, def) \
    { \
    ULONG cb; \
    if (!PrfQueryProfileSize(maindata->HiniOptions, app, key, &cb) || \
    cb!=sizeof(var) || \
    !PrfQueryProfileData(maindata->HiniOptions, app, key, &var, &cb)) \
    var=def; \
    }

#define SAVE(app, key, var) \
    PrfWriteProfileData(maindata->HiniOptions, app, key, &var, sizeof(var))

static VOID LoadOptions(PMAINDATA maindata)
{
    ULONG t;
    LOAD(App, "X", maindata->X, 160);
    LOAD(App, "Y", maindata->Y, 99);
    LOAD(App, "W", maindata->W, 350);
    LOAD(App, "H", maindata->H, 216);
    LOAD(App, "SetupX", maindata->SetupX, 100);
    LOAD(App, "SetupY", maindata->SetupY, 100);
    LOAD(App, "Min", maindata->Min, FALSE);
    LOAD(App, "CDLetter", maindata->FirstLetter, '\0');
    if (maindata->FirstLetter<'A' || maindata->FirstLetter>'Z')
        maindata->FirstLetter='\0';
    if (maindata->FirstLetter)
        maindata->StartLetter=maindata->FirstLetter;
    else
        maindata->StartLetter=maindata->AutoCDLetter;
    LOAD(App, "HighPrio", maindata->HighPrio, FALSE);
    LOAD(App, "Language", maindata->Lang, 0);
    if (maindata->Lang>=LANGS)
        maindata->Lang=0;
    Lang=maindata->Lang;
    for(t=0;t<maindata->Drives;t++)
    {
        CHAR tmp[20];
        PIOPTIONS options;
        _snprintf(tmp, sizeof(tmp), "%s_%u", App, t);
        options=&maindata->Options[t];
        LOAD(tmp, "Letter", options->Letter, '\0');
        if (options->Letter<'A' || options->Letter>'Z')
            options->Letter='\0';
        maindata->Options[t].Attached=FALSE;
        LOAD(tmp, "AutoAttach", options->AutoAttach, TRUE);
        LOAD(tmp, "Antijitter", options->Options.Antijitter, TRUE);
        LOAD(tmp, "Preread", options->Options.Preread, 0);
        options->Options.Preread=min(options->Options.Preread, 10);
        LOAD(tmp, "Tries", options->Options.Tries, 10);
        LOAD(tmp, "Log", options->Log, FALSE);
    }
}

static SaveOptions(PMAINDATA maindata)
{
    ULONG t;
    SAVE(App, "X", maindata->X);
    SAVE(App, "Y", maindata->Y);
    SAVE(App, "W", maindata->W);
    SAVE(App, "H", maindata->H);
    SAVE(App, "SetupX", maindata->SetupX);
    SAVE(App, "SetupY", maindata->SetupY);
    SAVE(App, "Min", maindata->Min);
    SAVE(App, "CDLetter", maindata->FirstLetter);
    SAVE(App, "HighPrio", maindata->HighPrio);
    SAVE(App, "Language", maindata->Lang);
    for(t=0;t<maindata->Drives;t++)
    {
        CHAR tmp[20];
        PIOPTIONS options;
        _snprintf(tmp, sizeof(tmp), "%s_%u", App, t);
        options=&maindata->Options[t];
        SAVE(tmp, "Letter", options->Letter);
        SAVE(tmp, "AutoAttach", options->AutoAttach);
        SAVE(tmp, "Antijitter", options->Options.Antijitter);
        SAVE(tmp, "Preread", options->Options.Preread);
        SAVE(tmp, "Tries", options->Options.Tries);
        SAVE(tmp, "Log", options->Log);
    }
}

_inline VOID LoadKey(PCSZ path, PCHAR name, PMAINDATA maindata)
{
    FILE* key;
    UINT len;
    static CHAR tmp[6];
    strcpy(name, "tonigy.key");
    key=fopen(path, "rb");
    memset(tmp, 0, sizeof(tmp));
    if (key)
    {
        len=filelength(fileno(key));
        if (len==0)
            len=1;
        maindata->Key=M_ALLOC(len);
        fread(maindata->Key, 1, len, key);
        fclose(key);
        //printf("loaded key.\n");
    }
    else
    {
        len=sizeof(tmp);
        maindata->Key=tmp;
    }
    maindata->KeyLen=len;
    maindata->KeyRes=1;
    //printf("orig. len=%u.\n", len);
}

BOOL utilInitMaindata(PSZ exename, HAB hab, PMAINDATA maindata)
{
    ULONG t;
    CHAR path[CCHMAXPATH];
    PCHAR name;
    memset(maindata, 0, sizeof(MAINDATA));
    if (!cdromDrives(&maindata->Drives, &maindata->AutoCDLetter))
    {
        errMsg(HWND_DESKTOP, "There are no CD-ROM drives in the system.");
        return FALSE;
    }
    //maindata->Drives=2;
    if (DosCreateMutexSem(NULL, &maindata->OptionsMutex, 0, FALSE))
    {
        errMsg(HWND_DESKTOP, "A mutex cannot be created.");
        return FALSE;
    }
    if (!AllocBuffers(maindata))
    {
        errMemory(HWND_DESKTOP);
        return FALSE;
    }
    strcpy(path, exename);
    name=strrchr(path, '\\');
    if (name)
        name++;
    else
        name=path;
    strcpy(name, "ea.ini");
    maindata->HiniEA=PrfOpenProfile(hab, path);
    if (!maindata->HiniEA)
    {
        errMsg(HWND_DESKTOP, "The EA.INI file cannot be opened.");
        return FALSE;
    }
    strcpy(name, "tonigy.ini");
    maindata->HiniOptions=PrfOpenProfile(hab, path);
    if (!maindata->HiniOptions)
    {
        errMsg(HWND_DESKTOP, "The TONIGY.INI file cannot be opened.");
        return FALSE;
    }
    maindata->Unreg=M_ALLOC(2000);
    if (!maindata->Unreg)
    {
        errMemory(HWND_DESKTOP);
        return FALSE;
    }
    maindata->Readme=M_ALLOC(2000);
    if (!maindata->Readme)
    {
        errMemory(HWND_DESKTOP);
        return FALSE;
    }
    maindata->Options=M_ALLOC(sizeof(IOPTIONS)*maindata->Drives);
    if (!maindata->Options)
    {
        errMemory(HWND_DESKTOP);
        return FALSE;
    }
    maindata->Disks=M_ALLOC(sizeof(TDISK)*maindata->Drives);
    if (!maindata->Disks)
    {
        errMemory(HWND_DESKTOP);
        return FALSE;
    }
    LoadKey(path, name, maindata);
    for(t=0;t<maindata->Drives;t++)
        maindata->Disks[t].Letter='\0';
    LoadOptions(maindata);
    LoadLocale(path, name);
    _snprintf(&maindata->Readme[1], 1999, localeGetMsg("readme"), TONIGY, COPYRIGHT);
    {
        CHAR tmp[6];
        tmp[1]='n';
        tmp[2]='r';
        tmp[0]='u';
        tmp[3]='e';
        tmp[4]='g';
        tmp[5]='\0';
        _snprintf(maindata->Unreg, 1999, localeGetMsg(tmp), TONIGY, COPYRIGHT);
    }
    maindata->Readme[0]='\0';
    maindata->Hab=hab;
    return TRUE;
}

_inline VOID LoadLocale(PCSZ path, PCHAR name)
{
    PCSZ l;
    strcpy(name, "LOCALE");
    switch (Lang)
    {
    case 1:
        l="de";
        break;
    case 2:
        l="ru";
        break;
    default:
        l="en";
    }
    localeInit(path, l, "en");
}

VOID utilTermMaindata(PMAINDATA maindata)
{
    ULONG t;
    SaveOptions(maindata);
    for(t=0;t<maindata->Drives;t++)
    {
        PTDISK disk;
        disk=&maindata->Disks[t];
        if (disk->Letter)
        {
            if (disk->Handle!=(HFILE)-1)
            {
                M_FREE(disk->Tracks);
                M_FREE(disk->Blocks);
            }
            LogFree(disk);
            DirElementFree(disk->Root);
        }
    }
    M_FREE(maindata->Disks);
    M_FREE(maindata->Options);
    M_FREE(maindata->Readme);
    M_FREE(maindata->Unreg);
    PrfCloseProfile(!maindata->HiniOptions);
    PrfCloseProfile(maindata->HiniEA);
    DosFreeMem(maindata->Data);
    DosFreeMem(maindata->Request);
    DosCloseMutexSem(maindata->OptionsMutex);
}
