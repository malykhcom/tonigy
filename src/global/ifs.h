#ifndef __IFS_H__
#define __IFS_H__

#include "fsd.h"

#pragma pack(4)

#define IFS_NAME "TONIGY"

#define FSCTL_STUBFSD_DAEMON_STARTED 0x8001
#define FSCTL_STUBFSD_GET_REQUEST    0x8002
#define FSCTL_STUBFSD_DONE_REQUEST   0x8003
#define FSCTL_STUBFSD_DAEMON_STOPPED 0x8004

#define TONIGY_DAEMON_NOT_RUNNING       ERROR_NO_CHILDREN
#define TONIGY_DAEMON_RUNNING           ERROR_ALREADY_EXISTS
#define TONIGY_NOT_DAEMON               ERROR_ACCESS_DENIED
#define TONIGY_INIT_FAILED              ERROR_INIT_ROUTINE_FAILED
#define TONIGY_NOT_READY                ERROR_NOT_READY
#define TONIGY_AJ_FAILED                2001

#define REQ_ATTACH                1
#define REQ_CHDIR                 2
#define REQ_CHGFILEPTR            3
#define REQ_CLOSE                 4
#define REQ_FILEATTRIBUTE         5
#define REQ_FILEINFO              6
#define REQ_FINDCLOSE             7
#define REQ_FINDFIRST             8
#define REQ_FINDFROMNAME          9
#define REQ_FINDNEXT             10
#define REQ_FSCTL                11
#define REQ_FSINFO               12
#define REQ_OPENCREATE           13
#define REQ_PATHINFO             14
#define REQ_READ                 15
#define REQ_DELETE               16

#ifdef RING0
typedef PVOID DISKID;
typedef PVOID FILEID;
typedef PVOID SEARCHID;
#else
typedef ULONG DISKID;
typedef ULONG FILEID;
typedef ULONG SEARCHID;
#endif

#define FSXCHG_ATTACH_DEVMAX  4

typedef struct
{
    DISKID diskid;
    USHORT flag;
    CHAR dev[FSXCHG_ATTACH_DEVMAX];
    USHORT cb;
} IFS_ATTACH, FAR* PIFS_ATTACH;

typedef struct
{
    DISKID diskid;
    USHORT flag;
    CHAR dir[CCHMAXPATH];
} IFS_CHDIR, FAR* PIFS_CHDIR;

typedef struct
{
    DISKID diskid;
    FILEID fileid;
    struct sffsi sffsi;
    LONG offset;
    USHORT type;
} IFS_CHGFILEPTR, FAR* PIFS_CHGFILEPTR;

typedef struct
{
    DISKID diskid;
    FILEID fileid;
    USHORT type;
} IFS_CLOSE, FAR* PIFS_CLOSE;

typedef struct
{
    DISKID diskid;
    CHAR name[CCHMAXPATH];
} IFS_DELETE, FAR* PIFS_DELETE;

typedef struct
{
    DISKID diskid;
    USHORT flag;
    CHAR name[CCHMAXPATH];
    USHORT attr;
} IFS_FILEATTRIBUTE, FAR* PIFS_FILEATTRIBUTE;

typedef struct 
{
    DISKID diskid;
    FILEID fileid;
    USHORT flag;
    struct sffsi sffsi;
    USHORT level;
    USHORT cb;
} IFS_FILEINFO, FAR* PIFS_FILEINFO;

typedef struct 
{
    DISKID diskid;
    SEARCHID searchid;
} IFS_FINDCLOSE, FAR* PIFS_FINDCLOSE;

typedef struct 
{
    DISKID diskid;
    SEARCHID searchid;
    CHAR name[CCHMAXPATH];
    USHORT attr;
    USHORT cb;
    USHORT match;
    USHORT level;
    USHORT flags;
} IFS_FINDFIRST, FAR* PIFS_FINDFIRST;

typedef struct IFS_FINDFROMNAME
{
    DISKID diskid;
    SEARCHID searchid;
    USHORT cb;
    USHORT match;
    USHORT level;
    USHORT flags;
    ULONG position;
} IFS_FINDFROMNAME, FAR* PIFS_FINDFROMNAME;

typedef struct IFS_FINDNEXT
{
    DISKID diskid;
    SEARCHID searchid;
    USHORT cb;
    USHORT match;
    USHORT level;
    USHORT flags;
} IFS_FINDNEXT, FAR* PIFS_FINDNEXT;

typedef struct
{
    USHORT func;
    USHORT cbmax;
    USHORT cb;
} IFS_FSCTL, FAR* PIFS_FSCTL;

typedef struct
{
    DISKID diskid;
    USHORT flag;
    USHORT cb;
    USHORT level;
} IFS_FSINFO, FAR* PIFS_FSINFO;

typedef struct
{
    DISKID diskid;
    FILEID fileid;
    CHAR name[CCHMAXPATH];
    struct sffsi sffsi;
    ULONG mode;
    USHORT flag;
    USHORT action;
    USHORT attr;
} IFS_OPENCREATE, FAR* PIFS_OPENCREATE;

typedef struct 
{
    DISKID diskid;
    USHORT flag;
    CHAR name[CCHMAXPATH];
    USHORT level;
    USHORT cb;
} IFS_PATHINFO, FAR* PIFS_PATHINFO;

typedef struct
{
    DISKID diskid;
    FILEID fileid;
    struct sffsi sffsi;
    USHORT cb;
} IFS_READ, FAR* PIFS_READ;

typedef struct
{
    ULONG rq;
    APIRET rc;
    union
    {
        IFS_ATTACH attach;
        IFS_CHDIR chdir;
        IFS_CHGFILEPTR chgfileptr;
        IFS_CLOSE close;
        IFS_DELETE delete;
        IFS_FILEATTRIBUTE fileattribute;
        IFS_FILEINFO fileinfo;
        IFS_FINDCLOSE findclose;
        IFS_FINDFIRST findfirst;
        IFS_FINDFROMNAME findfromname;
        IFS_FINDNEXT findnext;
        IFS_FSCTL fsctl;
        IFS_FSINFO fsinfo;
        IFS_OPENCREATE opencreate;
        IFS_PATHINFO pathinfo;
        IFS_READ read;
    } data;
} FSREQUEST, FAR* PFSREQUEST;

typedef CHAR FSDATA[65536];

typedef PCHAR PFSDATA;

typedef struct
{
#ifdef RING0
    LIN        linRequest;
    LIN        linData;
#else
    PFSREQUEST Request;
    PFSDATA    Data;
#endif
} SETXCHGBUFFERS, FAR* PSETXCHGBUFFERS;

#pragma pack()

#endif
