#include "html.h"
#include "..\global\global.h"
#include "..\global\generale.h"

<$start title='FAQ and Tips' \
desc='The additional information about Tonigy' \
keywords='Tonigy,FAQ,tips,compact disk,CD-ROM,WAV,RAW,MPG,MP3,encoder,LONGNAME' \
section='2'> 

<$center '<b>Tips and hints</b>'>

<$p 'You can read the CDINFO.TXT file on a Tonigy drive for some information \
about a compact disk and a CD-ROM drive.'>

<$p 'RAW files do not contain a RIFF WAV header, thus they should be used to \
prevent the L3ENC/2 encoder from adding a click in the begin of MP3 files.'>

<$p 'Tonigy stores extended attributes in the EA.INI file in the Tonigy \
directory. This file is a standard OS/2 user profile (a binary INI file).'>

<$p 'Extended attributes of *.WAV, *.RAW, *.MPG and *.ISO files are \
compact-disk-dependent, therefore ".LONGNAME" attribute can be used to set \
your own WPS titles for these files.'>

<$center '<b>General questions</b>'>

<$p '<b>Q</b>: What are Mode 1 and Mode 2 sectors? \
Why is their size different?'>

<$p '<b>A</b>: A compact disk is divided in sectors. The actual raw size of \
each sector is 2352 bytes. Different CD formats use that \
2352 bytes in different ways:'>

<$startlist>
<$list 'An audio sector uses all 2352 bytes for audio data.'>
<$list 'A Mode 1 sector uses 16 bytes for a synchronization record \
and a sector header, 288 bytes for error detection and correction info. Thus \
size of user data is 2048 bytes in Mode 1.'>
<$list 'A Mode 2 sector uses 24 bytes for a synchronization record, \
a sector header and a subheader. Also 280 bytes are used for error detection \
and correction info in Mode 2 Form 1 or 4 bytes are used for error  \
detection info in Mode 2 Form 2. Thus size of user data is 2048 \
bytes in Mode 2 Form 1 and 2324 bytes in Mode 2 Form 2.'>
<$endlist>

<$center '<b>CD-ROM drive-related and system-wide troubles</b>'>

<a name=OS2CDROM.DMD>

<$p '<b>Q</b>: WAV and RAW files are shown on the Tonigy drive, but the \
SYS0021 (The drive is not ready) error occurs during trying to work with these \
files. The CD-ROM drive capabilities list in the CDINFO.TXT file contains the \
"Does not support reading of CD-DA sectors" line. Can I use Tonigy or other \
audio grabbers with this CD-ROM drive?'>

<$p '<b>A</b>: You cannot if the CD-ROM drive does not support reading of \
CD-DA sectors really. But if the CD-ROM drive supports it and OS/2 provides \
the wrong information about capabilities of the CD-ROM drive you can try \
to use one of modified OS2CDROM.DMD files. Download some versions of \
OS2CDROM.DMD in the [<a href=download.html>Download</a>] section.'>

<$p '<b>Q</b>: Tonigy and some programs (for example, PMView) determine \
the drive letter of the CD-ROM drive wrong. How to correct it?'>

<$p '<b>A1</b>: Try to use the "Reserve Drive Letter" feature of OS/2:'>

<$center 'Reserve Drive Letter'>

<$p 'With RESERVEDRIVELETTER in CONFIG.SYS you have the ability to control \
assignment of some drive letters by reserving one or more consecutive drive \
letters during the earliest part of system initialization. (Note: In Warp4+ \
you can assign this option with the Reserved page of "Drives - Properties" \
dialog. CONFIG.SYS will automaticly be updated with the values you specified. \
In Warp3 this feature can be used after installing of FixPack 25 and later \
only.)<br> \
Syntax: RESERVEDRIVELETTER=drive<br> \
Remark: CD-ROM, Optical drives, Virtual disks and Tape drives gets drive \
letters beginning with the NEXT letter you have defined with \
RESERVEDRIVELETTER. For example you have a  harddisk with two partitions \
and one CD-ROM drive. Without RESERVEDRIVELETTER setup the hard disk partitions \
will get drive letter C: and D:, the CD-ROM drive will get drive letter E:. \
If you want your CD-ROM drive to become drive G: you should use \
"RESERVEDRIVELETTER=F:".'>

<$p '<b>A2</b>: If it appears after installing FP14 for OS/2 Warp v4.0 or FP1 \
for OS/2 WarpServer v4.5 (for e-Business), renew OS2KRNL and OS2LDR: \ 
<a href=ftp://ftp.software.ibm.com/ps/products/os2/fixes/v4warp/english-us/xr_m014/w41106.zip>w41106.zip</a> \
(for Warp v4.0), \
<a href=ftp://ftp.software.ibm.com/ps/products/os2/fixes/v4.5warp/english-us/xr_e001/uni1106.zip>uni1106.zip</a> \
(for WarpServer v4.5-UNI), or \
<a href=ftp://ftp.software.ibm.com/ps/products/os2/fixes/v4.5warp/english-us/xr_e001/smp1106.zip>smp1106.zip</a> \
(for WarpServer v4.5-SMP). The size of each zip-archive is about 800-900 kb.'>

<$end>
