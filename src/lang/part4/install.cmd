/* */

call RxFuncAdd "SysLoadFuncs", "REXXUTIL", "SysLoadFuncs"
call SysLoadFuncs

config="\CONFIG.SYS"
backup="CONFIG.BAK"
tonigy="TONIGY.IFS"
newline="IFS="directory()"\"tonigy

say "Tonigy Installer v1.1"
say ""
sysdisk=FileSpec("drive", SysSearchPath("PATH", "OS2"))
if sysdisk\="" then
    if Stream(sysdisk||config, "c", "query exists")\="" then
    do
        config=sysdisk||config
        say 'Do you want to add the line "'newline'"'
        call CharOut, "into "config" (an original config will be saved as "backup")? (Y/N) "
        if Input()=="Y" then
        do
            call SysFileDelete sysdisk"\"backup
            "@ren "config" "backup
            say "  Backup was created."
            backup=sysdisk"\"backup
            if Stream(backup, "c", "open read")="READY:" then
                if Stream(config, "c", "open write")="READY:" then
                do
                    do forever
                        line=LineIn(backup)
                        if line="" & Stream(backup, "s")\="READY" then
                            leave
                        if Translate(Left(Space(line, 0), 4))="IFS=" & Translate(Right(line, Length(tonigy)))=tonigy then
                            say '  Old line "'line'" was deleted.'
                        else
                            call LineOut config, line
                    end
                    call LineOut config, newline
                    say '  New line "'newline'" was added.'
                end
        end
    end
call CharOut, "Do you want to create Tonigy WPS folder? (Y/N) "
if Input()="Y" then
do
    id="<TONIGY11FOLDER>"
    ret=SysCreateObject("WPFolder", "Tonigy v1.1", "<WP_DESKTOP>", "OBJECTID="id)
    if ret then
        say "  Folder was created."
    ret=SysCreateObject("WPProgram", "Tonigy control", id, "EXENAME="directory()"\TONIGY.EXE")
    if ret then
        say "  Program object was created."
    ret=SysCreateObject("WPProgram", "Register Tonigy", id, "EXENAME="directory()"\REGISTER.EXE")
    if ret then
        say "  Register object was created."
    ret=SysCreateObject("WPShadow", "README.TXT", id, "SHADOWID="directory()"\README.TXT")
    if ret then
        say "  README.TXT object was created."
    ret=SysCreateObject("WPShadow", "LIESMICH.TXT", id, "SHADOWID="directory()"\LIESMICH.TXT")
    if ret then
        say "  LIESMICH.TXT object was created."
    ret=SysCreateObject("WPProgram", "Tonigy documentation (English)", id, "EXENAME=view.exe;PARAMETERS="directory()"\ENGLISH.INF")
    if ret then
        say "  Tonigy Documentation (English) object was created."
    ret=SysCreateObject("WPProgram", "Tonigy Dokumentation (Deutsch)", id, "EXENAME=view.exe;PARAMETERS="directory()"\GERMAN.INF")
    if ret then
        say "  Tonigy Dokumentation (Deutsch) object was created."
end
say "Done."

return

Input:
    do forever
        key=SysGetKey("NOECHO")
        select
            when c2d(key)=224 | c2d(key)=0 then nop
            when key="y" | key="Y" then
            do
                say "Yes"
                return "Y"
            end
            when key="n" | key="N" then
            do
                say "No"
                return "N"
            end
            otherwise Beep(1000, 10)
        end
    end
