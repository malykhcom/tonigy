#define copy &copy;

#define startborder <table bgcolor=#000000 width=100% cellspacing=0 cellpadding=0 border=0>%\
<tr><td><table width=100% cellspacing=1 cellpadding={$#1} border=0>

#define endborder </table></td></tr></table>

#define emptyline <table border=0 cellspacing=0 cellpadding=0><tr><td height={$#1}><table border=0 cellspacing=0 cellpadding=0><tr><td></td></tr></table></td></tr></table>

#define start <html>%\
<head>%\
<title>Tonigy - {$Title}</title>%\
<meta name="description" content="{$Desc}">%\
<meta name="keywords" content="{$Keywords}">%\
<style type="text/css"><!--%\
a { text-decoration: none; }%\
a:hover { text-decoration: none; color: #ff0000; }%\
a:active { text-decoration: none; color: #000000; }%\
td { font-size: 10pt; }%\
font { font-size: 8pt; }%\
--> </style>%\
</head>%\
<body bgcolor=#408080 text=#000000 link=#0000ff vlink=#0000ff alink=#000000 \
topmargin=4 leftmargin=4 rightmargin=4 bottommargin=4 \
marginwidth=4 marginheight=4>%\
%\
<$startborder '0'><tr><td bgcolor=#e6e6e6><table width=100% cellspacing=0 cellpadding=2 border=0>%\
<tr>%\
<td>DEF Group's projects:</td>%\
<td bgcolor=#b0b0b0>Tonigy</td>%\
<td>JavaC Booster</td>%\
</tr></table></td></tr><$endborder>%\
%\
<$emptyline '2'>%\
%\
<$startborder '0'><tr><td bgcolor=#e6e6e6 align=center><img src=images/tonigy.gif alt="Tonigy - Multimedia CD System/2" width=455 height=55></td></tr><$endborder>%\
%\
<$emptyline '2'>%\
%\
<$startborder '0'><tr><td bgcolor=#e6e6e6><table width=100% cellspacing=0 cellpadding=2 border=0>%\
<tr align=center>%\
<td width=13% \
#if ['{$section}' = '1' ] \
bgcolor=#b0b0b0 \
#endif \
><a href=index.html>About</a></td>%\
<td width=16% \
#if ['{$section}' = '2' ] \
bgcolor=#b0b0b0 \
#endif \
><a href=faq.html>FAQ and Tips</a></td>%\
<td width=13% \
#if ['{$section}' = '3' ] \
bgcolor=#b0b0b0 \
#endif \
><a href=history.html>History</a></td>%\
<td width=16% \
#if ['{$section}' = '4' ] \
bgcolor=#b0b0b0 \
#endif \
><a href=screens.html>Screenshots</a></td>%\
<td width=13% \
#if ['{$section}' = '5' ] \
bgcolor=#b0b0b0 \
#endif \
><a href=download.html>Download</a></td>%\
<td width=16% \
#if ['{$section}' = '6' ] \
bgcolor=#b0b0b0 \
#endif \
><a href=register.html>Register Now!</a></td>%\
<td width=13% \
#if ['{$section}' = '7' ] \
bgcolor=#b0b0b0 \
#endif \
><a href=links.html>Links</a></td>%\
</tr></table></td></tr><$endborder>%\
%\
<$emptyline '8'>%\
%\
<$startborder '3'><tr><td bgcolor=#e6e6e6 align=center><b>{$Title}</b></td></tr><$endborder>%\
%\
<$emptyline '2'>%\
%\
<$startborder '4'>%\
<tr>%\
<td valign=top bgcolor=#e6e6e6>%\
<font size=-1>%\
<br>%\
<center><b><a href=history.html#Tonigy>Last version</a></b><br><br>%\
2001 Aug 27 (1.4)<br><br>%\
<b><a href=history.html>Last site update</a></b><br><br>%\
2001 Sep 19%\
</center><br>%\
<p align=center><b>Do you need additional information?</b></p>%\
<p>E-mail us: <a href=mailto:tonigy@defgroup.com>tonigy@defgroup.com</a></p>%\
<p>Read overviews of Tonigy:</p>%\
<p>1. <a href="http://www.geocities.co.jp/SiliconValley-Bay/4081/cdmp3.html#tonigy">Tonigy IFS</a> [Japanese]</p>%\
<p>2. <a href="http://www.goldencode.com/atlos2/notes/tonigy/tonigy.html">Tonigy - A Virtual CD Installable File System</a> [English]</p>%\
<p>3. <a href="http://www.os2ezine.com/20010816/page_4.html">Making and Playing mp3 files with OS/2</a> [English]</p>%\
<p>Tonigy-related software:</p>%\
<p>1. <a href="http://hobbes.nmsu.edu/cgi-bin/h-search?key=leechmp3">Leech MP3</a>.</p>%\
<p>2. <a href="http://www.xmission.com/~scorey/">Tonigy Utility</a>.</p>%\
<p>&nbsp;</p>%\
<p>&nbsp;</p>%\
<p>&nbsp;</p>%\
</font>%\
<table width=160 border=0 cellspacing=0 cellpadding=0><tr><td></td></tr></table>%\
</td>%\
<td width=100% valign=top bgcolor=#ffffff>%\
%\
<!-- begin of main frame -->%\
<?newline>

#define end <?newline><!-- end of main frame -->%\
<p>&nbsp;</p>%\
</td>%\
</tr><$endborder>%\
<$emptyline '2'>%\
<$startborder '4'>%\
<tr><td align=center bgcolor=#e6e6e6>%\
<font size=-1>&copy; Copyright <a href="http://www.defgroup.com">DEF Group</a>, 2001.</font>%\
</td></tr><$endborder>%\
%\
</body>%\
</html>

#define bold <b>{$#1}</b>
#define center <p align=center>{$#1}</p>
#define p <p align=justify>{$#1}</p>

#define startlist <ul type=disc compact>
#define list <li>{$#1}</li>
#define endlist </ul>

#define NEW <b>NEW</b>:<?space>
#define INF <b>INF</b>:<?space>
#define CHG <b>CHG</b>:<?space>
#define FIX <b>FIX</b>:<?space>

#define img <a href=images/{$Name}.gif><img src=images/{$Name}_s.gif width={$W} height={$H} border=1 alt="{$Size} bytes"></a>
