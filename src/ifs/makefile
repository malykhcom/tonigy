DDK    = d:\msc60\ddk
MSC60  = d:\msc60

SRC=..\GLOBAL;.

CC16 = $(MSC60)\binp\cl
LINK16 = link

C16FLAGS = /c /W3 /Zlp /G2s /Anlw /Onir
# /Fc # /Od
L16FLAGS = /map /nod /exepack /packd /a:16 /far
#L16FLAGS = /map /nod /a:16 /far

C16INC = -I..\GLOBAL -I$(DDK)\H

L16LIBS = $(DDK)\lib\dhcalls.lib $(DDK)\lib\fshelper.lib $(DDK)\lib\os2286.lib

.h: $(SRC)
.c: $(SRC)

all: tonigy.ifs

tonigy.ifs: tonigy.obj tonigy.def 
	$(LINK16) $(L16FLAGS) tonigy.obj,$@,,$(L16LIBS),tonigy.def

tonigy.obj: tonigy.c fsd.h ifs.h
	$(CC16) $(C16FLAGS) $(C16INC) $<
