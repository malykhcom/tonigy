#ifndef __VIDEO_H__
#define __VIDEO_H__

_inline VOID dataFileFree(PTFILE file)
{
    M_FREE(file->data.Data.Data);
}

BOOL videoGetInfo(CHAR, HFILE, ULONG, PCDTRACKINFO);

//BOOL videoIsTrack(PCSZ, PTDISK, PULONG);

APIRET videoMakeEA(PCHAR, ULONG, PCDTRACKINFO);

APIRET videoOpen(PMAINDATA, PTDISK, PIFS_OPENCREATE, ULONG, PCSZ);

APIRET dataRead(PCHAR, ULONG, ULONG, PTDISK, PTFILE);

#endif
