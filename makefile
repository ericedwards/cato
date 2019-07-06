#
# Copyright (c) 1991 Eric A. Edwards
#
# This file is part of Cato, an implemenation of D.A.M.M.I.T.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# File: makefile
# Usage:
#

TASK = cato

SHELL =	/bin/sh
TOOLPATH = /usr/local/bin/68
AS = $(TOOLPATH)/as68
CC = $(TOOLPATH)/cc68
LD = $(TOOLPATH)/ld68
DW = $(TOOLPATH)/sform68
MP = $(TOOLPATH)/map68
SZ = $(TOOLPATH)/size68

ASFLAGS =
CCFLAGS = -O -c
CAFLAGS =
LDFLAGS = -X

OBJS = low.o main.o ui.o loop.o mfp.o dim.o cio.o global.o end.o

$(TASK):	$(OBJS)
	$(LD) $(LDFLAGS) -o $(TASK) $(OBJS) -lc
	$(DW) -e $(TASK) > $(TASK).U130
	$(DW) -o $(TASK) > $(TASK).U131
	$(DW) -r $(TASK) > $(TASK).dwn
	$(MP) $(TASK) > $(TASK).map
	$(SZ) $(TASK)

global.o:	global.h
	rm -f global.c
	cp global.h global.c
	$(CC) $(CCFLAGS) -DDEFINE_GLOBALS global.c

.c.o:
	$(CC) $(CCFLAGS) $*.c

.s.o:
	$(AS) $(ASFLAGS) $*.s


#
# Other useful targets
#

clean:
	rm -f $(OBJS)
	rm -f *.L global.c
	rm -f $(TASK).map $(TASK).dwn $(TASK).bin $(TASK)

lint:
	make clean
	lint -nu -DLINT *.c

count:
	make clean
	wc -l *.[csh]

print:
	make clean
	pr makefile *.[csh] | lpr

cheat:
	make clean
	make $(TASK)

#
# Dependencies
#

low.o:	low.s
main.o:	main.c hw.h global.h
loop.o:	loop.c hw.h global.h
ui.o:	ui.c hw.h global.h
mfp.o:	mfp.c hw.h global.h
dim.o:	dim.c hw.h global.h
cio.o:	cio.c
end.o:	end.s
