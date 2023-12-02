#include "global.h"

typedef VOID (*THREADFUNC)(PVOID);

static CHAR Name[]="\\PIPE\\TONIGY\\CONTROL";

static HWND Hwnd;
static HPIPE Handle;
static CHAR Buffer[512];

static VOID Send1(ULONG rc)
{
    ULONG tmp;
    _snprintf(Buffer, sizeof(Buffer), "%u\n", rc);
    DosWrite(Handle, Buffer, strlen(Buffer), &tmp);
}

static VOID Send2S(ULONG rc, PSZ ret)
{
    ULONG tmp;
    _snprintf(Buffer, sizeof(Buffer), "%u %s\n", rc, ret);
    DosWrite(Handle, Buffer, strlen(Buffer), &tmp);
}

static VOID Send2U(ULONG rc, ULONG ret)
{
    ULONG tmp;
    _snprintf(Buffer, sizeof(Buffer), "%u %u\n", rc, ret);
    DosWrite(Handle, Buffer, strlen(Buffer), &tmp);
}

static BOOL GetDrive(PMAINDATA maindata, PSZ arg, PULONG drive)
{
    ULONG tmp;
    tmp=strtoul(arg, NULL, 10);
    if (tmp>=maindata->Drives)
    {
        Send1(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    *drive=tmp;
    return TRUE;
}

static VOID Thread(PVOID args)
{
    PMAINDATA maindata;
    HAB hab;
    HMQ hmq;
    hab=WinInitialize(0);
    hmq=WinCreateMsgQueue(hab, 0);
    maindata=(PMAINDATA)args;
    for(;;)
    {
        DosConnectNPipe(Handle);
        for(;;)
        {
            APIRET rc;
            ULONG t;
            PCHAR arg;
            rc=DosRead(Handle, Buffer, sizeof(Buffer), &t);
            if (rc || t==0)
                break;
            Buffer[t]='\0';
            Buffer[strcspn(Buffer, "\r\n")]='\0';
            if (commonStrCompare(Buffer, "querydrivesnumber"))
            {
                Send2U(0, maindata->Drives);
                continue;
            }
            ///
            arg=strchr(Buffer, ' ');
            if (arg)
            {
                *arg='\0';
                arg++;
                while (*arg==' ')
                    arg++;
                if (!arg[0])
                {
                    Send1(ERROR_INVALID_PARAMETER);
                    continue;
                }
                if (commonStrCompare(Buffer, "querycdromletter"))
                {
                    ULONG drive;
                    if (GetDrive(maindata, arg, &drive))
                    {
                        CHAR tmp[3];
                        tmp[0]=maindata->StartLetter+drive;
                        tmp[1]=':';
                        tmp[2]='\0';
                        Send2S(0,  tmp);
                    }
                    continue;
                }
                if (commonStrCompare(Buffer, "querytonigyletter"))
                {
                    ULONG drive;
                    if (GetDrive(maindata, arg, &drive))
                    {
                        CHAR tmp[3];
                        tmp[0]=maindata->Options[drive].Letter;
                        if (tmp[0])
                        {
                            tmp[1]=':';
                            tmp[2]='\0';
                        }
                        Send2S(0, tmp);
                    }
                    continue;
                }
                if (commonStrCompare(Buffer, "querystatus"))
                {
                    ULONG drive;
                    if (GetDrive(maindata, arg, &drive))
                    {
                        ULONG status;
                        if (maindata->Options[drive].Attached)
                            status=1;
                        else
                            status=0;
                        Send2U(0, status);
                    }
                    continue;
                }
                if (commonStrCompare(Buffer, "attach"))
                {
                    ULONG drive;
                    if (GetDrive(maindata, arg, &drive))
                    {
                        if (WinSendMsg(Hwnd, TMSG_ATTACH, MPFROMLONG(drive), 0))
                            Send1(ERROR_GEN_FAILURE);
                        else
                            Send1(0);
                    }
                    continue;
                }
                if (commonStrCompare(Buffer, "detach"))
                {
                    ULONG drive;
                    if (GetDrive(maindata, arg, &drive))
                    {
                        if (WinSendMsg(Hwnd, TMSG_DETACH, MPFROMLONG(drive), 0))
                            Send1(ERROR_GEN_FAILURE);
                        else
                            Send1(0);
                    }
                    continue;
                }
            }
            Send1(ERROR_BAD_COMMAND);
        }
        DosDisConnectNPipe(Handle);
    }
}

static INT thrCreate(THREADFUNC start_address, PVOID args)
{
    return _beginthread(start_address, NULL, /*16384*/65536, args);
}

BOOL npipeInit(PMAINDATA maindata, HWND hwnd)
{
    if (DosCreateNPipe(Name, &Handle, NP_ACCESS_DUPLEX,
                       NP_WMESG | NP_RMESG | 0x1,
                       1024, 1024, 0))
        return FALSE;
    Hwnd=hwnd;
    if (thrCreate(Thread, maindata)==-1)
        return FALSE;
    return TRUE;
}
