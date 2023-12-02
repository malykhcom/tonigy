#define copy (c)

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
  <??tmp>

#define title \
  #rexxvar in = '{$#1}' \
  #evaluate '' out=copies('=', length(in)) \
  <??in><?NewLine><??out>

#define center \
  #rexxvar in = '{$#1}' \
  #evaluate '' out=strip(center(in, <$width>), 't') \
  <??out>

#define start <$center '<$TonigyDesc>'><?newline> -\
  <$center '<$TonigyCopy>'><?newline> -\
  <?newline>

#define end <?newline>E-Mail:    <$Email><?newline>Home page: <$Homepage>

#define bold {$#1}

#define NEW NEW:<?space>
#define INF INF:<?space>
#define CHG CHG:<?space>
#define FIX FIX:<?space>
