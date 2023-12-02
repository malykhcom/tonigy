#include "global.h"

typedef struct _MEMORYBLOCK
{
    BOOL Free;
    PVOID Pointer;
    ULONG Size;
} MEMORYBLOCK;

#define BLOCKS      1000

static MEMORYBLOCK Blocks[BLOCKS];

VOID debugInit(VOID)
{
    ULONG t;
    for(t=0;t<BLOCKS;t++)
        Blocks[t].Free=TRUE;
}

PVOID debugAlloc(ULONG size)
{
    ULONG t;
    PVOID tmp;
    tmp=malloc(size);
    if (!tmp)
    {
        printf("debugAlloc: failed\n");
        return NULL;
    }
    for(t=0;t<BLOCKS;t++)
        if (Blocks[t].Free)
        {
            Blocks[t].Free=FALSE;
            Blocks[t].Pointer=tmp;
            Blocks[t].Size=size;
            return tmp;
        }
    printf("debugAlloc: too many blocks\n");
    return NULL;
}

VOID debugFree(PVOID pointer)
{
    ULONG t;
    for(t=0;t<BLOCKS;t++)
        if (!Blocks[t].Free && Blocks[t].Pointer==pointer)
        {
            Blocks[t].Free=TRUE;
            free(pointer);
            return;
        }
    printf("debugFree: %x not found\n", pointer);
}

VOID debugTerm(VOID)
{
    ULONG t;
    ULONG f=0;
    for(t=0;t<BLOCKS;t++)
        if (!Blocks[t].Free)
        {
            FILE* tmp;
            CHAR name[20];
            printf("debugTerm: pointer=%x size=%u\n",
                   Blocks[t].Pointer,
                   Blocks[t].Size);
            _snprintf(name, sizeof(name), "%u._!!", f);
            tmp=fopen(name, "wb");
            fwrite(Blocks[t].Pointer, 1, Blocks[t].Size,  tmp);
            fclose(tmp);
            f++;
        }
}
