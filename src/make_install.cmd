/* */

outfile=directory()"\_install\tonigy17.wpi"
inpath=directory()"\_ready"
sfile=directory()"\others\tonigy.wis"
path=directory()

call directory "e:\soft\warpin"
"del "outfile
/*"wic "outfile" -s "sfile
"wic "outfile" -a 1 -r -c"inpath" *"*/
"wic "outfile" -s "sfile" -a 1 -r -c"inpath" *"

call directory path"\_ready"
"del ..\_install\*.zip"
"zip -r ..\_install\tonigy17.zip *"

call directory path

