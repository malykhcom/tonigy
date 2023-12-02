@echo off

cd _ready
md DOC
md LOCALE

cd ..\others
copy file_id.diz ..\_ready
copy install.cmd ..\_ready
midtime ..\_ready\file_id.diz
midtime ..\_ready\install.cmd

cd ..\LOCALE
copy messages.en ..\_ready\LOCALE
copy messages.de ..\_ready\LOCALE
copy messages.ru ..\_ready\LOCALE
midtime ..\_ready\LOCALE\messages.en
midtime ..\_ready\LOCALE\messages.de
midtime ..\_ready\LOCALE\messages.ru

cd ..\templates\text
call make
cd ..\..\generated\text
copy readme.txt ..\..\_ready
copy history.txt ..\..\_ready\DOC
copy license.txt ..\..\_ready\DOC
copy register.txt ..\..\_ready\DOC
midtime ..\..\_ready\readme.txt
midtime ..\..\_ready\DOC\history.txt
midtime ..\..\_ready\DOC\license.txt
midtime ..\..\_ready\DOC\register.txt
cd ..

cd ..\templates\ipf
call make
cd ..

cd ..\agent
rem del *.obj
del tonigy.exe
wmake
copy tonigy.exe ..\_ready
copy tonigy.inf ..\_ready
midtime ..\_ready\tonigy.exe
midtime ..\_ready\tonigy.inf

cd ..\register
del register.exe
call make
copy register.exe ..\_ready
midtime ..\_ready\register.exe

cd ..\ifs
del tonigy.ifs
call make
copy tonigy.ifs ..\_ready
midtime ..\_ready\tonigy.ifs

cd ..\_ready
midtime DOC
midtime LOCALE

