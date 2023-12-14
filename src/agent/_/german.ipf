:userdoc.
:title.Tonigy Dokumentation
:h1.
öber Tonigy
.br
:hp2.
Tonigy v1.2. Audio/Video CD IFS fÅr OS/2. Der pfiffige Ersatz fÅr Grabber.
:ehp2.
:p.
Tonigy ist ein installierbares Dateisystem fÅr CDs (CD IFS) das Tracks einer
Audio-CD in WAV oder RAW-Dateien und Tracks einer Video-CD in MPG-Dateien
abbildet. Benutze es um auf Audio- und Video-Tracks wie auf normale Dateien
zuzugreifen.
:p.
Hauptmerkmale&colon.
:ul compact.
:li.IFS (installierbares Dateisystem) fÅr OS/2
:li.Zeigt Audio-Tracks als WAV- oder RAW-Dateien
:li.Zeigt Video-Tracks als MPG-Dateien
:li.Einfache Bedienung und Einrichtung Åber PM-GUI OberflÑche
:li.Englisch/Deutsche SprachfÅhrung und Dokumentation
:li.UnterstÅtzt Erweiterte Attribute (lesend/schreibend)
:li.UnterstÅtzt angepasstes Anti-Jitter beim Lesen von Audiosektoren
:li.Option &odq.Hîchste PrioritÑt&cdq. fÅr langsame CD-Roms und PCs
:li.Warnungen und Fehler werden mitgeschrieben (Log-Funktion)
:li.UnterstÅtzt Kontrolle durch Named Pipes
:eul.
:h1.
Hilfe fÅr Hilfe
:p.
Druck auf F1 ruft die Online-Hilfe fÅr Tonigy auf.
:h2.
Schnellstart
:ol compact.
:li.Starte das Tonigy-Programm aus dem WPS-Ordner.
:li.WÑhle das CD-ROM aus, dass Tonigy ansprechen soll.
:li.Klicke auf "Setup" oder drÅcke die Tastenkombination Alt+S.
:li.WÑhle irgendeinen freien, aber NICHT BUSY markierten Laufwerksbuchstaben
   aus den vorhandenen aus, der zum Tonigy-Laufwerk wird [ich habe einfach T&colon.
gewÑhlt - KK].
:li.Klicke den OK-Button an oder drÅcke die ENTER-Taste um den Setup-Dialog zu
   schlie·en.
:li.Klicke auf "Attach" oder drÅcke Alt+A, um das Tonigy-Laufwerk mit dem
    CD-ROM zu verbinden.
:li.Arbeite mit dem neuen Laufwerk.
:li.Wenn Du alles erledigt hast, schlie·e das Programm. Die Verbindung
    zwischen CD-ROM und Tonigy-Laufwerk wird wieder aufgehoben.
:eol.
:p.
Wenn Du die verwendete Sprache Ñndern willst, rufe das KontextmenÅ
auf (rechte Maustaste Åber dem Hauptfenster klicken), Ñndere die
Sprache (Language), beende und starte Tonigy neu.
:h2 res=1.
Hauptfenster
:p.
Folgende Tasten kînnen verwendet werden, um das Hauptfenster von Tonigy zu
steuern&colon.
:dl compact tsize=20.
:dt.Alt+A
:dd.Laufwerk ins System einbinden
:dt.Alt+D
:dd.Laufwerk aus dem System nehmen
:dt.Alt+S
:dd.:link reftype=hd res=2.Laufwerkseinstellungen:elink.
:dt.Alt+X oder Alt+F4
:dd.Programm beenden
:edl.
:p.
Verwende &odq.:link reftype=hd res=3.Allgemeine Einstellungen:elink.&cdq.
im Pop-Up-MenÅ (rechte Maustaste im Hauptfenster von Tongy) um globale
Einstellungen zu verÑndern.
:p.
Wenn &odq.Auto-Verbindung&cdq. gewÑhlt ist, wird das Laufwerk bei jedem Start
von Tonigy automatisch verbunden.
:warning.Alle Tonigy-Laufwerke werden automatisch getrennt, wenn Tonigy
geschlossen wird.
:ewarning.
:h2 res=2.
LW-Einstellungs-Fenster
:p.
Wenn &odq.Automatisch verbinden&cdq. gewÑhlt ist, wird das Laufwerk bei jedem
Start von Tonigy automatisch verbunden.
:p.
Alle Aktionen von Tonigy kînnen in ein virtuelles
:link reftype=hd refid=log.TONIGY.LOG:elink. File auf dem
Tonigy Laufwerk geschrieben werden, wenn die 
&odq.Log-Warnungen/-Fehler&cdq. Option eingestellt ist.
:p.
Einige CD-ROMs haben Probleme mit der Sysnchronisation beim Lesen 
von Audiosektoren (sog. Jitter). Um dieses zu korrigieren, kann die 
&odq.:link reftype=hd refid=aj.Anti-Jitter:elink.&cdq. FÑhigkeit von
Tonigy genutzt werden. Um das Vorhandensein von Jitter festzustellen, arbeite
mit irgendeiner RAW- oder WAV-Datei eines Tonigy-Laufwerkes (kopiere die Datei,
spiele sie ab oder betrachte sie in einem HEX-Editor; der Status der 
&odq.Anti-Jitter&cdq. Optionen ist dabei unerheblich). Dann suche in der Datei CDINFO.TXT auf
dem Tonigy-Laufwerk nach folgender Zeile
:xmp.
x Audioblock(s)
:exmp.
:p.
Wenn dieser Text lautet
:xmp.
Dieses CD-ROM hat keine Probleme mit der Lesesynchronisation.
:exmp.
:p.
kann &odq.Anti-Jitter&cdq.ausgeschaltet werden.
:note.&odq.Anti-Jitter&cdq. reduziert die Lesegeschwindigkeit, daher sollte es
ausgeschaltet werden, wenn das CD-ROM keinen Jitter produziert und Du hîhere
Lesegeschwindigkeiten willst.
:p.
Wenn die &odq.Korrekturversuche&cdq. Option geschaltet ist, dann versucht Tonigy, mîgliche
Synchonisationsprobleme des Laufwerkes beim Lesen zu korrigieren. Wenn die
Probleme nach der gewÑhlten Anzahl Versuche nicht behoben werden kînnen, werden
die tatsÑchlich gelesenen Daten an das Programm geliefert, das das Tonigy-LW
gerade verwendet (vergl. dazu das :link reftype=hd refid=log.TONIGY.LOG:elink.).
Wenn Du ein langsames CD-ROM verwendest und .WAV oder .RAW-Dateien abhîrst,
sollte die Anzahl von Versuchen
zwischen 2 und 5 liegen. Wenn diese Daten encodiert (kopiert) werden oder das
CD-ROM schnell genug ist, kînnen auch hîhere Werte gesetzt werden. Ein nur
einmaliger Versuch ist in jedem Falle wenig sinnvoll.
:h2 res=3.
Fenster Allgemeine Einstellungen
:p.
&odq.Hîchste PrioritÑt&cdq. Ñndert die AusfÅhrungsprioritÑt des
Threads, der die Sektoren ausliest. Wenn diese Option geschaltet ist, wird der
Thread auf 'time-critical' und darin Level 31 gesetzt (dies ist die hîchste
Einstellung, die OS/2 fÅr einen Teilproze· zulÑsst und garantiert ihm den
hîchsten Anteil an Rechenzeit unter allen laufenden Prozesssen und Threads).
Ansonsten hat der Lese-Thread die normale PrioritÑts-Klasse und darin den Level
31. Stockende Wiedergabe von WAV- oder RAW-Dateien bei hoher FestlattenaktivitÑt
kann durch Einschalten von &odq.Hîchste PrioritÑt&cdq. verbessert werden.
:p.
&odq.Sprache&cdq. Radio-Schalter setzen die Sprache, in der Tonigy sich
prÑsentiert. Das Programm mu· nach einer énderung neu gestartet werden, um
diese wirksam werden zu lassen.
:h2 res=4.
Fenster Verbindung fehlgeschlagen
:p.
Dieses Fenster wird gezeigt, wenn Tonigy nicht in der Lage war, ein virtuelles
Laufwerk zu erstellen bzw. die Verbindung dazu herzustellen. Die hÑufigste
Ursache fÅr diesen Fehler ist der Versuch, ein Laufwerk anzusprechen, das
bereits in Benutzung durch ein anderes Programm oder einen anderen Prozess ist.
WÑhle einen anderen LW-Buchstaben und versuche, Tonigy erneut zu verbinden.
:h1.
Information fÅr fortgeschrittene Nutzer
:p.
Dieser Abschnitt enthÑlt Informationen fÅr Fortgeschrittene.
:h2 id=aj.
Anti-Jitter
:p.
Die Idee bei der Funktion von Anti-Jitter ist es, Sektoren mit einem
gewissen 'öberhang' zu lesen. Viele Grab-Programme nutzen diesen Gedanken,
um Jitter zu korrigieren, aber Tonigy fÅhrt neue Methoden ein um die Jitter
Korrektur eleganter und verlÑsslicher zu machen. Erstens kann das Ma· der
öberlagerung von Tonigy dynamisch verÑndert werden, wobei statistische Daten
von vorherigen Leseversuchen herangezogen werden. Zweitens achtet Tonigys
Anti-Jitter auf ungleichmÑ·ige Ergebnisse des Sektoren-Auslesens.
:h2.
Erweiterte Attribute
:p.
Das Tonigy-IFS unterstÅtzt das Lesen und Schreiben von Erweiterten Attributen.
:p.
Erweiterte Attribute von *.WAV, *.RAW und *.MPG-Dateien sind abhÑngig von der CD.
:p.
Tonigy speichert die EAs in einer EA.INI-Datei im Tonigy-Inhaltsverzeichnis.
Diese Datei ist eine binÑre Standard-OS/2 INI-Datei (wie System- oder User.INI)
:h2.
Named pipe
:p.
Tonigy kann durch die \PIPE\TONIGY\CONTROL Named Pipe gesteuert werden. Es
handelt sich hierbei um eine Nachrichten-Pipe, ein Kommando sollte dieser Pipe
als 1 Nachricht Åbergeben werden. Ein Kommando besteht aus einem Wort (keine
öberprÅfung auf Gro·/Kleinschreibung), dem Namen des Kommandos und in einigen
FÑllen einem oder mehr Parametern, getrennt durch ein Leerzeichen. Beispiel&colon.
:xmp.
QueryDrivesNumber
attach 1
:exmp.
:p.
Nach dem Schreiben des Kommandos in die Named Pipe mu· das aufrufende Programm
eine RÅckgabezeichenkette (return string) Åbernehmen. Diese beginnt mit einem
dezimalen return code. Wenn das Kommando Daten zurÅckgibt und der return code
gleich 0 ist (kein Fehler), enthÑlt der Rest des return strings Daten.
Beispiel&colon.
:xmp.
0
31
0 E:
:exmp.
:h3.
QueryDrivesNumber
:p.:hp2.Anmerkungen:ehp2.:p.Das Kommando erfragt die Anzahl der CD-ROM Laufwerke.
:p.:hp2.Parameter:ehp2.:p.Keine
:p.:hp2.RÅckgabe:ehp2.:p.FehlerCode
.br
:hp1.oder:ehp1.
.br
0 AnzahlDerLaufwerke
:p.:hp2.Beispiel:ehp2.
:dl compact tsize=20.
:dt.Gesendet&colon.:dd.QueryDrivesNumber
:dt.Erhalten&colon.:dd.0 2
:dt.Kommentar&colon.:dd.Zwei CD-ROM Laufwerke vorhanden.
:edl.
:h3.
QueryCDROMDrive
:p.:hp2.Anmerkungen:ehp2.:p.Das Kommando ermittelt den LW-Buchstaben des CD-ROMs.
:p.:hp2.Parameter:ehp2.:p.NummerDesLaufwerks
.br
:hp1.die Nummer des Laufwerks im Bereich von 0 bis N-1 wobei N die Anzahl der
CD-ROM Laufwerke darstellt
:ehp1.
:p.:hp2.RÅckgabe:ehp2.:p.FehlerCode
.br
:hp1.oder:ehp1.
.br
0 BuchstabeDesCD-ROMs
:p.:hp2.Beispiel:ehp2.
:dl compact tsize=20.
:dt.Gesendet&colon.:dd.QueryCDROMDrive 1
:dt.Erhalten&colon.:dd.0 E&colon.
:dt.Kommentar&colon.:dd.Der Buchstabe des zweiten CD-ROMs ist F&colon..
:edl.
:h3.
QueryTonigyDrive
:p.:hp2.Anmerkungen:ehp2.:p.Das Kommando ermittelt den Buchstaben des Tonigy-Laufwerkes.
:p.:hp2.Parameter:ehp2.:p.NummerDesLaufwerks
.br
:hp1.die Nummer des Laufwerks im Bereich von 0 bis N-1 wobei N die Anzahl der
CD-ROM Laufwerke darstellt
:ehp1.
:p.:hp2.RÅckgabe:ehp2.:p.FehlerCode
.br
:hp1.oder:ehp1.
.br
0 BuchstabeDesTonigyLaufwerkes
.br
:hp1.falls kein Tonigy-Laufwerk ausgewÑhlt wurde, wird ein leerer String als
Tonigy-Laufwerk zurÅckgegeben
:ehp1.
:p.:hp2.Beispiel:ehp2.
:dl compact tsize=20.
:dt.Gesendet&colon.:dd.QueryTonigyDrive 1
:dt.Erhalten&colon.:dd.0 T&colon.
:dt.Kommentar&colon.:dd.Der Buchstabe des zweiten Tonigy-Laufwerkes ist T&colon..
:edl.
:h3.
QueryStatus
:p.:hp2.Anmerkungen:ehp2.:p.Dies Kommando erfragt den Status des Tonigy-Laufwerkes.
:p.:hp2.Parameter:ehp2.:p.NummerDesLaufwerks
.br
:hp1.die Nummer des Laufwerks im Bereich von 0 bis N-1 wobei N die Anzahl der
CD-ROM Laufwerke darstellt
:ehp1.
:p.:hp2.RÅckgabe:ehp2.:p.FehlerCode 
.br
:hp1.oder:ehp1.
.br
0 Status
.br
:hp1.0 als Status - das LW ist getrennt, 1 als Status - das LW ist verbunden.
:ehp1.
:p.:hp2.Beispiel:ehp2.
:dl compact tsize=20.
:dt.Gesendet&colon.:dd.QueryStatus 1
:dt.Erhalten&colon.:dd.0 0
:dt.Kommentar&colon.:dd.Das zweite Tonigy-Laufwerk ist getrennt.
:edl.
:h3.
Attach
:p.:hp2.Anmerkungen:ehp2.:p.Das Kommando verbindet das Tonigy-Laufwerk.
:p.:hp2.Parameter:ehp2.:p.NummerDesLaufwerks
.br
:hp1.die Nummer des Laufwerks im Bereich von 0 bis N-1 wobei N die Anzahl der
CD-ROM Laufwerke darstellt
:ehp1.
:p.:hp2.RÅckgabe:ehp2.:p.FehlerCode
:p.:hp2.Beispiel:ehp2.
:dl compact tsize=20.
:dt.Gesendet&colon.:dd.Attach 1
:dt.Erhalten&colon.:dd.0
:dt.Kommentar&colon.:dd.Das zweite Tonigy-Laufwerk ist verbunden.
:edl.
:h3.
Detach
:p.:hp2.Anmerkungen:ehp2.:p.Das Kommando trennt das Tonigy-Laufwerk.
:p.:hp2.Parameter:ehp2.:p.NummerDesLaufwerks
.br
:hp1.die Nummer des Laufwerks im Bereich von 0 bis N-1 wobei N die Anzahl der
CD-ROM Laufwerke darstellt
:ehp1.
:p.:hp2.RÅckgabe:ehp2.:p.FehlerCode
:p.:hp2.Beispiel:ehp2.
:dl compact tsize=20.
:dt.Gesendet&colon.:dd.Detach 1
:dt.Erhalten&colon.:dd.0
:dt.Kommentar&colon.:dd.Das Tonigy-Laufwerk ist getrennt worden.
:edl.
:h3.
Einsatzbeispiele (REXX)
:xmp.
/* */

call RxFuncAdd "SysLoadFuncs", "REXXUTIL", "SysLoadFuncs"
call SysLoadFuncs

TonigyPipe="\PIPE\TONIGY\CONTROL"

if Stream(TonigyPipe, "c", "open")\="READY&colon." then
do
    say "Tonigy lÑuft nicht."
    return
end

Drives=QueryDrivesNumber()

say "Laufwerke&colon. "Drives
say
call PrintStatus

say
say "Beliebige Taste um alle Tonigy-LWs zu trennen"
'@pause>nul'
say
do i=0 to Drives-1
    call Detach i
end
call PrintStatus

say
say "Beliebige Taste um alle Tonigy-LWs zu verbinden"
'@pause>nul'
say
do i=0 to Drives-1
    call Attach i
end
call PrintStatus

return

PrintStatus&colon.
    do i=0 to Drives-1
        if QueryStatus(i)==1 then
            Status="Verbunden"
        else
            Status="Getrennt"
        say "Laufwerk "i"&colon. "QueryCDROMDrive(i)"->"QueryTonigyDrive(i)" ("Status")"
    end
return

QueryDrivesNumber&colon.
    call CharOut TonigyPipe, "QueryDrivesNumber"
    parse value LineIn(TonigyPipe) with code drives
    if code=0 then
        return drives
return 0

QueryCDROMDrive&colon.
    parse arg drive
    call CharOut TonigyPipe, "QueryCDROMLetter "drive
    parse value LineIn(TonigyPipe) with code letter
    if code=0 then
        return letter
return ""

QueryTonigyDrive&colon.
    parse arg drive
    call CharOut TonigyPipe, "QueryTonigyLetter "drive
    parse value LineIn(TonigyPipe) with code letter
    if code=0 then
        return letter
return ""

QueryStatus&colon.
    parse arg drive
    call CharOut TonigyPipe, "QueryStatus "drive
    parse value LineIn(TonigyPipe) with code status
    if code=0 then
        return status
return 0

Attach&colon.
    parse arg drive
    call CharOut TonigyPipe, "Attach "drive
return LineIn(TonigyPipe)
    
Detach&colon.
    parse arg drive
    call CharOut TonigyPipe, "Detach "drive
return LineIn(TonigyPipe)
:exmp.
:h2 id=log.
TONIGY.LOG
:note.TONIGY.LOG ist lîschbar und kann jederzeit Åberschrieben werden.
:p.
TONIGY.LOG Meldungen&colon.
:xmp.
Laufwerk x ist verbunden
:exmp.
:p.
Das Tonigy-Laufwerk ist durch den Benutzer oder automatisch verbunden.
:xmp.
Header der CD ist eingelesen
:exmp.
:p.
Tonigy hat einen CD-Wechsel erkannt und den Header neu gelesen.
:xmp.
Jitter entdeckt, aber Anti-Jitter ausgeschaltet
:exmp.
:p.
Tonigy hat Jitter beim ôffnen eines Tracks oder Blocks entdeckt, aber
Anti-Jitter ist ausgeschaltet. Es sollte fÅr dieses LW eingeschaltet
werden.
:xmp.
Warnung&colon. LONG READ gescheitert (Track x, Sektor x)
:exmp.
:xmp.
Warnung&colon. LONG READ gescheitert (Block x, Sektor x)
:exmp.
:p.
Das LONG READ-Kommando hat einen Error-Code zurÅckgegeben. Das Kommando wird
wiederholt.
:xmp.
Fehler&colon. LONG READ gescheitert (Track x, Sektor x)
:exmp.
:xmp.
Fehler&colon. LONG READ gescheitert (Block x, Sektor x)
:exmp.
:p.
Das LONG READ-Kommando hat einen Error-Code nach dem 5. Lese-Versuch
zurÅckgegeben. Du solltest eine andere CD oder ein anderes LW verwenden.
:xmp.
Warnung&colon. Anti-Jitter gescheitert (Track x, Sektor x)
:exmp.
:xmp.
Warnung&colon. Anti-Jitter gescheitert (Block x, Sektor x)
:exmp.
:p.
Jitter der momentan gelesenen Daten kann nicht korrigiert werden. Das LONG
READ Kommando und das Anti-Jitter werden wiederholt.
:xmp.
Fehler&colon. Anti-Jitter gescheitert (Track x, Sektor x)
:exmp.
:xmp.
Fehler&colon. Anti-Jitter gescheitert (Block x, Sektor x)
:exmp.
:p.
Jitter der gelesenen Daten kann auch nach :link reftype=hd res=2.eingestellter Wiederholanzahl nicht
korrigiert werden:elink.. Du solltest eine andere CD oder ein anderes LW verwenden.
:h1.
Versionsgeschichte
:p.
AbkÅrzungen&colon.
:dl compact.
:dt.INF
:dd.Information
:dt.NEU
:dd.Neues Feature 
:dt.FIX
:dd.Fixed
:dt.GEé
:dd.GeÑndert 
:edl.
:fn id=INF.Information:efn.
:fn id=NEW.Neues Feature:efn.
:fn id=FIX.Fixed:efn.
:fn id=CHG.GeÑndert:efn.
:h2.
v1.0
:p.
Version 1.0
:dl compact.
:dt.:link reftype=fn refid=INF.INF:elink.&colon.
:dd.Erstes Release
:edl.
:h2.
v1.1
:p.
Version 1.1
:dl compact.
:dt.:link reftype=fn refid=NEW.NEU:elink.&colon.
:dd.Information Åber Sektorblîcke zu CDINFO.TXT hinzugefÅgt
:dt.:link reftype=fn refid=NEW.NEU:elink.&colon.
:dd.BLOCKS.RAW Verzeichnis
:dt.:link reftype=fn refid=NEW.NEU:elink.&colon.
:dd.Pop-Up MenÅ im Hauptfenster
:dt.:link reftype=fn refid=NEW.NEU:elink.&colon.
:dd.&odq.Allegemeine Einstellungen&cdq.-Dialog
:dt.:link reftype=fn refid=NEW.NEU:elink.&colon.
:dd.&odq.Hîchste PrioritÑt&cdq.-Option
:dt.:link reftype=fn refid=NEW.NEU:elink.&colon.
:dd.Deutsche BedienerfÅhrung und Dokumentation
:dt.:link reftype=fn refid=NEW.NEU:elink.&colon.
:dd.Erster Buchstabe der CD-ROM Auswahl
:dt.:link reftype=fn refid=NEW.NEU:elink.&colon.
:dd.Warnungen/Fehler werden in virtuelles Log auf dem Tonigy-LW geschrieben.
:dt.:link reftype=fn refid=NEW.NEU:elink.&colon.
:dd.Online-Hilfe (Englisch und Deutsch)
:dt.:link reftype=fn refid=NEW.NEU:elink.&colon.
:dd.Named Pipe zur Kontrolle/Steuerung von Tonigy
:dt.:link reftype=fn refid=CHG.GEé:elink.&colon.
:dd.Hintergrundfarbe des Hauptfensters zu blass-grau geÑndert
:dt.:link reftype=fn refid=CHG.GEé:elink.&colon.
:dd.Neue Methode der Jitter-Erkennung
:dt.:link reftype=fn refid=CHG.GEé:elink.&colon.
:dd.README-Datei zu README.TXT umbenannt
:dt.:link reftype=fn refid=CHG.GEé:elink.&colon.
:dd.RAW-Verzeichnis nach TRACKS.RAW umbenannt
:dt.:link reftype=fn refid=CHG.GEé:elink.&colon.
:dd.Tonigy Web-Seiten und Kundendiest E-mail geÑndert
:dt.:link reftype=fn refid=CHG.GEé:elink.&colon.
:dd.Einige Kleinigkeiten
:edl.
:h2.
v1.2
:p.
Version 1.2
:dl compact.
:dt.:link reftype=fn refid=NEW.NEU:elink.&colon.
:dd.Abbilden von Video-Tracks auf MPG-Dateien
:dt.:link reftype=fn refid=CHG.GEé:elink.&colon.
:dd.Mehr Informationen Åber Tracks
:dt.:link reftype=fn refid=CHG.GEé:elink.&colon.
:dd.Einige Kleinigkeiten
:edl.
:h1.
Lizenzinformationen
:p.
Anmerkung des öbersetzers&colon. Dieser Text wurde nicht Åbersetzt, da erstens
der öbersetzer kein Jurist ist und wesentliche Teile nicht adÑquat
Åbertragen kann und zweitens amerikanisches Lizenzrecht in weiten Teilen
in Deutschland NICHT gilt bzw. keine Entsprechung findet.

Insbesondere hat der Passus bezÅglich des Verbotes der WeiterverÑu·erung an
Dritte vor deutschen Gerichten keinen Bestand.

Dies sollte aber niemanden davon abhalten, den Gedanken der Shareware zu
fîrdern, dergestalt, dass das Programm, wie von den Autoren gefordert, nach
30 Tagen zu registrieren ist, wenn es weiter benutzt wird, anderenfalls vom
Rechner zu lîschen ist. BITTE SEID FAIR - die Programmierer von Tonigy sind
es auch&colon. sie lassen das Programm OHNE JEDE EinschrÑnkung testen.
:p.
License Information
:p.
:hp2.
NOTICE TO USERS&colon. CAREFULLY READ THE FOLLOWING LEGAL AGREEMENT. USE OF THE
SOFTWARE PROVIDED WITH THIS AGREEMENT (THE "SOFTWARE") CONSTITUTES YOUR
ACCEPTANCE OF THESE TERMS. IF YOU DO NOT AGREE TO THE TERMS OF THIS AGREEMENT, 
DO NOT INSTALL  AND/OR USE THIS SOFTWARE. USER'S USE OF THIS SOFTWARE IS
CONDITIONED UPON COMPLIANCE BY USER WITH THE TERMS OF THIS AGREEMENT.
:ehp2.
:p.
:hp2.LICENSE GRANT.:ehp2. Deffie Group grants you a license to use one copy of this
SOFTWARE on any one hardware product for as many licenses as you purchase.
"You" means the company, entity or individual whose funds are used to pay the
license fee. "Use" means storing, loading, installing, executing or displaying
the SOFTWARE. You may not  modify the  SOFTWARE or disable any licensing or
control features of the SOFTWARE  except as an intended part of the SOFTWARE's
programming features. When you first obtain a copy of the SOFTWARE, you are 
granted an evaluation period of not more than 30 days, after which time you
must pay for the SOFTWARE according to the terms and  prices discussed in the
SOFTWARE's documentation, or you must remove the SOFTWARE from your computer. 
This license is not transferable to any other hardware product or other
company, entity, or individual.
:p.
:hp2.NO OTHER WARRANTIES. DEFFIE GROUP DOES NOT WARRANT THAT THE SOFTWARE IS ERROR
FREE. DEFFIE GROUP DISCLAIMS ALL OTHER WARRANTIES WITH RESPECT TO THE SOFTWARE,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD
PARTY RIGHTS. SOME JURISDICTIONS DO NOT ALLOW THE  EXCLUSION OF IMPLIED 
WARRANTIES OR LIMITATIONS ON HOW LONG AN IMPLIED WARRANTY MAY LAST, OR THE
EXCLUSION OR LIMITATION OF INCIDENTAL OR CONSEQUENTIAL DAMAGES, SO THE ABOVE
LIMITATIONS OR EXCLUSIONS MAY NOT APPLY TO YOU. THIS WARRANTY GIVES YOU
SPECIFIC LEGAL RIGHTS AND YOU MAY ALSO HAVE OTHER RIGHTS WHICH VARY FROM
JURISDICTION TO JURISDICTION.
:ehp2.
:h1.
Registrierung
:p.
Die Registrierungskosten betragen nur 20 US-Dollar. Die Registrierung gilt
fÅr die aktuelle und alle zukÅnftigen Versionen von Tonigy.
.br
.br
Zum Registrieren oder fÅr weitergehende Informationen suche bitte den
&odq.:hp2.Register Now:ehp2.&cdq. Bereich der Tonigy-Seite auf
(:hp4.http&colon.//www.tonigy.com:ehp4.).
:h1.
Kontaktadressen und Dankeschîn
:p.
Send questions, bug reports and suggestions to :hp4.tonigy@defgroup.com:ehp4.
.br
Visit the Tonigy site&colon. :hp4.http&colon.//www.tonigy.com:ehp4.
:p.
öbersetzung der Hilfe-Texte und der deutschen OberflÑche&colon.
:p.
Klaus Kulbarsch
.br
FIDO (preferred)
.br
2&colon.2426/1205  V.34
.br
2&colon.2426/1206  X.75
.br
E-Mail&colon. kulle@t-online.de
:euserdoc.
