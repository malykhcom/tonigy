/* */

call RxFuncAdd "SysLoadFuncs", "REXXUTIL", "SysLoadFuncs"
call SysLoadFuncs

config="\CONFIG.SYS"
backup="CONFIG.BAK"
tonigy="TONIGY.IFS"
if pos(" ", directory())=0 then
    newline="IFS="directory()"\"tonigy
else
    newline='IFS="'directory()"\"tonigy'"'

say "Tonigy Installer v1.7"
say ""
call CharOut, "Select language: (E)nglish or (G)erman: "
Lang=Language()
say ""
sysdisk=FileSpec("drive", SysSearchPath("PATH", "OS2"))
if sysdisk\="" then
    if Stream(sysdisk||config, "c", "query exists")\="" then
    do
        config=sysdisk||config
        if Lang="E" then
        do
            say 'Do you want to add the line "'newline'"'
            call CharOut, "into "config" (the original config will be saved as "backup")? (Y/N) "
        end
        else
        do
            say 'Soll die folgende neue Zeile "'newline'"'
            call CharOut, "in die "config" eingefÅgt werden (Original wird als "backup" gesichert)? (J/N) "
        end
        if Input()=="Y" then
        do
            call SysFileDelete sysdisk"\"backup
            "@ren "config" "backup
            if Lang="E" then
                say "  Backup was created."
            else
                say "  Backup wurde erstellt."
            backup=sysdisk"\"backup
            if Stream(backup, "c", "open read")="READY:" then
                if Stream(config, "c", "open write")="READY:" then
                do
                    do while lines(backup)
                        line=LineIn(backup)
                        parse var line t1"="t2
                        t1=Strip(t1)
                        t2=Strip(Strip(t2),, '"');
                        lp=Lastpos("\", t2)
                        if Translate(t1)="IFS" & Translate(Substr(t2, lp+1))=tonigy then
                            if Lang="E" then
                                say '  Old line "'line'" was deleted.'
                            else
                                say '  Alte Zeile "'line'" gelîscht.'
                        else
                            call LineOut config, line
                    end
                    call LineOut config, newline
                    if Lang="E" then
                        say '  New line "'newline'" was added.'
                    else
                        say '  Neue Zeile "'newline'" hinzugefÅgt.'
                end
        end
    end
if Lang="E" then
    call CharOut, "Do you want to create Tonigy WPS folder? (Y/N) "
else
    call CharOut, "Soll ein Tonigy WPS Ordner erstellt werden? (J/N) "
if Input()="Y" then
do
    id="<TONIGY17FOLDER>"
    ret=SysCreateObject("WPFolder", "Tonigy v1.7", "<WP_DESKTOP>", "OBJECTID="id)
    if ret then
        if Lang="E" then
            say "  Folder was created."
        else
            say "  Ordner wurde erstellt."
    call CreateProgram "Tonigy", directory()"\TONIGY.EXE"
    call CreateProgram "Register Tonigy", directory()"\REGISTER.EXE"
    call CreateShadow  "README.TXT", directory()"\README.TXT"
    call CreateProgram "Tonigy documentation", "view.exe;PARAMETERS="directory()"\TONIGY.INF"
    call CreateURL     "Tonigy homepage", "http://www.tonigy.com"
end

if Lang="E" then
do
    call SysIni "TONIGY.INI", "Tonigy", "Language", reverse(d2c(0, 4))
    say "Done."
end
else
do
    call SysIni "TONIGY.INI", "Tonigy", "Language", reverse(d2c(1, 4))
    say "Fertig."
end

return

Language:
    do forever
        key=SysGetKey("NOECHO")
        select
            when c2d(key)=224 | c2d(key)=0 then nop
            when key="e" | key="E" then
            do
                say "English"
                return "E"
            end
            when key="g" | key="G" then
            do
                say "German"
                return "G"
            end
            otherwise Beep(1000, 10)
        end
    end

Input:
    do forever
        key=SysGetKey("NOECHO")
        select
            when c2d(key)=224 | c2d(key)=0 then nop
            when key="y" | key="Y" | key="j" | key="J" then
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

CreateProgram:
    parse arg desc, exename
    ret=SysCreateObject("WPProgram", desc, id, "EXENAME="exename)
    if ret then
        if Lang="E" then
            say "  "desc" object was created."
        else
            say "  "desc" Objekt wurde erstellt."
return

CreateShadow:
    parse arg desc, shadow
    ret=SysCreateObject("WPShadow", desc, id, "SHADOWID="shadow)
    if ret then
        if Lang="E" then
            say "  "desc" object was created."
        else
            say "  "desc" Objekt wurde erstellt."
return
    
CreateURL:
    parse arg desc, url
    ret=SysCreateObject("WPUrl", desc, id, "URL="url)
    if ret then
        if Lang="E" then
            say "  "desc" object was created."
        else
            say "  "desc" Objekt wurde erstellt."
return
