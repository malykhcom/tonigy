#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#define INCL_DOS
#define INCL_ERRORS
#define INCL_PM
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <stdlib.h>
#include <process.h>
#include <limits.h>
#include <stddef.h>
#include <time.h>
#include <io.h>

#include "ifs.h"

#define TONIGY      "Tonigy v1.7"
#define COPYRIGHT   "(c) Copyright DEF Group, 2001-2002"

#define SECTOR_SIZE 2352
#define SECTORS     27
#define COMPARE     512
#define JITTER      5
#define SPIN_UP     10
#define REREADS     5

#define T_FILES     30
#define T_SEARCHES  30

#define ID_EMPTY    ULONG_MAX
#define GLOBAL_DATE ((((21<<4)+1)<<5)+1)
#define GLOBAL_TIME 0x0000
#define ATTR_FILE   (FILE_NORMAL)
#define ATTR_DIR    (FILE_NORMAL | FILE_DIRECTORY)

#define LOG_LINE    82

#define FILE_NAME   60
#define EA_APP      60

#define LANGS       3

typedef PSZ LANGSTRS[LANGS];

#pragma pack(1)
typedef struct
{
    CHAR rID[4];
    ULONG rLen;
    CHAR wID[4];
    CHAR fID[4];
    ULONG fLen;
    USHORT Tag;
    USHORT Channels;
    ULONG Rate;
    ULONG Bytes;
    USHORT Align;
    USHORT Bits;
    CHAR dID[4];
    ULONG dLen;
} WAVHEADER, * PWAVHEADER;
#pragma pack()

typedef struct
{
    CHAR Name[FILE_NAME];
    ULONG Size;
    ULONG Alloc;
    USHORT Attr;
    ULONG EASize;
    PFEALIST EA;
} FILEINFO, * PFILEINFO;

typedef struct
{
    ULONG Number;
    CHAR Name[9];
    enum
    {
        TRACK_TYPE_INVALID, TRACK_TYPE_UNKNOWN,
        TRACK_TYPE_AUDIO, TRACK_TYPE_ISO, TRACK_TYPE_BOOT, TRACK_TYPE_VIDEO,
        TRACK_TYPE_CDI
    } Type;
    ULONG Start;
    ULONG Len;
    UCHAR Mode;
    ULONG RealLen;
    USHORT SectorSize;
    USHORT SectorOffset;
    ULONG FirstSkip; // For VIDEO/MPG only
    ULONG RootSector; // For CD-I only
} CDTRACKINFO, * PCDTRACKINFO;

typedef struct _DIRELEMENT* PDIRELEMENT;
typedef struct _DIRELEMENT
{
    CHAR Name[FILE_NAME];
    CHAR EAApp[EA_APP];
    ULONG Size;
    ULONG Alloc;
    USHORT Attr;
    enum
    {
        ELEMENT_UNKNOWN,
        ELEMENT_DIR,
        ELEMENT_README, ELEMENT_CDINFO, ELEMENT_LOG, ELEMENT_UNREG,
        ELEMENT_AUDIO_TRACK_RAW,
        ELEMENT_AUDIO_TRACK_WAV,
        ELEMENT_AUDIO_BLOCK_RAW,
        ELEMENT_VIDEO,
        ELEMENT_ISO,
        ELEMENT_CDIFILE
    } Type;
    union
    {
        PDIRELEMENT Dir;
        ULONG Index;
        ULONG Sector;
    };
}  DIRELEMENT;

typedef struct
{
    BOOL Antijitter;
    ULONG Preread;
    ULONG Tries;
} GRABOPTIONS, * PGRABOPTIONS;


typedef struct
{
    BOOL Inited;
    BOOL Block;
    BOOL Raw;
    GRABOPTIONS Options;
    WAVHEADER WavHeader;
    ULONG Index;
    ULONG Number;
    ULONG Start;
    ULONG Sector;
    ULONG Len;
    PCHAR Data;
    LONG Preread;
    LONG Pointer;
} AUDIOFILEDATA, * PAUDIOFILEDATA;

typedef struct
{
    ULONG Index;
    ULONG Number;
    ULONG Start;
    ULONG Sectors;
    ULONG SectorSize;
    USHORT SectorOffset;
    ULONG Sector;
    ULONG Len;
    PCHAR Data;
} DATAFILEDATA, * PDATAFILEDATA;

typedef struct
{
    ULONG ID;
    ULONG FilesNum;
    PFILEINFO Files;
    ULONG NextPos;
} TSEARCH, * PTSEARCH;

typedef struct
{
    ULONG ID;
    BOOL Ready;
    CHAR EAApp[EA_APP];
    enum
    {
        FILE_TYPE_README, FILE_TYPE_CDINFO, FILE_TYPE_LOG, FILE_TYPE_UNREG,
        FILE_TYPE_AUDIO,
        FILE_TYPE_DATA,
    } Type;
    union
    {
        PSZ Info;
        PSZ Log;
        AUDIOFILEDATA Audio;
        DATAFILEDATA Data;
    } data;
} TFILE, * PTFILE;

typedef struct _LOGRECORD* PLOGRECORD;
typedef struct _LOGRECORD
{
    DATETIME DateTime;
    enum {LOG_ATTACHED, LOG_CHANGED, LOG_JITTER, LOG_READ, LOG_AJ} Type;
    BOOL Block;
    ULONG Number;
    ULONG Sector;
    BOOL Fatal;
    PLOGRECORD Next;
} LOGRECORD;

typedef struct
{
    ULONG Drive;
    CHAR Letter;
    CHAR StartLetter;
    HFILE Handle;
    ULONG Sectors;
    ULONG TracksNum;
    PCDTRACKINFO Tracks;
    ULONG BlocksNum;
    PCDTRACKINFO Blocks;
    TFILE Files[T_FILES];
    TSEARCH Searches[T_SEARCHES];
    enum {AJ_UNKNOWN, AJ_NEED, AJ_NOT_NEED} AJNeed;
    HMTX OptionsMutex;
    PBOOL Log;
    ULONG LogRecords;
    PLOGRECORD LogFirst;
    PLOGRECORD LogLast;
    PDIRELEMENT Root;
} TDISK, * PTDISK;

typedef struct
{
    CHAR Letter;
    BOOL Attached;
    BOOL AutoAttach;
    GRABOPTIONS Options;
    BOOL Log;
} IOPTIONS, * PIOPTIONS;

typedef struct
{
    HAB Hab;
    PCHAR Key;
    CHAR AutoCDLetter;
    UINT KeyLen;
    CHAR KeyRes;
    INT Agent;

    PCHAR MsgText;
    ULONG Drives;
    CHAR StartLetter;
    HINI HiniEA;
    HINI HiniOptions;
    PFSREQUEST Request;
    PCHAR MsgTitle;
    PFSDATA Data;
    PTDISK Disks;
    
    PCHAR Readme;
    PCHAR Unreg;
    LONG X, Y, W, H;
    LONG SetupX, SetupY;
    BOOL Min;
    CHAR FirstLetter;
    BOOL HighPrio;
    ULONG Lang;
    HMTX OptionsMutex;
    PIOPTIONS Options;
} MAINDATA, * PMAINDATA;

extern CHAR ReadMe[];
extern ULONG Lang;

#define TMSG_ATTACH (WM_USER+1)
#define TMSG_DETACH (WM_USER+2)

#include "locale.h"
#include "debug.h"
#include "agent.h"
#include "cdrom.h"
#include "file.h"
#include "common.h"
#include "ea.h"
#include "util.h"
#include "ui.h"
#include "errors.h"
#include "key.h"
#include "npipe.h"
#include "iso.h"
#include "video.h"
#include "audio.h"
#include "cdi.h"
#include "button.h"

APIRET openVerifyFlags(ULONG, USHORT);
APIRET openAllocFile(PTDISK, PTFILE*);
VOID openSetup(PTFILE, PIFS_OPENCREATE);

_inline ULONG Extract4(PCHAR buf)
{
    ULONG ret;
    ret=buf[0];
    ret<<=8;
    ret|=buf[1];
    ret<<=8;
    ret|=buf[2];
    ret<<=8;
    ret|=buf[3];
    return ret;
}

_inline USHORT Extract2(PCHAR buf)
{
    USHORT ret;
    ret=buf[0];
    ret<<=8;
    ret|=buf[1];
    return ret;
}

#endif
