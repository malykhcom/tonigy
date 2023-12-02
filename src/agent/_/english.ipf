:userdoc.
:title.Tonigy documentation
:h1.
About Tonigy
.br
:hp2.
Tonigy v1.2. Audio/Video CD IFS for OS/2. The smart replacement of grabbers.
:ehp2.
:p.
Tonigy is an Audio/Video CD IFS for OS/2 that maps tracks of Audio CD into
WAV or RAW files and tracks of Video CD into MPG files. Use it for 
accessing audio and video tracks like ordinary files.
:p.
Main features&colon.
:ul compact.
:li.IFS for OS/2
:li.represents audio tracks as WAV or RAW files
:li.represents video tracks as MPG files
:li.easy control and setup by PM GUI panel
:li.English/German interface and documentation
:li.supports extended attributes (reading/writing)
:li.supports adaptive Anti-Jittering for reading audio sectors
:li.&odq.Highest Priority&cdq. option for slow CD-ROM drives and PCs
:li.logging warning and error messages
:li.supports control through named pipe
:eul.
:h1.
Usage help
:p.
Press the F1 key to call the online help when using Tonigy.
:h2.
Quick start
:ol compact.
:li.Run the Tonigy program object from the Tonigy WPS folder.
:li.Select a CD-ROM drive that will be used by Tonigy.
:li.Push the Setup button or press Alt+S.
:li.Select any free (not BUSY) letter for a Tonigy drive in the Setup dialog.
:li.Push the OK button or press Enter to close the Setup dialog.
:li.Push the Attach button or press Alt+A to attach the Tonigy drive.
:li.Work with the Tonigy drive.
:li.After working close the Tonigy program. It will detach the Tonigy drive automaticly.
:eol.
:p.
If you want to change the using language, call pop-up menu 
(by clicking button 2 of the pointing device on the Main window's workspace),
switch Language and restart Tonigy.
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
:dd.:link reftype=hd res=2.Setup drive:elink.
:dt.Alt+X or Alt+F4
:dd.Exit
:edl.
:p.
Use the &odq.:link reftype=hd res=3.General options:elink.&cdq. choice in 
the pop-up menu (called by clicking button 2 of the pointing device on 
the main window's workspace) to change global Tonigy settings.
:p.
If the &odq.Autoattach&cdq. is on, this drive will be attached automaticly when
Tonigy is started.
:warning.All Tonigy drives will be detached automaticly when Tonigy is closed.
:ewarning.
:h2 res=2.
Setup Drive window
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
:h2 res=3.
General Options window
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
:h2 res=4.
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
:h1.
History
:p.
Notations&colon.
:dl compact.
:dt.INF
:dd.Information
:dt.NEW
:dd.New feature
:dt.FIX
:dd.Fixed
:dt.CHG
:dd.Changed
:edl.
:fn id=INF.Information:efn.
:fn id=NEW.New feature:efn.
:fn id=FIX.Fixed:efn.
:fn id=CHG.Changed:efn.
:h2.
v1.0
:p.
Version 1.0
:dl compact.
:dt.:link reftype=fn refid=INF.INF:elink.&colon.
:dd.First release
:edl.
:h2.
v1.1
:p.
Version 1.1
:dl compact.
:dt.:link reftype=fn refid=NEW.NEW:elink.&colon.
:dd.Information about sectors blocks is added to CDINFO.TXT
:dt.:link reftype=fn refid=NEW.NEW:elink.&colon.
:dd.BLOCKS.RAW directory
:dt.:link reftype=fn refid=NEW.NEW:elink.&colon.
:dd.Pop-up menu in the main window
:dt.:link reftype=fn refid=NEW.NEW:elink.&colon.
:dd."General Option" dialog
:dt.:link reftype=fn refid=NEW.NEW:elink.&colon.
:dd."Highest Priority" option
:dt.:link reftype=fn refid=NEW.NEW:elink.&colon.
:dd.German interface and documentation
:dt.:link reftype=fn refid=NEW.NEW:elink.&colon.
:dd.First letter of CD-ROM drives selection 
:dt.:link reftype=fn refid=NEW.NEW:elink.&colon.
:dd.Logging warning and error messages into a virtual file on a Tonigy disk
:dt.:link reftype=fn refid=NEW.NEW:elink.&colon.
:dd.Online help (English and German).
:dt.:link reftype=fn refid=NEW.NEW:elink.&colon.
:dd.Named pipe for control Tonigy
:dt.:link reftype=fn refid=CHG.CHG:elink.&colon.
:dd.Background color of the main window is changed to pale gray
:dt.:link reftype=fn refid=CHG.CHG:elink.&colon.
:dd.New method of Jitter detection
:dt.:link reftype=fn refid=CHG.CHG:elink.&colon.
:dd.README file is renamed to README.TXT
:dt.:link reftype=fn refid=CHG.CHG:elink.&colon.
:dd.RAW directory is renamed to TRACKS.RAW
:dt.:link reftype=fn refid=CHG.CHG:elink.&colon.
:dd.Tonigy web site URL and support E-Mail are changed
:dt.:link reftype=fn refid=CHG.CHG:elink.&colon.
:dd.Minor changes
:edl.
:h2.
v1.2
:p.
Version 1.2
:dl compact.
:dt.:link reftype=fn refid=NEW.NEW:elink.&colon.
:dd.Mapping video tracks into MPG files
:dt.:link reftype=fn refid=CHG.CHG:elink.&colon.
:dd.Advanced information about tracks
:dt.:link reftype=fn refid=CHG.CHG:elink.&colon.
:dd.Minor changes
:edl.
:h1.
License Information
:p.
:hp2.
NOTICE TO USERS&colon. CAREFULLY READ THE FOLLOWING LEGAL AGREEMENT. USE OF THE
SOFTWARE PROVIDED WITH THIS AGREEMENT (THE "SOFTWARE") CONSTITUTES YOUR
ACCEPTANCE OF THESE TERMS. IF YOU DO NOT AGREE TO THE TERMS OF THIS AGREEMENT, 
DO NOT INSTALL  AND/OR USE THIS SOFTWARE. USER'S USE OF THIS SOFTWARE IS
CONDITIONED UPON COMPLIANCE BY USER WITH THE TERMS OF THIS AGREEMENT.
:ehp2.
:p.
:hp2.LICENSE GRANT.:ehp2. Deffie Group grants you a license to use one copy of this
SOFTWARE on any one hardware product for as many licenses as you purchase.
"You" means the company, entity or individual whose funds are used to pay the
license fee. "Use" means storing, loading, installing, executing or displaying
the SOFTWARE. You may not  modify the  SOFTWARE or disable any licensing or
control features of the SOFTWARE  except as an intended part of the SOFTWARE's
programming features. When you first obtain a copy of the SOFTWARE, you are 
granted an evaluation period of not more than 30 days, after which time you
must pay for the SOFTWARE according to the terms and  prices discussed in the
SOFTWARE's documentation, or you must remove the SOFTWARE from your computer. 
This license is not transferable to any other hardware product or other
company, entity, or individual.
:p.
:hp2.NO OTHER WARRANTIES. DEFFIE GROUP DOES NOT WARRANT THAT THE SOFTWARE IS ERROR
FREE. DEFFIE GROUP DISCLAIMS ALL OTHER WARRANTIES WITH RESPECT TO THE SOFTWARE,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD
PARTY RIGHTS. SOME JURISDICTIONS DO NOT ALLOW THE  EXCLUSION OF IMPLIED 
WARRANTIES OR LIMITATIONS ON HOW LONG AN IMPLIED WARRANTY MAY LAST, OR THE
EXCLUSION OR LIMITATION OF INCIDENTAL OR CONSEQUENTIAL DAMAGES, SO THE ABOVE
LIMITATIONS OR EXCLUSIONS MAY NOT APPLY TO YOU. THIS WARRANTY GIVES YOU
SPECIFIC LEGAL RIGHTS AND YOU MAY ALSO HAVE OTHER RIGHTS WHICH VARY FROM
JURISDICTION TO JURISDICTION.
:ehp2.
:h1.
Registration
:p.
The registration fee is only $20. Once registered you remain a registered user
for all future Tonigy versions.
.br
.br
For registration or additional information, visit the 
&odq.:hp2.Register Now!:ehp2.&cdq. section of the Tonigy site 
(:hp4.http&colon.//www.tonigy.com:ehp4.).
:h1.
Support and Feedback
:p.
Send questions, bug reports and suggestions to :hp4.tonigy@defgroup.com:ehp4.
.br
Visit the Tonigy site&colon. :hp4.http&colon.//www.tonigy.com:ehp4.
:euserdoc.
