/*
 This REXX script adds .LONGNAME EAs to WAV-files on Tonigy drives.
 Information to add is obtained from a CDDB server with the CDDBQuery utility.
 */

CDDBServer="www.freedb.org"
CDDBPort="888";

TonigyPipe="\PIPE\TONIGY\CONTROL"

call RxFuncAdd "SysLoadFuncs", "REXXUTIL", "SysLoadFuncs"
call SysLoadFuncs

if Stream(TonigyPipe, "c", "open")\="READY:" then
do
    say "Tonigy is not running."
    return
end

drives=QueryDrivesNumber()

do i=0 to drives-1
    if QueryStatus(i)==1 then
    do
        call RenewLongNames QueryCDROMDrive(i), QueryTonigyDrive(i)
    end
end

return

RenewLongNames:
    parse arg cdromdrive, tonigydrive
    say "Found "tonigydrive "drive (the shadow of "cdromdrive")"
    "@querycddb.exe "cdromdrive" tonigy defgroup.com "CDDBServer" "CDDBPort" | rxqueue"
    say
    do queued()
        parse pull a
        parse var a "Track"number": "title
        if number\='' then
        do
            say "Found a title for the "number" track: "title
            call AddLongName tonigydrive||"\track"number".wav", title
        end
    end
return

QueryDrivesNumber:
    call CharOut TonigyPipe, "QueryDrivesNumber"
    parse value LineIn(TonigyPipe) with code drives
    if code=0 then
        return drives
return 0
    
QueryStatus:
    parse arg drive
    call CharOut TonigyPipe, "QueryStatus "drive
    parse value LineIn(TonigyPipe) with code status
    if code=0 then
        return status
return 0
    
QueryCDROMDrive:
    parse arg drive
    call CharOut TonigyPipe, "QueryCDROMLetter "drive
    parse value LineIn(TonigyPipe) with code letter
    if code=0 then
        return letter
return ""

QueryTonigyDrive:
    parse arg drive
    call CharOut TonigyPipe, "QueryTonigyLetter "drive
    parse value LineIn(TonigyPipe) with code letter
    if code=0 then
        return letter
return ""


AddLongName: procedure
    parse arg filename, longname
    data="fdff"x||reverse(d2c(length(longname), 2))||longname
    call SysPutEA filename, ".LONGNAME", data
return
