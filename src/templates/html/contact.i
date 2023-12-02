#include "html.h"
#include "..\global\global.h"
#include "..\global\generale.h"

<$start title='Contact' \
desc='Tonigy. Contact information' \
keywords='Tonigy' \
section='1'> 

<$center '<b>How to contact us</b>'>

<p>Please, send questions, bug reports and suggestions to 
<a href=mailto:tonigy@defgroup.com>tonigy@defgroup.com</a>.</p>

<p>Or use the form:</p>

<form action="/cgi-bin/contact.pl" method="post">
<table align=center border=0>
<tr>
<td><b>Your message:</b><br><textarea cols=50 name=message rows=10 wrap=hard></textarea></td>
</tr>
<tr>
<td>

<table width=100% border=0>
<tr>
<td><b>Your name:</b></td>
<td><input maxlength=40 name=name size=30></td>
</tr>
<tr>
<td>Your E-Mail:</b></td>
<td><input maxlength=40 name=email size=30></td>
</tr>
</table>

</td>
</tr>
<tr>
<td align=right><input type=submit value="Send us message"></td>
</tr>
</table>
</form>


<$end>
