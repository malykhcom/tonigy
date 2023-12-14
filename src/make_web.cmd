@echo off
copy others\.htaccess _web
cd templates\html
call make
cd ..\..
dc DK generated\html\register_russia.html 
copy generated\html\*.html _web
copy _install\tonigy* _web