#include "global.h"

/*static volatile BOOL Quit=FALSE;

static ULONG _System MainHandler(PEXCEPTIONREPORTRECORD p1,
                                 PEXCEPTIONREGISTRATIONRECORD p2,
                                 PCONTEXTRECORD p3,
                                 PVOID pv)
{
    if (p1->ExceptionNum==XCPT_SIGNAL &&
        (p1->ExceptionInfo[0]==XCPT_SIGNAL_INTR ||
         p1->ExceptionInfo[0]==XCPT_SIGNAL_KILLPROC ||
         p1->ExceptionInfo[0]==XCPT_SIGNAL_BREAK))
    {
        Quit=TRUE;
        return XCPT_CONTINUE_EXECUTION;
    }
    return XCPT_CONTINUE_SEARCH;
}*/

ULONG Lang=0;

int main(int argc, char* argv[])
{
    MAINDATA maindata;
    HAB hab;
    HMQ hmq;
    HMTX mutex;
    setbuf(stdout, NULL);
    DosError(FERR_DISABLEHARDERR | FERR_ENABLEEXCEPTION);
    hab=WinInitialize(0);
    if (!hab)
    {
        DosBeep(1000, 10);
        return 1;
    }
    hmq=WinCreateMsgQueue(hab, 0);
    if (!hmq)
    {
        WinTerminate(hab);
        DosBeep(1000, 10);
        return 2;
    }
    debugInit();
    if (!utilIsRunning(&mutex))
    {
        utilDetachAll();
        if (utilInitMaindata(argv[0], hab, &maindata))
        {
/*            EXCEPTIONREGISTRATIONRECORD exc;
            memset(&exc, 0, sizeof(exc));
            exc.ExceptionHandler=(ERR)MainHandler;
            DosSetExceptionHandler(&exc);*/
            keyDecode(&maindata);
            maindata.MsgText=localeGetMsg("trial.text");
            maindata.MsgTitle=localeGetMsg("trial.title");
            if (agentInit(&maindata))
            {
                HWND hwnd;
                utilPrio(maindata.Agent, maindata.HighPrio);
                utilAutoAttach(&maindata);
                if (uiInit(argv[0], &hwnd, &maindata))
                {
                    APIRET rc;
                    QMSG qmsg;
                    npipeInit(&maindata, hwnd);
                    while(WinGetMsg(hab, &qmsg, NULL, 0, 0))
                        WinDispatchMsg(hab, &qmsg);
                    do
                    {
                        rc=DosKillThread(maindata.Agent);
                    }
                    while (rc==ERROR_INTERRUPT);
                    do
                    {
                        rc=DosWaitThread((PTID)&maindata.Agent, DCWW_WAIT);
                    }
                    while (rc==ERROR_INTERRUPT);
                    uiTerm(hwnd);
                }
                utilDetachAll();
            }
            else
                errMsg(HWND_DESKTOP, localeGetMsg("error.ifs"));
            //DosUnsetExceptionHandler(&exc);
            utilTermMaindata(&maindata);
        }
        DosCloseMutexSem(mutex);
    }
    else
        errMsg(HWND_DESKTOP, "Tonigy is already running.");
    debugTerm();
    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
    return 0;
}