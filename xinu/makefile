# Xinu makefile
SRCDIR = ../src
H = ../src/h
L = ../src/bin

MDL = h
CC = gcc
CFLAGS = -I$(H) -gstabs
ASM = nasm /MX /I$(H) -o elf
LINK = ld
LIB = ar rcs
LIBFLAGS = /c

.c.o:
	$(CC) -c $(CFLAGS) $<

.s.o:
	$(ASM) $*

.o.exe:
	$(CC) -g -e$* $*.obj $(LIBS) -lc

# Files which somehow gcc can't find in .a. 
SRCS = xinu.c
OBJS = junk.o xinu.o
EXES = xinu.exe
LIBS = $(L)/xinu.a $(L)/network.a $(L)/serial.a $(L)/tcl.a
XOBJS = $(SRCDIR)/kernel/pcreate.o $(SRCDIR)/kernel/psendi.o \
$(SRCDIR)/kernel/gpq.o $(SRCDIR)/kernel/pdelete.o \
$(SRCDIR)/kernel/preset.o $(SRCDIR)/kernel/pcount.o \
$(SRCDIR)/kernel/preceive.o $(SRCDIR)/kernel/psend.o \
$(SRCDIR)/kernel/mkpool.o $(SRCDIR)/kernel/blkcmp.o \
$(SRCDIR)/kernel/sleep10.o $(SRCDIR)/kernel/suspend.o \
$(SRCDIR)/kernel/getbufi.o

all:	$(EXES)

xinu.exe: $(OBJS) $(LIBS)
	$(CC) -oxinu.exe xinu.o $(XOBJS)  $(LIBS) -lm -lncurses -ldl

clean:	FRC
	rm -rf *.exe
	rm -rf *.o
	cd $(SRCDIR) && make clean

depend:	FRC
	makedepend

FRC:	makefile

$(LIBS):
	rm -rf $@
	cd $(SRCDIR) && make
# DO NOT DELETE THIS LINE - 'make depend' depends on it
