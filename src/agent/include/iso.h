#ifndef __ISO_H__
#define __ISO_H__

BOOL isoGetInfo(HFILE, PCDTRACKINFO);

PDIRELEMENT isoMakeDirectory(PTDISK);

APIRET isoMakeEA(PCHAR, ULONG, PCDTRACKINFO);

APIRET isoOpen(PMAINDATA, PTDISK, PIFS_OPENCREATE, ULONG, PCSZ);

#endif