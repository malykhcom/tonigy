DEST=tonigy
SRC=..\GLOBAL;INCLUDE;SOURCE;..\..\..\RSAEURO;$(%WATCOM)\H;$(%WATCOM)\H\OS2

C_OPT=-wx -sg -bm -s -ocehilnrtuz -ol+ -5r -fp3
LINK_OPT=debug all

OBJS1=main.obj debug.obj agent.obj cdrom.obj file.obj ea.obj ui.obj button.obj
OBJS2=common.obj util.obj errors.obj key.obj npipe.obj locale.obj
OBJS3=attach.obj chdir.obj chgfilep.obj close.obj delete.obj fileattribute.obj fileinfo.obj
OBJS4=findclose.obj findfirst.obj findfromname.obj findnext.obj fsinfo.obj
OBJS5=fsctl.obj opencreate.obj pathinfo.obj read.obj iso.obj video.obj audio.obj cdi.obj

OBJS=$(OBJS2) $(OBJS1) $(OBJS3) $(OBJS4) $(OBJS5)

.erase
.c: $(SRC)
 
.c.obj: .AUTODEPEND
	wcc386 $(C_OPT) -i=$(SRC) $<

$(DEST).exe : $(OBJS) makefile $(DEST).lnk $(DEST).res tonigy.inf
	unlock $(DEST).exe
        wlink @$(DEST).lnk
	rc $(DEST).res $(DEST).exe
        lxlite $(DEST).exe

$(DEST).lnk : makefile
	echo name $(DEST).exe >$^@
        echo system os2v2_pm >>$^@
        echo option stack=64k >>$^@
        echo option eliminate >>$^@
        echo option map=tonigy.map >>$^@
	echo libpath $(%WATCOM)\lib386 >>$^@
     	echo libpath $(%WATCOM)\lib386\os2 >>$^@
        echo lib ..\..\..\RSAEURO\rsaeuro.lib >>$^@
#	echo $(LINK_OPT) >>$^@
	for %i in ($(OBJS)) do echo file %i >>$^@

$(DEST).res : $(DEST).rc $(DEST).h $(DEST).dlg
        rc -r -p $(DEST).rc $(DEST).res

tonigy.inf : ..\generated\ipf\tonigy.ipf
	ipfc -D:001 -C:0850 -L:ENU ..\generated\ipf\tonigy.ipf
	copy ..\generated\ipf\tonigy.hlp tonigy.inf

#german.hlp : german.ipf
#	ipfc -D:049 -C:0850 -L:DEU german.ipf
#	copy german.hlp german.inf
