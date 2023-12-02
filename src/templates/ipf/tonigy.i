#include "ipf.h"
#autotag ':' '&colon.'
#autotag on
#include "..\global\global.h"
#include "..\global\generale.h"
#include "..\global\historye.h"
#include "..\global\license.h"
#autotag off

<$start title='<$HelpTitle>'>
<$h1><$HelpAbout>
.br
<$bold '<$TonigyDesc>. <$TonigySlogan>'>
<$p '<$TonigyLongDesc>'>
<$p '<$Feature0>'>
<$startlist>
<$list><$Feature1>
<$list><$Feature2>
<$list><$Feature3>
<$list><$Feature4>
<$list><$Feature5>
<$list><$Feature6>
<$list><$Feature7>
<$list><$Feature8>
<$list><$Feature9>
<$list><$Feature10>
<$list><$Feature11>
<$list><$Feature12>
<$list><$Feature13>
<$endlist>

<$h1>Usage help
<$p 'Press the F1 key to call the online help when using Tonigy.'>

<$h2>Quick start
<$startolist>
<$olist><$QuickStart1>
<$olist><$QuickStart2>
<$olist><$QuickStart3>
<$olist><$QuickStart4>
<$olist><$QuickStart5>
<$olist><$QuickStart6>
<$olist><$QuickStart7>
<$olist><$QuickStart8>
<$endolist>

:h2 res=1.
Main window
:p.
You can use following keys to control the Tonigy main window:
:dl compact tsize=20.
:dt.Alt+A
:dd.Attach drive
:dt.Alt+D
:dd.Detach drive
:dt.Alt+S
:dd.:link reftype=hd res=2.Setup:elink.
:dt.Alt+X or Alt+F4
:dd.Exit
:edl.
:p.
If the &odq.Autoattach&cdq. is on, this drive will be attached automaticly when
Tonigy is started.
:warning.All Tonigy drives will be detached automaticly when Tonigy is closed.
:ewarning.

:h2 res=2.
Setup window
:p.
This window contains&colon.
<$startlist>
<$list>:link reftype=hd res=3.Setup Drive:elink. page or pages
<$list>:link reftype=hd res=4.General:elink. page
<$endlist>

:h2 res=3.

Setup Drive page
:p.
If the &odq.Automatic attach&cdq. option is enabled, the drive will be attached
automaticly when Tonigy is started.
:p.
You can log all important Tonigy events into the virtual 
:link reftype=hd refid=log.TONIGY.LOG:elink. file on
a Tonigy drive by enabling &odq.Log warnings and errors&cdq. options.
:p.
Some CD-ROM drive models has troubles with synchronization of reading audio
sectors (jitter).
To
correct such troubles, you can use the 
&odq.:link reftype=hd refid=aj.Anti-Jittering:elink.&cdq. feature of Tonigy.
To determine presence of jitter, work
with any RAW or WAV file from a Tonigy drive (copy this file, play it or view it by
HEX-editor; state of &odq.Anti-Jittering&cdq. options is unimportant). Then
search the CDINFO.TXT on the Tonigy drive for a text after line
:xmp.
x audio blocks(s).
:exmp.
:p.
If this text is 
:xmp.
This CD-ROM drive has no troubles with synchronization of reading.
:exmp.
:p.
you may disable &odq.Anti-Jittering&cdq..
:note.&odq.Anti-Jittering&cdq. slows reading speed, therefore disable it if 
the CD-ROM drive has no jitter and you want to increase track
reading speed.
:p.
The &odq.Tries to correct&cdq. option sets the amount of attempts to correct
troubles with synchronization of reading. If troubles cannot be corrected after
choosed amount of attempts, a really readed data will be returned to program,
which is using a Tonigy drive (see also :link reftype=hd refid=log.TONIGY.LOG:elink.).
If you use a slow CD-ROM drive and listen to WAV or RAW files from a Tonigy
drive, the recommended amount of attempts is value in the range 2 through 5. But
when WAV or RAW files are encoded (copied) from a Tonigy drive or
a enough fast CD-ROM drive is used, set a bigger
value. One try only is not recommended anyway.
:h2 res=4.
General page
:p.
&odq.Highest priority&cdq. changes the execution priority of the sectors
reading thread. If this options is enabled, the time-critical 
priority class and priority level 31 are set for the reading thread. 
Otherwise, the reading thread has the regular priority class and priority
level 31. If you hear breaks of the sound when WAV or RAW files are being
played during a high disk activity, enable the &odq.Highest Priority&cdq. option.
:p.
&odq.Language&cdq. radio buttons set the interface language. You should restart Tonigy
to apply language change.
:h2 res=5.
Attach Failed window
:p.
This window is displayed when Tonigy cannot attach a Tonigy drive. The most
popular reason of such error is the selection a busy drive letter for a Tonigy
drive. Select another drive letter for a Tonigy drive and try to attach
again.
:h1.
Information for advanced users
:p.
This section contains an information for advanced users.
:h2 id=aj.
Anti-Jittering
:p.
An idea of Anti-Jittering is to read audio sectors with some overlap. Many 
grabbers use such idea to correct Jitter, but Tonigy provide new methods for
more smart and reliable Jitter correction. First, a size of the overlap (preread)
can be changed by Tonigy dynamicly using statistical information from
previous sectors reading. Second, Tonigy's Anti-Jittering gives heed to
unstable results of sectors reading.
:h2.
Extended Attributes
:p.
Tonigy IFS supports reading and writing of extended attributes.
:p.
Extended attributes of *.WAV, *.RAW and *.MPG files are compact-disk-dependent.
:p.
Tonigy stores extended attributes in the EA.INI file in the Tonigy directory.
This file is a standard OS/2 user profile (a binary INI file).
:h2.
Named pipe
:p.
Tonigy can be controlled through the \PIPE\TONIGY\CONTROL named pipe.
This pipe is a message pipe, a command should be written to the
pipe as one message. A command consists of one case insensitive word (a
name of the command) and, in some cases, one or more parameters separated by 
the one space symbol. For example&colon.
:xmp.
QueryDrivesNumber
attach 1
:exmp.
:p.
After writing a command to the control named pipe, a program have to read
a return string. A return string begins with a decimal return code.
If the command can return a data and the decimal return code is equal to zero (no
errors), the rest of the return string contains a returned data. For example&colon.
:xmp.
0
31
0 E:
:exmp.
:h3.
QueryDrivesNumber
:p.:hp2.Remarks:ehp2.:p.The command queryes the number of CD-ROM drives.
:p.:hp2.Parameters:ehp2.:p.None
:p.:hp2.Returns:ehp2.:p.ErrorCode
.br
:hp1.or:ehp1.
.br
0 NumberOfDrives
:p.:hp2.Example:ehp2.
:dl compact tsize=20.
:dt.Sended&colon.:dd.QueryDrivesNumber
:dt.Received&colon.:dd.0 2
:dt.Comment&colon.:dd.There are two CD-ROM drives.
:edl.
:h3.
QueryCDROMDrive
:p.:hp2.Remarks:ehp2.:p.The command queryes the letter of CD-ROM drive.
:p.:hp2.Parameters:ehp2.:p.NumberOfDrive
.br
:hp1.the number of the drive in the range 0 through N-1, where N is the number
of CD-ROM drives:ehp1.
:p.:hp2.Returns:ehp2.:p.ErrorCode
.br
:hp1.or:ehp1.
.br
0 LetterOfCDROMDrive.
:p.:hp2.Example:ehp2.
:dl compact tsize=20.
:dt.Sended&colon.:dd.QueryCDROMDrive 1
:dt.Received&colon.:dd.0 E&colon.
:dt.Comment&colon.:dd.A letter of the second CD-ROM drive is F&colon..
:edl.
:h3.
QueryTonigyDrive
:p.:hp2.Remarks:ehp2.:p.The command queryes the letter of the Tonigy drive.
:p.:hp2.Parameters:ehp2.:p.NumberOfDrive
.br
:hp1.the number of the drive in the range 0 through N-1, where N is the number
of CD-ROM drives:ehp1.
:p.:hp2.Returns:ehp2.:p.ErrorCode
.br
:hp1.or:ehp1.
.br
0 LetterOfTonigyDrive.
.br
:hp1.if the letter of the Tonigy drive is not selected, an empty string is
returned as LetterOfTonigyDrive:ehp1.
:p.:hp2.Example:ehp2.
:dl compact tsize=20.
:dt.Sended&colon.:dd.QueryTonigyDrive 1
:dt.Received&colon.:dd.0 T&colon.
:dt.Comment&colon.:dd.A letter of the second Tonigy drive drive is T&colon..
:edl.
:h3.
QueryStatus
:p.:hp2.Remarks:ehp2.:p.The command queryes the status of the Tonigy drive.
:p.:hp2.Parameters:ehp2.:p.NumberOfDrive
.br
:hp1.the number of the drive in the range 0 through N-1, where N is the number
of CD-ROM drives:ehp1.
:p.:hp2.Returns:ehp2.:p.ErrorCode
.br
:hp1.or:ehp1.
.br
0 Status
.br
:hp1.0 as Status - the drive is detached, 1 as Status - the drive is attached:ehp1.
:p.:hp2.Example:ehp2.
:dl compact tsize=20.
:dt.Sended&colon.:dd.QueryStatus 1
:dt.Received&colon.:dd.0 0
:dt.Comment&colon.:dd.The second Tonigy drive is detached.
:edl.
:h3.
Attach
:p.:hp2.Remarks:ehp2.:p.The command attach the Tonigy drive.
:p.:hp2.Parameters:ehp2.:p.NumberOfDrive
.br
:hp1.the number of the drive in the range 0 through N-1, where N is the number
of CD-ROM drives:ehp1.
:p.:hp2.Returns:ehp2.:p.ErrorCode
:p.:hp2.Example:ehp2.
:dl compact tsize=20.
:dt.Sended&colon.:dd.Attach 1
:dt.Received&colon.:dd.0
:dt.Comment&colon.:dd.The second Tonigy drive has been attached.
:edl.
:h3.
Detach
:p.:hp2.Remarks:ehp2.:p.The command detach the Tonigy drive.
:p.:hp2.Parameters:ehp2.:p.NumberOfDrive
.br
:hp1.the number of the drive in the range 0 through N-1, where N is the number
of CD-ROM drives:ehp1.
:p.:hp2.Returns:ehp2.:p.ErrorCode
:p.:hp2.Example:ehp2.
:dl compact tsize=20.
:dt.Sended&colon.:dd.Detach 1
:dt.Received&colon.:dd.0
:dt.Comment&colon.:dd.The second Tonigy drive has been detached.
:edl.
:h3.
Usage example (REXX)
:xmp.
/* */

call RxFuncAdd "SysLoadFuncs", "REXXUTIL", "SysLoadFuncs"
call SysLoadFuncs

TonigyPipe="\PIPE\TONIGY\CONTROL"

if Stream(TonigyPipe, "c", "open")\="READY&colon." then
do
    say "Tonigy is not running."
    return
end

Drives=QueryDrivesNumber()

say "Drives&colon. "Drives
say
call PrintStatus

say
say "Press any key to detach all Tonigy drives"
'@pause>nul'
say
do i=0 to Drives-1
    call Detach i
end
call PrintStatus

say
say "Press any key to attach all Tonigy drives"
'@pause>nul'
say
do i=0 to Drives-1
    call Attach i
end
call PrintStatus

return

PrintStatus&colon.
    do i=0 to Drives-1
        if QueryStatus(i)==1 then
            Status="attached"
        else
            Status="detached"
        say "Drive "i"&colon. "QueryCDROMDrive(i)"->"QueryTonigyDrive(i)" ("Status")"
    end
return

QueryDrivesNumber&colon.
    call CharOut TonigyPipe, "QueryDrivesNumber"
    parse value LineIn(TonigyPipe) with code drives
    if code=0 then
        return drives
return 0

QueryCDROMDrive&colon.
    parse arg drive
    call CharOut TonigyPipe, "QueryCDROMLetter "drive
    parse value LineIn(TonigyPipe) with code letter
    if code=0 then
        return letter
return ""

QueryTonigyDrive&colon.
    parse arg drive
    call CharOut TonigyPipe, "QueryTonigyLetter "drive
    parse value LineIn(TonigyPipe) with code letter
    if code=0 then
        return letter
return ""

QueryStatus&colon.
    parse arg drive
    call CharOut TonigyPipe, "QueryStatus "drive
    parse value LineIn(TonigyPipe) with code status
    if code=0 then
        return status
return 0

Attach&colon.
    parse arg drive
    call CharOut TonigyPipe, "Attach "drive
return LineIn(TonigyPipe)
    
Detach&colon.
    parse arg drive
    call CharOut TonigyPipe, "Detach "drive
return LineIn(TonigyPipe)
:exmp.
:h2 id=log.
TONIGY.LOG
:note.TONIGY.LOG is a deletable file, therefore you can remove it at any time.
:p.
TONIGY.LOG messages&colon.
:xmp.
Drive x is attached
:exmp.
:p.
The Tonigy drive is attached by user or automaticly.
:xmp.
Header of CD is rescaned
:exmp.
:p.
Tonigy detected a compact disk change and header of compact disk is rescaned.
:xmp.
Jitter is detected, but Anti-Jittering is disabled
:exmp.
:p.
Tonigy detected Jitter during track or block opening, but Anti-Jittering
is disabled. You should enable Anti-Jittering for this Tonigy drive.
:xmp.
Warning&colon. LONG READ failed (track x, sector x)
:exmp.
:xmp.
Warning&colon. LONG READ failed (block x, sector x)
:exmp.
:p.
The LONG READ command returned an error code. The LONG READ command will
be runned over again.
:xmp.
Error&colon. LONG READ failed (track x, sector x)
:exmp.
:xmp.
Error&colon. LONG READ failed (block x, sector x)
:exmp.
:p.
The LONG READ command returned an error code after the 5th try. You
should use another compact disk or change the CD-ROM drive.
:xmp.
Warning&colon. Anti-Jittering failed (track x, sector x)
:exmp.
:xmp.
Warning&colon. Anti-Jittering failed (block x, sector x)
:exmp.
:p.
Jitter of the readed data cannot be corrected. The LONG READ command and
Anti-Jittering will be runned over again.
:xmp.
Error&colon. Anti-Jittering failed (track x, sector x)
:exmp.
:xmp.
Error&colon. Anti-Jittering failed (block x, sector x)
:exmp.
:p.
Jitter of the readed data cannot be corrected after 
:link reftype=hd res=2.choosed amount of tries:elink..
You should change the CD-ROM drive or use another compact disk.

<$h1>History
<$p 'Notations:'>
<$startdict>
<$dictt>INF
<$dictd><$INF_>
<$dictt>NEW
<$dictd><$NEW_>
<$dictt>FIX
<$dictd><$FIX_>
<$dictt>CHG
<$dictd><$CHG_>
<$enddict>
<$footnote 'INF.<$INF_>'>
<$footnote 'NEW.<$NEW_>'>
<$footnote 'FIX.<$FIX_>'>
<$footnote 'CHG.<$CHG_>'>
<$h2>Version 1.0
<$startdict>
<$History0_1>
<$enddict>

<$h2>Version 1.1
<$startdict>
<$History1_1>
<$History1_2>
<$History1_3>
<$History1_4>
<$History1_5>
<$History1_6>
<$History1_7>
<$History1_8>
<$History1_9>
<$History1_10>
<$History1_11>
<$History1_12>
<$History1_13>
<$History1_14>
<$History1_15>
<$History1_16>
<$enddict>

<$h2>Version 1.2
<$startdict>
<$History2_1>
<$History2_2>
<$History2_3>
<$enddict>

<$h2>Version 1.3
<$startdict>
<$History3_1>
<$History3_2>
<$History3_3>
<$History3_4>
<$History3_5>
<$enddict>

<$h2>Version 1.4
<$startdict>
<$History4_1>
<$History4_2>
<$History4_3>
<$History4_4>
<$enddict>

<$h2>Version 1.5
<$startdict>
<$History5_1>
<$History5_2>
<$History5_3>
<$History5_4>
<$History5_5>
<$History5_6>
<$enddict>

<$h2>Version 1.6
<$startdict>
<$History6_1>
<$History6_2>
<$History6_3>
<$enddict>

<$h2>Version 1.7
<$startdict>
<$History7_1>
<$History7_2>
<$enddict>


<$h1><$License0>
:hp2.<$p '<$License1>'>:ehp2.
<$p '<$License2>'>
:hp2.<$p '<$License3>'>:ehp2.

<$h1>Registration
<$p '<$Register1>'>
<$p '<$Register2>'>

<$h1>Support and Feedback
<$p 'Send questions, bug reports and suggestions to \
<$colored "<$Email>">'>
.br
Visit the Tonigy site: <$colored '<$Homepage>'>

<$end>
