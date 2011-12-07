/* rfile.h */

/* Remote file device control block and defined constants */

/* Constants for server device control functions */

#define	RFCLEAR		1		/* Clear incoming messages	*/
#define	RFPORT		2001		/* default remote file port	*/
#define	MAXRNAME	64		/* max length for named address	*/

/* Constants for controlling retrys for server communication */

#define	RTIMOUT		5		/* Timeout for server response	*/
#define	RMAXTRY		2		/* Number of retrys per op.	*/

/* Constants for rf pseudo-device state variable */

#define	RFFREE		-1		/* pseudo-device is unused	*/
#define RFLIMBO		0		/* pseudo-device is in limbo	*/
#define	RFUSED		1		/* pseudo-device is used	*/
#define RFMFREE     -1
#define RFMLIMBO    0
#define RFMUSED     1
#define RFMOPENED   2
#define RFMCLOSED   3

/* Declaration of remote file master I/O control block */

struct	rfmastr {
	int 	rdev;			/* device number for this entry	*/
	int 	rstatus;		/* device in use?		*/
	int 	rmutex;			/* exclusion for this device	*/
	int	rcount;			/* no. of open files on device	*/
	struct	fpacket	rpacket;	/* packet construction area	*/
	IPaddr	ripaddr;		/* ip address of remote server	*/
	int 	rport;			/* port number on remote server	*/
	char	rname[MAXRNAME];	/* name of remote server	*/
	int 	rnetdev;		/* internet device assigned to	*/
};					/* remote file server		*/

#ifndef Nrfm
#define	Nrfm	1
#endif

extern  struct  rfmastr Rf[];       /* remote file master devices   */

/* Declaration of rf pseudo-device I/O control block */

struct	rfblk	{			/* Remote file control block	*/
	int	rf_dnum;		/* File's device num in devtab	*/
	char	rf_name[RNAMLEN];	/* Name of remote file		*/
	int	rf_state;		/* State of this pseudo device	*/
	int	rf_mode;		/* FLREAD, FLWRITE or both	*/
	int	rf_mutex;		/* exclusion for this file	*/
	long	rf_pos;			/* current byte offset in file	*/
	struct	rfmastr	*rf_mastptr;	/* pointer to the master device */
					/* control block		*/
};

#ifndef	Nrf
#define	Nrf	1
#endif

extern	struct 	rfblk	rftab[];	/* remote file control blocks	*/
