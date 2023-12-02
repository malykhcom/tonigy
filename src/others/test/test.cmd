/* */

config="d:\config.sys"
tonigy="TONIGY.IFS"
ret="c:\tonigy"
if Stream(config, "c", "open read")="READY:" then
do
    do while lines(config)
        line=LineIn(config)
        parse var line t1"="t2
        t1=Strip(t1)
        t2=Strip(Strip(t2),, '"');
        lp=Lastpos("\", t2)
        if Translate(t1)="IFS" & Translate(Substr(t2, lp+1))=tonigy then
            ret=Left(t2, lp-1)
    end
    call Stream config, "c", "close"
end

say ret
