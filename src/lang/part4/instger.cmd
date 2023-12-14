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
        say 'Soll die folgende neue Zeile "'newline'"'
        call CharOut, "in die "config" eingefÅgt werden (Original wird als "backup" gesichert)? (J/N) "
        if Input()=="J" then
        do
            call SysFileDelete sysdisk"\"backup
            "@ren "config" "backup
            say "  Backup wurde erstellt."
            backup=sysdisk"\"backup
            if Stream(backup, "c", "open read")="READY:" then
                if Stream(config, "c", "open write")="READY:" then
                do
                    do forever
                        line=LineIn(backup)
                        if line="" & Stream(backup, "s")\="READY" then
                            leave
                        if Translate(Left(Space(line, 0), 4))="IFS=" & Translate(Right(line, Length(tonigy)))=tonigy then
                            say '  Alte Zeile "'line'" gelîscht.'
                        else
                            call LineOut config, line
                    end
                    call LineOut config, newline
                    say '  Neue Zeile "'newline'" hinzugefÅgt.'
                end
        end
    end
call CharOut, "Soll ein Tonigy WPS Ordner erstellt werden? (J/N) "
if Input()="J" then
do
    id="<TONIGY11FOLDER>"
    ret=SysCreateObject("WPFolder", "Tonigy v1.1", "<WP_DESKTOP>", "OBJECTID="id)
    if ret then
        say "  Ordner wurde erstellt."
    ret=SysCreateObject("WPProgram", "Tonigy control", id, "EXENAME="directory()"\TONIGY.EXE")
    if ret then
        say "  Programm Ojekt wurde erstellt."
    ret=SysCreateObject("WPProgram", "Register Tonigy", id, "EXENAME="directory()"\REGISTER.EXE")
    if ret then
        say "  Register Objekt wurde erstellt."
    ret=SysCreateObject("WPShadow", "README.TXT", id, "SHADOWID="directory()"\README.TXT")
    if ret then
        say "  README.TXT Objekt wurde erstellt."
    ret=SysCreateObject("WPShadow", "LIESMICH.TXT", id, "SHADOWID="directory()"\LIESMICH.TXT")
    if ret then
        say "  LIESMICH.TXT Objekt wurde erstellt."
    ret=SysCreateObject("WPProgram", "Tonigy documentation (English)", id, "EXENAME=view.exe;PARAMETERS="directory()"\ENGLISH.INF")
    if ret then
        say "  Tonigy Documentation (English) Objekt wurde erstellt."
    ret=SysCreateObject("WPProgram", "Tonigy Dokumentation (Deutsch)", id, "EXENAME=view.exe;PARAMETERS="directory()"\GERMAN.INF")
    if ret then
        say "  Tonigy Dokumentation (Deutsch) Objekt wurde erstellt."
end
say "Fertig."

return

Input:
    do forever
        key=SysGetKey("NOECHO")
        select
            when c2d(key)=224 | c2d(key)=0 then nop
            when key="j" | key="J" then
            do
                say "Ja"
                return "J"
            end
            when key="n" | key="N" then
            do
                say "Nein"
                return "N"
            end
            otherwise Beep(1000, 10)
        end
    end
