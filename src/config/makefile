# makefile for config

MDL = s
CC = gcc -g
CFLAGS = -I.

OBJS = gettoken.o config.o lazy.o

.c.o:
	$(CC) $(CFLAGS) -c $<

config.exe:	$(OBJS)
	$(CC) -oconfig.exe $(OBJS)

gettoken.obj:	gettoken.c gettoken.h lazy.h

config.obj:	config.c gettoken.h lazy.h

lazy.obj:	lazy.h

clean:
	rm *.obj
	rm config.exe
