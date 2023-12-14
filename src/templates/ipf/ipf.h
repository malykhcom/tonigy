#define width 78

#definerexx format
tmp=''
i=1
l=0
do forever
  w=word(in, i)
  if w='' then leave
  len=length(w)
  if l=0 then
  do
    tmp=tmp||w
    l=len
  end
  else
    if l+len><$width>
    then
    do
      tmp=tmp||'<?newline>'||w
      l=len
    end
    else
    do
      tmp=tmp' 'w
      l=l+1+len
    end
  i=i+1
end
#definerexx

#define p \
  #rexxvar in = '{$#1}' \ 
  #evaluate '' <$format> \
  :p.<??tmp>

#define start :userdoc.%\
:title.{$Title}%\

#define end :euserdoc.
#define h1 :h1.
#define h2 :h2.
#define bold :hp2.{$#1}:ehp2.
#define colored :hp4.{$#1}:ehp4.
;#define p :p.

#define startlist :ul compact.
#define list :li.
#define endlist :eul.

#define startolist :ol compact.
#define olist :li.
#define endolist :eol.

#define startdict :dl compact.
#define dictt :dt.
#define dictd :dd.
#define enddict :edl.

#define footnote :fn id={$#1}:efn.

#define INF :dt.:link reftype=fn refid=INF.INF:elink.&colon. \
<?newline>:dd.
#define NEW :dt.:link reftype=fn refid=NEW.NEW:elink.&colon. \
<?newline>:dd.
#define CHG :dt.:link reftype=fn refid=CHG.CHG:elink.&colon. \
<?newline>:dd.

#define FIX :dt.:link reftype=fn refid=FIX.FIX:elink.&colon. \
<?newline>:dd.
