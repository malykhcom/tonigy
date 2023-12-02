#ifndef __DEBUG_H__
#define __DEBUG_H__

VOID debugInit(VOID);
PVOID debugAlloc(ULONG);
VOID debugFree(PVOID);
VOID debugTerm(VOID);

#if 0
#define M_INIT() debugInit()
#define M_ALLOC(x) debugAlloc(x)
#define M_FREE(x) debugFree(x)
#define M_TERM() debugTerm()
#else
#define M_INIT() NULL
#define M_ALLOC(x) malloc(x)
#define M_FREE(x) free(x)
#define M_TERM() NULL
#endif

#endif
