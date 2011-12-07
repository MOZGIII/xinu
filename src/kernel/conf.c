/* conf.c */
/* (GENERATED FILE: DO NOT EDIT) */

#include <conf.h>
#include <pc.h>			/* generic PC definitions */
#include <ec.h>                 /* Ethernet Controller definitions */

/* device independent I/O switch */

struct	devsw	devtab[NDEVS] = {

/*------------------------------------------------------------------------
 * Format of each entry is:
 *
 * dev number, dev name, ref count,
 * init, open, close,
 * read, write, seek,
 * getc, putc, cntl,
 * port addr, dev input vector, dev output vector,
 * input interrupt routine, output interrupt routine,
 * dev i/o block, minor dev number
 *------------------------------------------------------------------------
 */

/* CONSOLE is tty on BIOS */
0,"tty",-1,
ttyinit,ttyopen,ionull,
ttyread,ttywrite,ionull,
ttygetc,ttyputc,ttycntl,
0,KBDVEC|BIOSFLG,0,
ttyiin,ioerr,
NULLPTR,0,

/* GENERIC is tty on WINDOW */
1,"",0,
lwinit,lwopen,lwclose,
lwread,lwwrite,ionull,
lwgetc,lwputc,lwcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,1,

/* GENERIC is tty on WINDOW */
2,"",0,
lwinit,lwopen,lwclose,
lwread,lwwrite,ionull,
lwgetc,lwputc,lwcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,2,

/* GENERIC is tty on WINDOW */
3,"",0,
lwinit,lwopen,lwclose,
lwread,lwwrite,ionull,
lwgetc,lwputc,lwcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,3,

/* GENERIC is tty on WINDOW */
4,"",0,
lwinit,lwopen,lwclose,
lwread,lwwrite,ionull,
lwgetc,lwputc,lwcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,4,

/* GENERIC is tty on WINDOW */
5,"",0,
lwinit,lwopen,lwclose,
lwread,lwwrite,ionull,
lwgetc,lwputc,lwcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,5,

/* GENERIC is tty on WINDOW */
6,"",0,
lwinit,lwopen,lwclose,
lwread,lwwrite,ionull,
lwgetc,lwputc,lwcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,6,

/* GENERIC is tty on WINDOW */
7,"",0,
lwinit,lwopen,lwclose,
lwread,lwwrite,ionull,
lwgetc,lwputc,lwcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,7,

/* STDIO is xio */
8,"stdio",-1,
ionull,xioopen,ionull,
ioerr,ioerr,ioerr,
ioerr,ioerr,ioerr,
0,0,0,
ioerr,ioerr,
NULLPTR,0,

/* STDIN is sio */
9,"stdin",-1,
sioinit,sioopen,sioclose,
sioread,siowrite,sioseek,
siogetc,sioputc,siocntl,
0,0,0,
ioerr,ioerr,
NULLPTR,0,

/* STDOUT is sio */
10,"stdout",-1,
sioinit,sioopen,sioclose,
sioread,siowrite,sioseek,
siogetc,sioputc,siocntl,
0,0,0,
ioerr,ioerr,
NULLPTR,1,

/* STDERR is sio */
11,"stderr",-1,
sioinit,sioopen,sioclose,
sioread,siowrite,sioseek,
siogetc,sioputc,siocntl,
0,0,0,
ioerr,ioerr,
NULLPTR,2,

/* GENERIC is sio */
12,"",-1,
sioinit,sioopen,sioclose,
sioread,siowrite,sioseek,
siogetc,sioputc,siocntl,
0,0,0,
ioerr,ioerr,
NULLPTR,3,

/* GENERIC is sio */
13,"",-1,
sioinit,sioopen,sioclose,
sioread,siowrite,sioseek,
siogetc,sioputc,siocntl,
0,0,0,
ioerr,ioerr,
NULLPTR,4,

/* GENERIC is sio */
14,"",-1,
sioinit,sioopen,sioclose,
sioread,siowrite,sioseek,
siogetc,sioputc,siocntl,
0,0,0,
ioerr,ioerr,
NULLPTR,5,

/* GENERIC is sio */
15,"",-1,
sioinit,sioopen,sioclose,
sioread,siowrite,sioseek,
siogetc,sioputc,siocntl,
0,0,0,
ioerr,ioerr,
NULLPTR,6,

/* GENERIC is sio */
16,"",-1,
sioinit,sioopen,sioclose,
sioread,siowrite,sioseek,
siogetc,sioputc,siocntl,
0,0,0,
ioerr,ioerr,
NULLPTR,7,

/* GENERIC is sio */
17,"",-1,
sioinit,sioopen,sioclose,
sioread,siowrite,sioseek,
siogetc,sioputc,siocntl,
0,0,0,
ioerr,ioerr,
NULLPTR,8,

/* GENERIC is sio */
18,"",-1,
sioinit,sioopen,sioclose,
sioread,siowrite,sioseek,
siogetc,sioputc,siocntl,
0,0,0,
ioerr,ioerr,
NULLPTR,9,

/* NULLDEV is null */
19,"null",-1,
ionull,nullopen,ionull,
nullread,nullwrit,ionull,
nullgetc,ionull,ionull,
0,0,0,
ioerr,ioerr,
NULLPTR,0,

/* DOS is dos */
20,"dos",-1,
ionull,msopen,ionull,
ioerr,ioerr,ioerr,
ioerr,ioerr,mscntl,
0,0,0,
ioerr,ioerr,
NULLPTR,0,

/* GENERIC is mf */
21,"",0,
mfinit,nullopen,mfclose,
mfread,mfwrite,mfseek,
mfgetc,mfputc,ioerr,
0,0,0,
ioerr,ioerr,
NULLPTR,0,

/* GENERIC is mf */
22,"",0,
mfinit,nullopen,mfclose,
mfread,mfwrite,mfseek,
mfgetc,mfputc,ioerr,
0,0,0,
ioerr,ioerr,
NULLPTR,1,

/* GENERIC is mf */
23,"",0,
mfinit,nullopen,mfclose,
mfread,mfwrite,mfseek,
mfgetc,mfputc,ioerr,
0,0,0,
ioerr,ioerr,
NULLPTR,2,

/* GENERIC is mf */
24,"",0,
mfinit,nullopen,mfclose,
mfread,mfwrite,mfseek,
mfgetc,mfputc,ioerr,
0,0,0,
ioerr,ioerr,
NULLPTR,3,

/* GENERIC is mf */
25,"",0,
mfinit,nullopen,mfclose,
mfread,mfwrite,mfseek,
mfgetc,mfputc,ioerr,
0,0,0,
ioerr,ioerr,
NULLPTR,4,

/* GENERIC is mf */
26,"",0,
mfinit,nullopen,mfclose,
mfread,mfwrite,mfseek,
mfgetc,mfputc,ioerr,
0,0,0,
ioerr,ioerr,
NULLPTR,5,

/* GENERIC is mf */
27,"",0,
mfinit,nullopen,mfclose,
mfread,mfwrite,mfseek,
mfgetc,mfputc,ioerr,
0,0,0,
ioerr,ioerr,
NULLPTR,6,

/* GENERIC is mf */
28,"",0,
mfinit,nullopen,mfclose,
mfread,mfwrite,mfseek,
mfgetc,mfputc,ioerr,
0,0,0,
ioerr,ioerr,
NULLPTR,7,

/* NAMESPACE is nam */
29,"namespace",-1,
naminit,namopen,ionull,
ioerr,ioerr,ioerr,
ioerr,ioerr,namcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,0,

/* EC0 is eth */
30,"ec0",0,
ethinit,nullopen,ionull,
ioerr,ethwrite,ioerr,
ioerr,ioerr,ioerr,
EC0PORT,EC0VEC,0,
ethinter,ioerr,
NULLPTR,0,

/* UDP is dgm */
31,"udp",-1,
ionull,dgmopen,ionull,
ioerr,ioerr,ioerr,
ioerr,ioerr,dgmcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,0,

/* GENERIC is dg */
32,"",0,
dginit,nullopen,dgclose,
dgread,dgwrite,ioerr,
ioerr,ioerr,dgcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,0,

/* GENERIC is dg */
33,"",0,
dginit,nullopen,dgclose,
dgread,dgwrite,ioerr,
ioerr,ioerr,dgcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,1,

/* GENERIC is dg */
34,"",0,
dginit,nullopen,dgclose,
dgread,dgwrite,ioerr,
ioerr,ioerr,dgcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,2,

/* GENERIC is dg */
35,"",0,
dginit,nullopen,dgclose,
dgread,dgwrite,ioerr,
ioerr,ioerr,dgcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,3,

/* GENERIC is dg */
36,"",0,
dginit,nullopen,dgclose,
dgread,dgwrite,ioerr,
ioerr,ioerr,dgcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,4,

/* GENERIC is dg */
37,"",0,
dginit,nullopen,dgclose,
dgread,dgwrite,ioerr,
ioerr,ioerr,dgcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,5,

/* GENERIC is dg */
38,"",0,
dginit,nullopen,dgclose,
dgread,dgwrite,ioerr,
ioerr,ioerr,dgcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,6,

/* GENERIC is dg */
39,"",0,
dginit,nullopen,dgclose,
dgread,dgwrite,ioerr,
ioerr,ioerr,dgcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,7,

/* TCP is tcpm */
40,"tcp",-1,
ionull,tcpmopen,ionull,
ioerr,ioerr,ioerr,
ioerr,ioerr,tcpmcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,0,

/* GENERIC is tcp */
41,"",0,
tcpinit,nullopen,tcpclose,
tcpread,tcpwrite,ioerr,
tcpgetc,tcpputc,tcpcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,0,

/* GENERIC is tcp */
42,"",0,
tcpinit,nullopen,tcpclose,
tcpread,tcpwrite,ioerr,
tcpgetc,tcpputc,tcpcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,1,

/* GENERIC is tcp */
43,"",0,
tcpinit,nullopen,tcpclose,
tcpread,tcpwrite,ioerr,
tcpgetc,tcpputc,tcpcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,2,

/* GENERIC is tcp */
44,"",0,
tcpinit,nullopen,tcpclose,
tcpread,tcpwrite,ioerr,
tcpgetc,tcpputc,tcpcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,3,

/* GENERIC is tcp */
45,"",0,
tcpinit,nullopen,tcpclose,
tcpread,tcpwrite,ioerr,
tcpgetc,tcpputc,tcpcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,4,

/* GENERIC is tcp */
46,"",0,
tcpinit,nullopen,tcpclose,
tcpread,tcpwrite,ioerr,
tcpgetc,tcpputc,tcpcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,5,

/* GENERIC is tcp */
47,"",0,
tcpinit,nullopen,tcpclose,
tcpread,tcpwrite,ioerr,
tcpgetc,tcpputc,tcpcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,6,

/* GENERIC is tcp */
48,"",0,
tcpinit,nullopen,tcpclose,
tcpread,tcpwrite,ioerr,
tcpgetc,tcpputc,tcpcntl,
0,0,0,
ioerr,ioerr,
NULLPTR,7,

/* PIPE is pipm */
49,"pipe",-1,
ionull,pipopen,ionull,
ioerr,ioerr,ioerr,
ioerr,ioerr,ionull,
0,0,0,
ioerr,ioerr,
NULLPTR,0,

/* GENERIC is pip */
50,"",-1,
pipinit,nullopen,pipclose,
pipread,pipwrite,ioerr,
pipgetc,pipputc,ionull,
0,0,0,
ioerr,ioerr,
NULLPTR,0,

/* GENERIC is pip */
51,"",-1,
pipinit,nullopen,pipclose,
pipread,pipwrite,ioerr,
pipgetc,pipputc,ionull,
0,0,0,
ioerr,ioerr,
NULLPTR,1,

/* GENERIC is pip */
52,"",-1,
pipinit,nullopen,pipclose,
pipread,pipwrite,ioerr,
pipgetc,pipputc,ionull,
0,0,0,
ioerr,ioerr,
NULLPTR,2,

/* GENERIC is pip */
53,"",-1,
pipinit,nullopen,pipclose,
pipread,pipwrite,ioerr,
pipgetc,pipputc,ionull,
0,0,0,
ioerr,ioerr,
NULLPTR,3
};
