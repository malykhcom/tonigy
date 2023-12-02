#include "global.h"

typedef VOID (*THREADFUNC)(PVOID);

static volatile BOOL Quit=FALSE;

static ULONG _System ThreadHandler(PEXCEPTIONREPORTRECORD p1,
                                   PEXCEPTIONREGISTRATIONRECORD p2,
                                   PCONTEXTRECORD p3,
                                   PVOID pv)
{
    if (p1->ExceptionNum==XCPT_ASYNC_PROCESS_TERMINATE)
    {
        Quit=TRUE;
        return XCPT_CONTINUE_EXECUTION;
    }
    return XCPT_CONTINUE_SEARCH;
}

static BOOL AgentStarted(PMAINDATA maindata)
{
    APIRET rc;
    SETXCHGBUFFERS buffers;
    ULONG cb;
    buffers.Request=maindata->Request;
    buffers.Data=maindata->Data;
    cb=sizeof(buffers);
    rc=DosFSCtl(NULL, 0, NULL,
                &buffers, cb, &cb,
                FSCTL_STUBFSD_DAEMON_STARTED,
                IFS_NAME,
                (HFILE)-1,
                FSCTL_FSDNAME);
    if (rc)
        return FALSE;
    return TRUE;
}

static VOID AgentStopped(VOID)
{
    DosFSCtl(NULL, 0, NULL,
             NULL, 0, NULL,
             FSCTL_STUBFSD_DAEMON_STOPPED,
             IFS_NAME,
             (HFILE)-1,
             FSCTL_FSDNAME);
}

static BOOL GetRequest(VOID)
{
    APIRET rc;
    rc=DosFSCtl(NULL, 0, NULL,
                NULL, 0, NULL,
                FSCTL_STUBFSD_GET_REQUEST,
                IFS_NAME,
                (HFILE)-1,
                FSCTL_FSDNAME);
    if (rc)
        return FALSE;
    return TRUE;
}

static BOOL DoneRequest(VOID)
{
    APIRET rc;
    rc=DosFSCtl(NULL, 0, NULL,
                NULL, 0, NULL,
                FSCTL_STUBFSD_DONE_REQUEST,
                IFS_NAME,
                (HFILE)-1,
                FSCTL_FSDNAME);
    if (rc)
        return FALSE;
    return TRUE;
}

static VOID HandleRequest(PMAINDATA maindata)
{
    APIRET rc;
    PFSREQUEST request=maindata->Request;
    //printf("RQ: %d\n", request->rq);
    switch (request->rq)
    {
    case REQ_ATTACH:
        rc=DoAttach(maindata, &request->data.attach);
        break;
    case REQ_CHDIR:
        rc=DoChDir(maindata, &request->data.chdir);
        break;
    case REQ_CHGFILEPTR:
        rc=DoChgFilePtr(maindata, &request->data.chgfileptr);
        break;
    case REQ_CLOSE:
        rc=DoClose(maindata, &request->data.close);
        break;
    case REQ_DELETE:
        rc=DoDelete(maindata, &request->data.delete);
        break;
    case REQ_FILEATTRIBUTE:
        rc=DoFileAttribute(maindata, &request->data.fileattribute);
        break;
    case REQ_FILEINFO:
        rc=DoFileInfo(maindata, &request->data.fileinfo);
        break;
    case REQ_FINDCLOSE:
        rc=DoFindClose(maindata, &request->data.findclose);
        break;
    case REQ_FINDFIRST:
        rc=DoFindFirst(maindata, &request->data.findfirst);
        break;
    case REQ_FINDFROMNAME:
        rc=DoFindFromName(maindata, &request->data.findfromname);
        break;
    case REQ_FINDNEXT:
        rc=DoFindNext(maindata, &request->data.findnext);
        break;
    case REQ_FSCTL:
        rc=DoFSCtl(maindata, &request->data.fsctl);
        break;
    case REQ_FSINFO:
        rc=DoFSInfo(maindata, &request->data.fsinfo);
        break;
    case REQ_OPENCREATE:
        rc=DoOpenCreate(maindata, &request->data.opencreate);
        break;
    case REQ_PATHINFO:
        rc=DoPathInfo(maindata, &request->data.pathinfo);
        break;
    case REQ_READ:
        rc=DoRead(maindata, &request->data.read);
        break;
    default:
        //printf("REQUEST %u\n", request->rq);
        rc=ERROR_NOT_SUPPORTED;
    }
    /*if (rc)
        printf("_ %u\n", rc);*/
    request->rc=rc;
}

static BOOL RunAgent(PMAINDATA maindata)
{
    while(!Quit)
    {
        if (!GetRequest())
            return FALSE;
        HandleRequest(maindata);
        if (!DoneRequest())
            return FALSE;
    }
    return TRUE;
}

static INT thrCreate(THREADFUNC start_address, PVOID args)
{
    return _beginthread(start_address, NULL, /*16384*/65536, args);
}

static VOID Agent(PVOID args)
{
    EXCEPTIONREGISTRATIONRECORD exc;
    PMAINDATA maindata=(PMAINDATA)args;
    DosError(FERR_DISABLEHARDERR | FERR_ENABLEEXCEPTION);
    memset(&exc, 0, sizeof(exc));
    exc.ExceptionHandler=(ERR)ThreadHandler;
    DosSetExceptionHandler(&exc);
    RunAgent(maindata);
    AgentStopped();
}

BOOL agentInit(PMAINDATA maindata)
{
    INT tmp;
    if (!AgentStarted(maindata))
    {
        return FALSE;
    }
    tmp=thrCreate(Agent, maindata);
    if (tmp==-1)
    {
        AgentStopped();
        return FALSE;
    }
    maindata->Agent=tmp;
    return TRUE;
}
