/* $Id: lpd.h,v 1.1 1992/10/07 17:40:55 johnr Exp $ */



typedef struct
{
	u_long	lp_status;			/* 0 = available, 1 = used */
	u_long	lp_command;			/* command given to lpd. 0 = no command yet */
	u_long	lp_state;			/* 0 = no state yet */
	u_long	lp_fsize;			/* filesize, as given in 02 or 03 sub-cmd */
	int		lp_fd;				/* file descriptor for reads and writes */
	int		lp_pid;				/* pid of the process */
} LPD_SLOTS;

/*
 *	LPC_ defines - line printer command-field defines
 */
#define		LPC_PRINTW			1		/* print any waiting jobs */
#define		LPC_RECVJOB			2		/* receive a printer job */
#define		LPC_SQSS			3		/* send Q state - short */
#define		LPC_SQSL			4		/* send Q state - long */
#define		LPC_RMJOBS			5		/* Remove jobs */

/*
 *	LPS_ defines - line printer state-field defines
 */
#define		LPS_WAITSC			0x01	/* wait on sub-command */
#define		LPS_RECVDATA		0x02	/* Receiving Data */
#define		LPS_DATADONE		0x04	/* Done receiving data file */
#define		LPS_RECVCONT		0x08	/* Receiving Control File */
#define		LPS_CONTDONE		0x10	/* Done receiving control file */
#define		LPS_RECV00			0x20	/* Receiving the 0 terminator */

/*
 *	RJSC_ defines - Receive Job Sub-command defines
 */
#define		RJSC_ABORT			1		/* Abort Job */
#define		RJSC_CONTROL		2		/* Receive Control File */
#define		RJSC_DATA			3		/* Receive Data File */


#define	PRINTER_PORT	515
#define	LPD_NSLOTS		4
