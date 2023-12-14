#ifndef __UTIL_H__
#define __UTIL_H__

BOOL utilIsRunning(PHMTX);

VOID utilDetachAll(VOID);
VOID utilAutoAttach(PMAINDATA);
BOOL utilAttach(PMAINDATA, ULONG);
BOOL utilDetach(PMAINDATA, ULONG);
BOOL utilEject(PMAINDATA, ULONG);
_inline VOID utilPrio(INT tid, BOOL high)
{
    DosSetPriority(PRTYS_THREAD, high?3:2, 31, tid);
}

BOOL utilInitMaindata(PSZ, HAB, PMAINDATA);
VOID utilTermMaindata(PMAINDATA);

#endif
