#ifndef __AUDIO_H__
#define __AUDIO_H__

_inline VOID audioFileFree(PTFILE file)
{
    M_FREE(file->data.Audio.Data);
}

APIRET audioMakeEA(PCHAR, ULONG, BOOL, PCDTRACKINFO, BOOL);

/*BOOL audioIsTrack(PCSZ, PTDISK, PULONG, PBOOL);
BOOL audioIsBlock(PCSZ, PTDISK, PULONG, PBOOL);  */

APIRET audioOpen(PMAINDATA, PTDISK, PIFS_OPENCREATE, BOOL, ULONG, BOOL, PCSZ);
APIRET audioRead(PMAINDATA, PCHAR, ULONG, ULONG, PTDISK, PTFILE);

PDIRELEMENT blocksRawMakeDirectory(PTDISK disk);
PDIRELEMENT tracksRawMakeDirectory(PTDISK disk);
PDIRELEMENT tracksWavMakeDirectory(PTDISK disk);

#endif
