/* conf.h */
/* (GENERATED FILE; DO NOT EDIT) */

#define	NULLPTR	(char *)0

/* Device table declarations */
struct	devsw	{			/* device table entry		*/
	int	dvnum;
	char	dvnam[10];
	int	dvrefct;
	int	(*dvinit)();
	int	(*dvopen)();
	int	(*dvclose)();
	int	(*dvread)();
	int	(*dvwrite)();
	int	(*dvseek)();
	int	(*dvgetc)();
	int	(*dvputc)();
	int	(*dvcntl)();
	int	dvport;
	int	dvivec;
	int	dvovec;
	int	(*dviint)();
	int	(*dvoint)();
	char	*dvioblk;
	int	dvminor;
	};

extern	struct	devsw devtab[];		/* one entry per device		*/

/* Device name definitions */

#define	CONSOLE	0		/* class tty	*/
#define	STDIO	8		/* class xio	*/
#define	STDIN	9		/* class sio	*/
#define	STDOUT	10		/* class sio	*/
#define	STDERR	11		/* class sio	*/
#define	NULLDEV	19		/* class null	*/
#define	DOS	20		/* class dos	*/
#define	NAMESPACE	29		/* class nam	*/
#define	EC0	30		/* class eth	*/
#define	UDP	31		/* class dgm	*/
#define	TCP	40		/* class tcpm	*/
#define	PIPE	49		/* class pipm	*/

/* Control block sizes */

#define	Ntty	8
#define	Nxio	1
#define	Nsio	10
#define	Nnull	1
#define	Ndos	1
#define	Nmf	8
#define	Nnam	1
#define	Neth	1
#define	Ndgm	1
#define	Ndg	8
#define	Ntcpm	1
#define	Ntcp	8
#define	Npipm	1
#define	Npip	4

#define	NDEVS	54

/* Declarations of I/O routines referenced */

extern	int	ioerr();
extern	int	ttyinit();
extern	int	ttyopen();
extern	int	ionull();
extern	int	ttyread();
extern	int	ttywrite();
extern	int	ttygetc();
extern	int	ttyputc();
extern	int	ttycntl();
extern	int	ttyiin();
extern	int	lwinit();
extern	int	lwopen();
extern	int	lwclose();
extern	int	lwread();
extern	int	lwwrite();
extern	int	lwgetc();
extern	int	lwputc();
extern	int	lwcntl();
extern	int	xioopen();
extern	int	sioinit();
extern	int	sioopen();
extern	int	sioclose();
extern	int	sioread();
extern	int	siowrite();
extern	int	sioseek();
extern	int	siogetc();
extern	int	sioputc();
extern	int	siocntl();
extern	int	nullopen();
extern	int	nullread();
extern	int	nullwrit();
extern	int	nullgetc();
extern	int	dsinit();
extern	int	dsopen();
extern	int	dsread();
extern	int	dswrite();
extern	int	dsseek();
extern	int	dscntl();
extern	int	lfinit();
extern	int	lfopen();
extern	int	lfclose();
extern	int	lfread();
extern	int	lfwrite();
extern	int	lfseek();
extern	int	lfgetc();
extern	int	lfputc();
extern	int	msopen();
extern	int	mscntl();
extern	int	mfinit();
extern	int	mfclose();
extern	int	mfread();
extern	int	mfwrite();
extern	int	mfseek();
extern	int	mfgetc();
extern	int	mfputc();
extern	int	naminit();
extern	int	namopen();
extern	int	namcntl();
extern	int	ethinit();
extern	int	ethwrite();
extern	int	ethinter();
extern	int	dgmopen();
extern	int	dgmcntl();
extern	int	dginit();
extern	int	dgclose();
extern	int	dgread();
extern	int	dgwrite();
extern	int	dgcntl();
extern	int	tcpmopen();
extern	int	tcpmcntl();
extern	int	tcpinit();
extern	int	tcpclose();
extern	int	tcpread();
extern	int	tcpwrite();
extern	int	tcpcntl();
extern	int	tcpgetc();
extern	int	tcpputc();
extern	int	pipopen();
extern	int	pipinit();
extern	int	pipclose();
extern	int	pipread();
extern	int	pipwrite();
extern	int	pipgetc();
extern	int	pipputc();
extern	int	rfsopen();
extern	int	rfminit();
extern	int	rfopen();
extern	int	rfmclose();
extern	int	rfmcntl();
extern	int	rfinit();
extern	int	rfclose();
extern	int	rfread();
extern	int	rfwrite();
extern	int	rfseek();
extern	int	rfcntl();
extern	int	rfgetc();
extern	int	rfputc();


/* Configuration and size constants */

#define MEMMARK				/* enable memory marking	*/
#define NPROC   64                      /* number of user processes     */
#define NSEM    128                     /* total number of semaphores   */

#define	RTCLOCK				/* system has a real-time clock	*/
/* #define NETWORK  */                  /* include network interfaces   */
#define NETDEV  EC0                     /* network input/output device  */

#define	TCPSBS	4096			/* TCP send buffer sizes	*/
#define TCPRBS  4096                    /* TCP receive buffer sizes     */

#define NPORTS  16

#define	TIMERGRAN	1		/* timer granularity, secs/10	*/

#ifndef	Noth
#define	Noth	0
#endif

#define VERSION "1.0 (2007), based on XINU 7.9 PC" /* label printed at startup     */
