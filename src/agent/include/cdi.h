#ifndef __CDI_H__
#define __CDI_H__

BOOL cdiGetInfo(HFILE, PCDTRACKINFO);

PDIRELEMENT cdiMakeDirectory(PTDISK);

APIRET cdiOpen(PMAINDATA, PTDISK, PIFS_OPENCREATE, ULONG, ULONG, PCSZ);
#endif
