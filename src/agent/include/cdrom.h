#ifndef __CDROM_H__
#define __CDROM_H__

BOOL cdromDrives(PULONG, PCHAR);

BOOL cdromQuery(PMAINDATA, PTDISK);
VOID cdromClose(PTDISK);
PSZ cdromInfo(PTDISK);
PSZ cdromLog(PTDISK);

BOOL cdromReadLong(HFILE, ULONG, ULONG, PCHAR);
BOOL cdromGrab(PTDISK, BOOL, ULONG, ULONG, ULONG, PCHAR);

#endif
