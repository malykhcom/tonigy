#define INCL_DOSERRORS
#define INCL_DOS
#include <os2.h>

#include "dhcalls.h"
#include "fsd.h"
#include "fsh.h"

#define RING0
#include "ifs.h"

#undef FS_ATTACH

/* void memcpy(void * p, void * q, int b); */

#define IFSEXPORT far pascal _loadds
/*#define IFSEXPORT far pascal _export _loadds*/

PFN Device_Help;

CHAR pascal FS_NAME[]="TONIGY";
ULONG pascal FS_ATTRIBUTE = FSA_REMOTE;

PID PidAgent;

typedef ULONG SEM;

SEM semSerialize;
SEM semRqAvail;
SEM semRqDone;

typedef CHAR HVMLOCK[12];
HVMLOCK Lock[2];

PFSREQUEST Request;
PFSDATA    FSData;

/* Christ!  The kernel sometimes passes "null" pointers that are
   not actually null.  Thanks to Karl Olsen's RAMFS for showing
   this. */
#define IS_NULL(p) (SELECTOROF(p) < 4)

static APIRET VirtToLin(LIN FAR* lin, PVOID virt)
{
    return DevHelp_VirtToLin(SELECTOROF(virt), OFFSETOF(virt), lin);
}

static PID QueryPID()
{
    APIRET rc;
    struct
    {
        PID pid;
        USHORT uid;
        USHORT pdb;
    } info;

    rc=FSH_QSYSINFO(QSI_PROCID, (PCHAR)&info, sizeof(info));
    if (rc)
        return 0;
    return info.pid;
}

static APIRET LockBuffers()
{
    APIRET rc;

   rc=FSH_SEMREQUEST(&semSerialize, TO_INFINITE);
   if (rc)
       return rc;

   if (!PidAgent)
   {
       FSH_SEMCLEAR(&semSerialize);
       return TONIGY_DAEMON_NOT_RUNNING;
   }

   return NO_ERROR;
}

static APIRET UnlockBuffers()
{
   return FSH_SEMCLEAR(&semSerialize);
}

#define UNLOCK_AND_EXIT(rcexpr) \
   { \
    APIRET temprc1, temprc2; \
    temprc2 = rcexpr; \
    temprc1 = UnlockBuffers(); \
    return temprc1 ? temprc1 : temprc2; \
    }

static APIRET signalDaemonAndWait()
{
   APIRET rc;
   rc=FSH_SEMCLEAR(&semRqAvail);
   if (rc)
       return rc;
   do
   {
       rc=FSH_SEMWAIT(&semRqDone, TO_INFINITE);
   }
   while (rc==ERROR_INTERRUPT);
   if (rc)
       return rc;
   do
   {
       rc=FSH_SEMSET(&semRqDone);
   }
   while (rc==ERROR_INTERRUPT);
   if (rc)
       return rc;
   if (!PidAgent)
       return TONIGY_DAEMON_NOT_RUNNING;
   return NO_ERROR;
}

static APIRET QueryDiskID(DISKID FAR* diskid, USHORT vpb)
{
    struct vpfsi FAR* pvpfsi;
    struct vpfsd FAR* pvpfsd;
    FSH_GETVOLPARM(vpb, &pvpfsi, &pvpfsd);
    if (!pvpfsd)
        return ERROR_INVALID_PARAMETER;
    *(PVOID FAR*)diskid=*(PVOID FAR*)pvpfsd;
    return NO_ERROR;
}

static APIRET daemonStarted(PSETXCHGBUFFERS pxchg)
{
   APIRET rc;
   PAGELIST pagelist[17];
   ULONG pages;
   PID pid;
   LIN linpagelist;
   LIN linlock0, linlock1;

   rc=FSH_SEMREQUEST(&semSerialize, TO_INFINITE);
   if (rc)
       return rc;
   if (PidAgent)
       UNLOCK_AND_EXIT(TONIGY_DAEMON_RUNNING);
   rc=VirtToLin(&linpagelist, pagelist);
   if (rc)
       UNLOCK_AND_EXIT(rc);
   rc=VirtToLin(&linlock0, &Lock[0]);
   if (rc)
       UNLOCK_AND_EXIT(rc);
   rc=VirtToLin(&linlock1, &Lock[1]);
   if (rc)
       UNLOCK_AND_EXIT(rc);
   rc=DevHelp_VMLock(VMDHL_WRITE | VMDHL_LONG,
                     pxchg->linRequest,
                     sizeof(FSREQUEST),
                     linpagelist,
                     linlock0,
                     &pages);
   if (rc)
       UNLOCK_AND_EXIT(TONIGY_INIT_FAILED);
   rc=DevHelp_LinToGDTSelector(SELECTOROF(Request),
                               pxchg->linRequest,
                               sizeof(FSREQUEST));
   if (rc)
       UNLOCK_AND_EXIT(TONIGY_INIT_FAILED);
   rc=DevHelp_VMLock(VMDHL_WRITE | VMDHL_LONG,
                     pxchg->linData,
                     sizeof(FSDATA),
                     linpagelist,
                     linlock1,
                     &pages);
   if (rc)
       UNLOCK_AND_EXIT(TONIGY_INIT_FAILED);
   rc=DevHelp_LinToGDTSelector(SELECTOROF(FSData),
                               pxchg->linData,
                               sizeof(FSDATA));
   if (rc)
       UNLOCK_AND_EXIT(TONIGY_INIT_FAILED);
   rc=FSH_SEMSET(&semRqAvail);
   if (rc)
       UNLOCK_AND_EXIT(rc);
   rc=FSH_SEMSET(&semRqDone);
   if (rc)
       UNLOCK_AND_EXIT(rc);
   pid=QueryPID();
   if (!pid)
       UNLOCK_AND_EXIT(ERROR_INVALID_PROCID);
   PidAgent=pid;
   UNLOCK_AND_EXIT(NO_ERROR);
}

static APIRET daemonStopped(VOID)
{
    APIRET rc;
    LIN linlock0, linlock1;
    PidAgent=0;
    FSH_SEMCLEAR(&semRqDone);
    rc=FSH_SEMREQUEST(&semSerialize, TO_INFINITE);
    if (rc)
        return rc;
    rc=VirtToLin(&linlock0, &Lock[0]);
    if (rc)
        UNLOCK_AND_EXIT(rc);
   rc=VirtToLin(&linlock1, &Lock[1]);
   if (rc)
       UNLOCK_AND_EXIT(rc);
    DevHelp_VMUnLock(linlock0);
    DevHelp_VMUnLock(linlock1);
    UNLOCK_AND_EXIT(NO_ERROR);
}

#define COPYPTR(dst, src) \
   * (VOID FAR* FAR*) (dst) = * (VOID FAR* FAR*) (src)

static APIRET CopyFealistToData(PEAOP eaop)
{
    APIRET rc;
    PFEALIST fealist;
    rc=FSH_PROBEBUF(PB_OPREAD, (PCHAR)eaop, sizeof(EAOP));
    if (rc)
        return ERROR_INVALID_PARAMETER;
    fealist=eaop->fpFEAList;
    rc=FSH_PROBEBUF(PB_OPREAD, (PCHAR)fealist, sizeof(ULONG));
    if (rc)
        return ERROR_INVALID_PARAMETER;
    if (fealist->cbList<sizeof(ULONG))
        return ERROR_EA_LIST_INCONSISTENT;
    if (fealist->cbList>sizeof(FSDATA))
        return ERROR_EA_LIST_TOO_LONG;
    rc=FSH_PROBEBUF(PB_OPREAD, (PCHAR)fealist, (USHORT)fealist->cbList);
    if (rc)
        return ERROR_INVALID_PARAMETER;
    memcpy(FSData, fealist, fealist->cbList);
    return NO_ERROR;
}

static APIRET CopyGealistToData(PEAOP eaop)
{
    APIRET rc;
    PGEALIST gealist;
    rc=FSH_PROBEBUF(PB_OPREAD, (PCHAR)eaop, sizeof(EAOP));
    if (rc)
        return ERROR_INVALID_PARAMETER;
    gealist=eaop->fpGEAList;
    rc=FSH_PROBEBUF(PB_OPREAD, (PCHAR)gealist, sizeof(ULONG));
    if (rc)
        return ERROR_INVALID_PARAMETER;
    if (gealist->cbList<sizeof(ULONG))
        return ERROR_EA_LIST_INCONSISTENT;
    if (gealist->cbList>sizeof(FSDATA))
        return ERROR_EA_LIST_TOO_LONG;
    rc=FSH_PROBEBUF(PB_OPREAD, (PCHAR)gealist, (USHORT)gealist->cbList);
    if (rc)
        return ERROR_INVALID_PARAMETER;
    memcpy(FSData, gealist, gealist->cbList);
    return NO_ERROR;
}

static APIRET SetOError(PEAOP eaop, USHORT oerror)
{
    APIRET rc;
    rc=FSH_PROBEBUF(PB_OPWRITE, (PCHAR)eaop, sizeof(EAOP));
    if (rc)
        return ERROR_INVALID_PARAMETER;
    eaop->oError=oerror;
    return NO_ERROR;
}

static APIRET CopyDataToFealist(PEAOP eaop)
{
    APIRET rc;
    PFEALIST fealist;
    ULONG cb;
    rc=FSH_PROBEBUF(PB_OPREAD, (PCHAR)eaop, sizeof(EAOP));
    if (rc)
        return ERROR_INVALID_PARAMETER;
    fealist=eaop->fpFEAList;
    rc=FSH_PROBEBUF(PB_OPREAD, (PCHAR)fealist, sizeof(ULONG));
    if (rc)
        return ERROR_INVALID_PARAMETER;
    cb=((PFEALIST)FSData)->cbList;
    if (cb>fealist->cbList)
    {
        rc=FSH_PROBEBUF(PB_OPWRITE, (PCHAR)fealist, sizeof(ULONG));
        if (rc)
            return ERROR_INVALID_PARAMETER;
        fealist->cbList=cb;
        return ERROR_BUFFER_OVERFLOW;
    }
    rc=FSH_PROBEBUF(PB_OPWRITE, (PCHAR)fealist, (USHORT)cb);
    if (rc)
        return ERROR_INVALID_PARAMETER;
    memcpy(fealist, FSData, cb);
    return NO_ERROR;
}

static APIRET FileInfoPre(USHORT flag, USHORT level, USHORT cb, PCHAR data)
{
   APIRET rc;
   if (flag!=FI_RETRIEVE)
   {
       if (level==FIL_QUERYEASIZE)
       {
           rc=CopyFealistToData((PEAOP)data);
           if (rc)
               return rc;
       }
       else
       {
           rc=FSH_PROBEBUF(PB_OPREAD, data, cb);
           if (rc)
               return ERROR_INVALID_PARAMETER;
           memcpy(FSData, data, cb);
       }
   }
   else
   {
       if (level==FIL_QUERYEASFROMLIST)
       {
           rc=CopyGealistToData((PEAOP)data);
           if (rc)
               return rc;
       }
   }
   return NO_ERROR;
}

static APIRET FileInfoPost(USHORT flag, USHORT level, USHORT cb, PCHAR data)
{
   APIRET rc;
   if (flag==FI_RETRIEVE)
   {
       if (level==FIL_QUERYEASFROMLIST ||
           level==4)
       {
           rc=CopyDataToFealist((PEAOP)data);
           if (rc)
               return rc;
       }
       else
       {
           rc=FSH_PROBEBUF(PB_OPWRITE, data, cb);
           if (rc)
               return ERROR_INVALID_PARAMETER;
           memcpy(data, FSData, cb);
       }
   }
   return NO_ERROR;
}

APIRET UndoFindFirst(struct cdfsi FAR* pcdfsi)
{
    APIRET rc;
    PIFS_FINDCLOSE p=&Request->data.findclose;
    p->searchid=Request->data.findfirst.searchid;
    rc=QueryDiskID(&p->diskid, pcdfsi->cdi_hVPB);
    if (rc)
        return rc;
    Request->rq=REQ_FINDCLOSE;
    return signalDaemonAndWait();
}

// Not supported

APIRET IFSEXPORT FS_ALLOCATEPAGESPACE(struct sffsi FAR* psffsi,
                                      struct sffsd FAR* psffsd,
                                      ULONG size,
                                      ULONG wantcontig)
{
    return ERROR_NOT_SUPPORTED;
}

APIRET IFSEXPORT FS_CANCELLOCKREQUEST(struct sffsi FAR* psffsi,
                                      struct sffsd FAR* psffsd,
                                      PVOID lockrange)
{
    return ERROR_NOT_SUPPORTED;
}

APIRET IFSEXPORT FS_COMMIT(USHORT type,
                           USHORT ioflag,
                           struct sffsi FAR* psffsi,
                           struct sffsd FAR* psffsd)
{
    return NO_ERROR;
}

APIRET IFSEXPORT FS_COPY(USHORT flag,
                         struct cdfsi FAR* pcdfsi,
                         struct cdfsd FAR* pcdfsd,
                         PSZ src,
                         USHORT srccurdirend,
                         PSZ dst,
                         USHORT dstcurdirend,
                         USHORT nametype)
{
    return ERROR_NOT_SUPPORTED;
}

APIRET IFSEXPORT FS_DOPAGEIO(struct sffsi FAR* psffsi,
                             struct sffsd FAR* psffsd,
                             struct PageCmdHeader FAR* pagecmdlist)
{
    return ERROR_NOT_SUPPORTED;
}

APIRET IFSEXPORT FS_FILEIO(struct sffsi FAR* psffsi,
                           struct sffsd FAR* psffsd,
                           PCHAR cbCmdList,
                           USHORT pCmdLen,
                           PUSHORT poError,
                           USHORT IOflag)
{
    return ERROR_NOT_SUPPORTED;
}

APIRET IFSEXPORT FS_FILELOCKS(struct sffsi FAR* psffsi,
                              struct sffsd FAR* psffsd,
                              struct filelock FAR* pUnLockRange,
                              struct filelock FAR* pLockRange,
                              ULONG timeout,
                              ULONG flags)
{
    return ERROR_NOT_SUPPORTED;
}

APIRET IFSEXPORT FS_FINDNOTIFYFIRST(struct cdfsi FAR* pcdfsi,
                                    struct cdfsd FAR* pcdfsd,
                                    PSZ pName,
                                    USHORT iCurDirEnd,
                                    USHORT attr,
                                    PUSHORT pHandle,
                                    PCHAR pData,
                                    USHORT cbData,
                                    PUSHORT pcMatch,
                                    USHORT level,
                                    ULONG timeout)
{
    return ERROR_NOT_SUPPORTED;
}

APIRET IFSEXPORT FS_FINDNOTIFYNEXT(USHORT handle,
                                   PCHAR pData,
                                   USHORT cbData,
                                   PUSHORT pcMatch,
                                   USHORT infolevel,
                                   ULONG timeout)
{
    return ERROR_NOT_SUPPORTED;
}

APIRET IFSEXPORT FS_FINDNOTIFYCLOSE(USHORT handle)
{
    return ERROR_NOT_SUPPORTED;
}

APIRET IFSEXPORT FS_FLUSHBUF(USHORT hVPB,
                             USHORT fsFlag)
{
    return NO_ERROR;
}


APIRET IFSEXPORT FS_IOCTL(struct sffsi FAR* psffsi,
                          struct sffsd FAR* psffsd,
                          USHORT usCat,
                          USHORT usFunc,
                          PCHAR pParm,
                          USHORT cbMaxParm,
                          PUSHORT pcbParm,
                          PCHAR pData,
                          USHORT cbMaxData,
                          PUSHORT pcbData)
{
    return NO_ERROR;
}

APIRET IFSEXPORT FS_MKDIR(struct cdfsi FAR* pcdfsi,
                          struct cdfsd FAR* pcdfsd,
                          PSZ pszName,
                          USHORT iCurDirEnd,
                          PEAOP pEABuf,
                          USHORT flags)
{
    return ERROR_WRITE_PROTECT;
}

APIRET IFSEXPORT FS_MOUNT(USHORT flag,
                          struct vpfsi FAR* pvpfsi,
                          struct vpfsd FAR* pvpfsd,
                          USHORT hVPB,
                          PCHAR pBoot)
{
    return ERROR_NOT_SUPPORTED;
}

APIRET IFSEXPORT FS_MOVE(struct cdfsi FAR* pcdfsi,
                         struct cdfsd FAR* pcdfsd,
                         PSZ pszSrc,
                         USHORT iSrcCurDirEnd,
                         PSZ pszDst,
                         USHORT iDstCurDirEnd,
                         USHORT flags)
{
    return ERROR_WRITE_PROTECT;
}

APIRET IFSEXPORT FS_NEWSIZE(struct sffsi FAR* psffsi,
                            struct sffsd FAR* psffsd,
                            ULONG cbLen,
                            USHORT fsIOFlag)
{
    return ERROR_WRITE_PROTECT;
}

APIRET IFSEXPORT FS_NMPIPE(struct sffsi FAR* psffsi,
                           struct sffsd FAR* psffsd,
                           USHORT OpType,
                           union npoper FAR* pOpRec,
                           PCHAR pData,
                           PCHAR pName)
{
    return ERROR_NOT_SUPPORTED;
}

APIRET IFSEXPORT FS_OPENPAGEFILE(PULONG pFlags,
                                 PULONG pcMaxReq,
                                 PSZ pName,
                                 struct sffsi FAR* psffsi,
                                 struct sffsd FAR* psffsd,
                                 USHORT usOpenMode,
                                 USHORT usOpenFlag,
                                 USHORT usAttr,
                                 ULONG  Reserved)
{
    return ERROR_NOT_SUPPORTED;
}

APIRET IFSEXPORT FS_PROCESSNAME(PSZ pszName)
{
    return NO_ERROR;
}

APIRET IFSEXPORT FS_RMDIR(struct cdfsi FAR* pcdfsi,
                          struct cdfsd FAR* pcdfsd,
                          PSZ pszName,
                          USHORT iCurDirEnd)
{
    return ERROR_WRITE_PROTECT;
}

APIRET IFSEXPORT FS_SETSWAP(struct sffsi FAR* psffsi,
                            struct sffsd FAR* psffsd)
{
   return ERROR_NOT_SUPPORTED;
}

APIRET IFSEXPORT FS_SHUTDOWN(USHORT usType,
                             ULONG ulReserved)
{
    return NO_ERROR;
}

APIRET IFSEXPORT FS_WRITE(struct sffsi FAR* psffsi,
                          struct sffsd FAR* psffsd,
                          PCHAR pData,
                          PUSHORT pcbLen,
                          USHORT fsIOFlag)
{
    return ERROR_WRITE_PROTECT;
}


APIRET IFSEXPORT FS_VERIFYUNCNAME(USHORT flag,
                                  PSZ pName)
{
    return ERROR_NOT_SUPPORTED;
}

/// READY!!

APIRET IFSEXPORT FS_ATTACH(USHORT flag,
                           PSZ dev,
                           struct vpfsd FAR * pvpfsd,
                           struct cdfsd FAR * pcdfsd,
                           PCHAR parm,
                           PUSHORT cb)
{
    APIRET rc;
    PIFS_ATTACH p=&Request->data.attach;

    if (flag==FSA_DETACH && !PidAgent)
    {
        return NO_ERROR;
    }
    rc=LockBuffers();
    if (rc)
        return rc;
    if (pvpfsd)
        COPYPTR(&p->diskid, pvpfsd);
    p->flag=flag;
    if (strlen(dev)>=FSXCHG_ATTACH_DEVMAX)
        UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
    strcpy(p->dev, dev);
    p->cb=*cb;
    if (((flag==FSA_ATTACH) || (flag==FSA_DETACH)) && *cb)
    {
        rc=FSH_PROBEBUF(PB_OPREAD, parm, *cb);
        if (rc)
            UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
        memcpy(FSData, parm, *cb);
    }
    Request->rq=REQ_ATTACH;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    if (flag==FSA_ATTACH_INFO)
    {
        *cb=p->cb;
        if (*cb && !Request->rc)
        {
           rc=FSH_PROBEBUF(PB_OPWRITE, parm, *cb);
           if (rc)
               UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
           memcpy(parm, FSData, *cb);
        }
    }
    if (pvpfsd)
        COPYPTR(pvpfsd, &p->diskid);
    UNLOCK_AND_EXIT(Request->rc);
}

APIRET IFSEXPORT FS_CHDIR(USHORT flag,
                          struct cdfsi FAR* pcdfsi,
                          struct cdfsd FAR* pcdfsd,
                          PSZ dir,
                          USHORT curdirend)
{
    APIRET rc;
    PIFS_CHDIR p=&Request->data.chdir;

    rc=LockBuffers();
    if (rc)
        return rc;
    if (flag==CD_EXPLICIT || flag == CD_VERIFY)
    {
        rc=QueryDiskID(&p->diskid, pcdfsi->cdi_hVPB);
        if (rc)
            UNLOCK_AND_EXIT(rc);
        if (flag==CD_EXPLICIT)
        {
            if (strlen(dir)>=CCHMAXPATH)
                UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
            strcpy(p->dir, dir);
        }
    }
    p->flag=flag;
    Request->rq=REQ_CHDIR;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    UNLOCK_AND_EXIT(Request->rc);
}

APIRET IFSEXPORT FS_CHGFILEPTR(struct sffsi FAR* psffsi,
                               struct sffsd FAR* psffsd,
                               LONG offset,
                               USHORT type,
                               USHORT ioflag)
{
    APIRET rc;
    PIFS_CHGFILEPTR p = &Request->data.chgfileptr;

    rc=LockBuffers();
    if (rc)
        return rc;
    rc=QueryDiskID(&p->diskid, psffsi->sfi_hVPB);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    COPYPTR(&p->fileid, psffsd);
    p->sffsi=*psffsi;
    p->offset=offset;
    p->type=type;
    Request->rq = REQ_CHGFILEPTR;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    if (!Request->rc)
        *psffsi=p->sffsi;
    UNLOCK_AND_EXIT(Request->rc);
}


APIRET IFSEXPORT FS_CLOSE(USHORT type,
                          USHORT ioflag,
                          struct sffsi FAR* psffsi,
                          struct sffsd FAR* psffsd)
{
    APIRET rc;
    PIFS_CLOSE p=&Request->data.close;

    rc=LockBuffers();
    if (rc)
        return rc;
    rc=QueryDiskID(&p->diskid, psffsi->sfi_hVPB);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    COPYPTR(&p->fileid, psffsd);
    p->type=type;
    Request->rq=REQ_CLOSE;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    UNLOCK_AND_EXIT(Request->rc);
}

APIRET IFSEXPORT FS_DELETE(struct cdfsi FAR* pcdfsi,
                           struct cdfsd FAR* pcdfsd,
                           PSZ name,
                           USHORT curdirend)
{
    APIRET rc;
    PIFS_DELETE p=&Request->data.delete;

    rc=LockBuffers();
    if (rc)
        return rc;
    rc=QueryDiskID(&p->diskid, pcdfsi->cdi_hVPB);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    if (strlen(name)>=CCHMAXPATH)
        UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
    strcpy(p->name, name);
    Request->rq=REQ_DELETE;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    UNLOCK_AND_EXIT(Request->rc);
}

APIRET IFSEXPORT FS_EXIT(USHORT uid,
                         USHORT pid,
                         USHORT pdb)
{
    if (PidAgent && (pid==PidAgent)) // duty hack!!
        daemonStopped();
    return NO_ERROR;
}

APIRET IFSEXPORT FS_FILEATTRIBUTE(USHORT flag,
                                  struct cdfsi FAR* pcdfsi,
                                  struct cdfsd FAR* pcdfsd,
                                  PSZ name,
                                  USHORT curdirend,
                                  PUSHORT attr)
{
    APIRET rc;
    PIFS_FILEATTRIBUTE p=&Request->data.fileattribute;

    rc=LockBuffers();
    if (rc)
        return rc;
    rc=QueryDiskID(&p->diskid, pcdfsi->cdi_hVPB);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    p->flag=flag;
    if (strlen(name)>=CCHMAXPATH)
        UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
    strcpy(p->name, name);
    Request->rq=REQ_FILEATTRIBUTE;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    if (!(flag & FA_SET) && !Request->rc)
        *attr=p->attr;
    UNLOCK_AND_EXIT(Request->rc);
}

APIRET IFSEXPORT FS_FILEINFO(USHORT flag,
                             struct sffsi FAR* psffsi,
                             struct sffsd FAR* psffsd,
                             USHORT level,
                             PCHAR data,
                             USHORT cb,
                             USHORT ioflag)
{
    APIRET rc;
    PIFS_FILEINFO p=&Request->data.fileinfo;

    rc=LockBuffers();
    if (rc)
        return rc;
    rc=FileInfoPre(flag, level, cb, data);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    rc=QueryDiskID(&p->diskid, psffsi->sfi_hVPB);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    COPYPTR(&p->fileid, psffsd);
    p->flag=flag;
    p->sffsi=*psffsi;
    p->level=level;
    p->cb=cb;
    Request->rq = REQ_FILEINFO;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    if (Request->rc)
    {
        if (flag!=FI_RETRIEVE &&
            level==FIL_QUERYEASIZE)
        {
            rc=SetOError((PEAOP)data, 0);
            if (rc)
                UNLOCK_AND_EXIT(rc);
        }
        UNLOCK_AND_EXIT(Request->rc);
    }
   *psffsi=p->sffsi;
   rc=FileInfoPost(flag, level, cb, data);
   UNLOCK_AND_EXIT(rc);
}

APIRET IFSEXPORT FS_FINDCLOSE(struct fsfsi FAR* pfsfsi,
                              struct fsfsd FAR* pfsfsd)
{
    APIRET rc;
    PIFS_FINDCLOSE p=&Request->data.findclose;

    rc=LockBuffers();
    if (rc)
        return rc;
    rc=QueryDiskID(&p->diskid, pfsfsi->fsi_hVPB);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    COPYPTR(&p->searchid, pfsfsd);
    Request->rq = REQ_FINDCLOSE;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    UNLOCK_AND_EXIT(Request->rc);
}

APIRET IFSEXPORT FS_FINDFIRST(struct cdfsi FAR* pcdfsi,
                              struct cdfsd FAR* pcdfsd,
                              PSZ name,
                              USHORT curdirend,
                              USHORT attr,
                              struct fsfsi FAR* pfsfsi,
                              struct fsfsd FAR* pfsfsd,
                              PCHAR data,
                              USHORT cb,
                              PUSHORT match,
                              USHORT level,
                              USHORT flags)
{
    APIRET rc, rc2;
    PIFS_FINDFIRST p=&Request->data.findfirst;

    rc=LockBuffers();
    if (rc)
        return rc;
    rc=QueryDiskID(&p->diskid, pcdfsi->cdi_hVPB);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    if (strlen(name) >= CCHMAXPATH)
        UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
    strcpy(p->name, name);
    p->attr=attr;
    p->cb=cb;
    p->match=*match;
    p->level=level;
    p->flags=flags;
    if (level==FIL_QUERYEASFROMLIST)
    {
        rc=CopyGealistToData((PEAOP)data);
        if (rc)
            UNLOCK_AND_EXIT(rc);
        p->cb-=sizeof(EAOP);
    }
    Request->rq = REQ_FINDFIRST;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    if (!Request->rc)
    {
        rc=FSH_PROBEBUF(PB_OPWRITE, data, cb);
        if (rc)
        {
            rc2=UndoFindFirst(pcdfsi);
            if (rc2)
                UNLOCK_AND_EXIT(rc2);
            UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
        }
        if (level==FIL_QUERYEASFROMLIST)
        {
            data+=sizeof(EAOP);
            cb-=sizeof(EAOP);
        }
        memcpy(data, FSData, cb);
        COPYPTR(pfsfsd, &p->searchid);
        *match=p->match;
    }
    else
    {
        rc=SetOError((PEAOP)data, 0);
        if (rc)
            UNLOCK_AND_EXIT(rc);
    }
    UNLOCK_AND_EXIT(Request->rc);
}

APIRET IFSEXPORT FS_FINDFROMNAME(struct fsfsi FAR* pfsfsi,
                                 struct fsfsd FAR* pfsfsd,
                                 PCHAR data,
                                 USHORT cb,
                                 PUSHORT match,
                                 USHORT level,
                                 ULONG position,
                                 PSZ name,
                                 USHORT flags)
{
    APIRET rc;
    PIFS_FINDFROMNAME p=&Request->data.findfromname;

    rc=LockBuffers();
    if (rc)
        return rc;
    rc=QueryDiskID(&p->diskid, pfsfsi->fsi_hVPB);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    COPYPTR(&p->searchid, pfsfsd);
    p->cb=cb;
    p->match=*match;
    p->level=level;
    p->flags=flags;
    p->position=position;
    Request->rq=REQ_FINDFROMNAME;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    if (!Request->rc)
    {
        rc=FSH_PROBEBUF(PB_OPWRITE, data, cb);
        if (rc)
            UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
        if (level==FIL_QUERYEASFROMLIST)
        {
            data+=sizeof(EAOP);
            cb-=sizeof(EAOP);
        }
        memcpy(data, FSData, cb);
        *match=p->match;
    }
    else
    {
        rc=SetOError((PEAOP)data, 0);
        if (rc)
            UNLOCK_AND_EXIT(rc);
    }
    UNLOCK_AND_EXIT(Request->rc);
}

APIRET IFSEXPORT FS_FINDNEXT(struct fsfsi FAR* pfsfsi,
                             struct fsfsd FAR* pfsfsd,
                             PCHAR data,
                             USHORT cb,
                             PUSHORT match,
                             USHORT level,
                             USHORT flags)
{
    APIRET rc;
    PIFS_FINDNEXT p=&Request->data.findnext;

    rc=LockBuffers();
    if (rc)
        return rc;
    rc=QueryDiskID(&p->diskid, pfsfsi->fsi_hVPB);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    COPYPTR(&p->searchid, pfsfsd);
    p->cb=cb;
    p->match=*match;
    p->level=level;
    p->flags=flags;
    Request->rq=REQ_FINDNEXT;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    if (!Request->rc)
    {
        rc=FSH_PROBEBUF(PB_OPWRITE, data, cb);
        if (rc)
            UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
        if (level==FIL_QUERYEASFROMLIST)
        {
            data+=sizeof(EAOP);
            cb-=sizeof(EAOP);
        }
        memcpy(data, FSData, cb);
        *match=p->match;
    }
    else
    {
        rc=SetOError((PEAOP)data, 0);
        if (rc)
            UNLOCK_AND_EXIT(rc);
    }
    UNLOCK_AND_EXIT(Request->rc);
}


APIRET IFSEXPORT FS_FSCTL(union argdat FAR* argdat,
                          USHORT argtype,
                          USHORT func,
                          PCHAR parm,
                          USHORT cbmaxparm,
                          PUSHORT cbparm,
                          PCHAR data,
                          USHORT cbmaxdata,
                          PUSHORT cbdata)
{
    APIRET rc;
    PIFS_FSCTL p=&Request->data.fsctl;

    if ((argtype!=FSCTL_ARG_FILEINSTANCE) &&
        (argtype!=FSCTL_ARG_CURDIR) &&
        (argtype!=FSCTL_ARG_NULL))
        return ERROR_NOT_SUPPORTED;
    if (cbmaxparm)
    {
        rc=FSH_PROBEBUF(PB_OPREAD, (PCHAR)cbparm, sizeof(USHORT));
        if (rc)
            return ERROR_INVALID_PARAMETER;
        if (*cbparm>cbmaxparm)
            return ERROR_INVALID_PARAMETER;
        if (*cbparm)
        {
            rc=FSH_PROBEBUF(PB_OPREAD, parm, *cbparm);
            if (rc)
                return ERROR_INVALID_PARAMETER;
        }
    }
    switch (func)
    {
    case FSCTL_STUBFSD_DAEMON_STARTED:
        if (*cbparm!=sizeof(SETXCHGBUFFERS))
            return ERROR_INVALID_PARAMETER;
        return daemonStarted((PSETXCHGBUFFERS)parm);
    case FSCTL_STUBFSD_DAEMON_STOPPED:
        if (!PidAgent)
            return TONIGY_DAEMON_NOT_RUNNING;
        if (QueryPID()!=PidAgent)
            return TONIGY_NOT_DAEMON;
        return daemonStopped();
    case FSCTL_STUBFSD_GET_REQUEST:
        if (QueryPID()!=PidAgent)
            return TONIGY_NOT_DAEMON;
        rc=FSH_SEMWAIT(&semRqAvail, TO_INFINITE);
        if (rc)
            return rc;
        rc=FSH_SEMSET(&semRqAvail);
        if (rc)
            return rc;
        return NO_ERROR;
    case FSCTL_STUBFSD_DONE_REQUEST:
        if (QueryPID()!=PidAgent)
            return TONIGY_NOT_DAEMON;
        rc=FSH_SEMCLEAR(&semRqDone);
        if (rc)
            return rc;
         return NO_ERROR;
    default:
        rc=LockBuffers();
        if (rc)
            return rc;
        p->func=func;
        p->cbmax=cbmaxdata;
        Request->rq=REQ_FSCTL;
        rc=signalDaemonAndWait();
        if (rc)
            UNLOCK_AND_EXIT(rc);
        if (p->cb)
        {
            rc=FSH_PROBEBUF(PB_OPWRITE, (PCHAR)cbdata, sizeof(USHORT));
            if (rc)
                UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
            *cbdata=p->cb;
            if (!Request->rc)
            {
                if (*cbdata>cbmaxdata)
                    UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
                if (*cbdata)
                {
                    rc=FSH_PROBEBUF(PB_OPWRITE, data, *cbdata);
                    if (rc)
                        UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
                    memcpy(data, FSData, *cbdata);
                }
            }
        }
        UNLOCK_AND_EXIT(Request->rc);
    }
}

APIRET IFSEXPORT FS_FSINFO(USHORT flag,
                           USHORT vpb,
                           PCHAR data,
                           USHORT cb,
                           USHORT level)
{
    APIRET rc;
    PIFS_FSINFO p=&Request->data.fsinfo;
    
    rc=LockBuffers();
    if (rc)
        return rc;
    rc=QueryDiskID(&p->diskid, vpb);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    p->flag=flag;
    p->cb=cb;
    p->level=level;
    Request->rq=REQ_FSINFO;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    if ((flag==INFO_RETRIEVE) && !Request->rc)
    {
        cb=p->cb;
        if (cb)
        {
            rc=FSH_PROBEBUF(PB_OPWRITE, data, cb);
            if (rc)
                UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
            memcpy(data, FSData, cb);
        }
    }
    UNLOCK_AND_EXIT(Request->rc);
}

APIRET IFSEXPORT FS_INIT(PSZ parm,
                         ULONG devhlp,
                         PULONG minifsd)
{
    static CHAR title[]="Tonigy IFS v1.7 is installed.\r\n";
    APIRET rc;
    SEL sels[2];

    Device_Help=(PFN)devhlp;
    PidAgent=0;

    rc=DevHelp_AllocGDTSelector(sels, 2);
    if (rc)
        return rc;
    DosPutMessage(1, sizeof(title)-1, title);
    Request=MAKEP(sels[0], 0);
    FSData=MAKEP(sels[1], 0);
    return NO_ERROR;
}

APIRET IFSEXPORT FS_OPENCREATE(struct cdfsi FAR* pcdfsi,
                               struct cdfsd FAR* pcdfsd,
                               PSZ name,
                               USHORT curdirend,
                               struct sffsi FAR* psffsi,
                               struct sffsd FAR* psffsd,
                               ULONG mode,
                               USHORT flag,
                               PUSHORT action,
                               USHORT attr,
                               PEAOP eabuf,
                               PUSHORT genflag)
{
    APIRET rc;
    PIFS_OPENCREATE p = &Request->data.opencreate;
    
    rc=LockBuffers();
    if (rc)
        return rc;
    rc=QueryDiskID(&p->diskid, pcdfsi->cdi_hVPB);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    if (strlen(name)>=CCHMAXPATH)
        UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
    strcpy(p->name, name);
    p->sffsi=*psffsi;
    p->mode=mode;
    p->flag=flag;
    p->attr=attr;
    Request->rq=REQ_OPENCREATE;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    if (!Request->rc)
    {
        *psffsi=p->sffsi;
        if (psffsi->sfi_type==STYPE_FILE)
            COPYPTR(psffsd, &p->fileid);
        *action=p->action;
        *genflag=0;
    }
    UNLOCK_AND_EXIT(Request->rc);
}

APIRET IFSEXPORT FS_PATHINFO(USHORT flag,
                             struct cdfsi FAR* pcdfsi,
                             struct cdfsd FAR* pcdfsd,
                             PSZ name,
                             USHORT curdirend,
                             USHORT level,
                             PCHAR data,
                             USHORT cb)
{
    APIRET rc;
    PIFS_PATHINFO p=&Request->data.pathinfo;

    rc=LockBuffers();
    if (rc)
        return rc;
    rc=FileInfoPre(flag, level, cb, data);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    rc=QueryDiskID(&p->diskid, pcdfsi->cdi_hVPB);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    p->flag=flag;
    if (strlen(name)>=CCHMAXPATH)
        UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
    strcpy(p->name, name);
    p->level=level;
    p->cb=cb;
    Request->rq=REQ_PATHINFO;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    if (Request->rc)
    {
        if (flag!=PI_RETRIEVE &&
            level==FIL_QUERYEASIZE)
        {
            rc=SetOError((PEAOP)data, 0);
            if (rc)
                UNLOCK_AND_EXIT(rc);
        }
        UNLOCK_AND_EXIT(Request->rc);
    }
    rc=FileInfoPost(flag, level, cb, data);
    UNLOCK_AND_EXIT(rc);
}

APIRET IFSEXPORT FS_READ(struct sffsi FAR* psffsi,
                         struct sffsd FAR* psffsd,
                         PCHAR data,
                         PUSHORT cb,
                         USHORT ioflag)
{
    APIRET rc;
    PIFS_READ p=&Request->data.read;

    rc=LockBuffers();
    if (rc)
        return rc;
    rc=QueryDiskID(&p->diskid, psffsi->sfi_hVPB);
    if (rc)
        UNLOCK_AND_EXIT(rc);
    COPYPTR(&p->fileid, psffsd);
    p->sffsi=*psffsi;
    p->cb=*cb;
    Request->rq=REQ_READ;
    rc=signalDaemonAndWait();
    if (rc)
        UNLOCK_AND_EXIT(rc);
    if (!Request->rc)
    {
        *psffsi=p->sffsi;
        *cb=p->cb;
        if (*cb)
        {
            rc=FSH_PROBEBUF(PB_OPWRITE, data, *cb);
            if (rc)
                UNLOCK_AND_EXIT(ERROR_INVALID_PARAMETER);
            memcpy(data, FSData, *cb);
        }
    }
    UNLOCK_AND_EXIT(Request->rc);
}
