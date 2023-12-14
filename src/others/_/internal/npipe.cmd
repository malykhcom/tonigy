/* */

call RxFuncAdd "SysLoadFuncs", "REXXUTIL", "SysLoadFuncs"
call SysLoadFuncs

TonigyPipe="\PIPE\TONIGY\CONTROL"

if Stream(TonigyPipe, "c", "open")\="READY:" then
do
    say "Tonigy is not running."
    return
end

Drives=QueryDrivesNumber()

say "Drives: "Drives
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

PrintStatus:
    do i=0 to Drives-1
        if QueryStatus(i)==1 then
            Status="attached"
        else
            Status="detached"
        say "Drive "i": "QueryCDROMDrive(i)"->"QueryTonigyDrive(i)" ("Status")"
    end
return

QueryDrivesNumber:
    call CharOut TonigyPipe, "QueryDrivesNumber"
    parse value LineIn(TonigyPipe) with code drives
    if code=0 then
        return drives
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

QueryStatus:
    parse arg drive
    call CharOut TonigyPipe, "QueryStatus "drive
    parse value LineIn(TonigyPipe) with code status
    if code=0 then
        return status
return 0

Attach:
    parse arg drive
    call CharOut TonigyPipe, "Attach "drive
return LineIn(TonigyPipe)
    
Detach:
    parse arg drive
    call CharOut TonigyPipe, "Detach "drive
return LineIn(TonigyPipe)
