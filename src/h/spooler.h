/* $Id: spooler.h,v 1.4 1992/11/02 21:16:45 johnr Exp $ */

typedef	struct spl_dnode	SPL_DNODE;		/* data node */

#define		SPL_SZDATA		1024

struct spl_dnode
{
	u_short		in_status;				/* gives status of node */
	u_short		in_len;					/* length of data in a data block */
	u_long		in_jobid;				/* job-id key value */
	SPL_DNODE	*in_link;				/* points to next dnode */
	u_char		in_buf[SPL_SZDATA];		/* data goes here */
};

/*
 *	Bit field defines for the in_status field of SPL_DNODE
 */
#define	SPL_STUSED		0x0001			/* node is being used */
#define	SPL_STBEG		0x0002			/* beginning node */
#define	SPL_STEND		0x0004			/* ending node */
#define	SPL_STFULL		0x0008			/* data portion is full */


typedef	struct spl_qjobs	SPL_QJOBS;

#define		QJNS		24			/* QJOB namesize */

struct spl_qjobs
{
	u_long		qj_jobid;		/* key value: job id = 0 when on free list */
	u_long		qj_len;			/* total file length */
	u_char		qj_uname[QJNS];	/* user name */
	u_char		qj_fname[QJNS];	/* file name */
	u_char		qj_hname[QJNS];	/* host name */
	IPaddr		qj_hostip;		/* host ip address */
	u_long		qj_proto;		/* sending protocol type */
	SPL_DNODE	*qj_iptr;		/* pointer to START record of file */
	SPL_DNODE	*qj_fillp;		/* pointer to currently filling record */
	SPL_QJOBS	*qj_flink;		/* forward link */
	SPL_QJOBS	*qj_blink;		/* back link */
};

/*
 *	Defines for the qj_proto field of SPL_QJOB
 */
#define	SPL_LPD			1				/* lpd protocol */
#define SPL_TFTPD_N		2				/* tftp netasci protocol */
#define SPL_TFTPD_O		3				/* tftp octet protocol */


#define	SPL_SZDNODE		sizeof(struct spl_dnode)
#define	SPL_SZQJOBS		sizeof(struct spl_qjobs)

/*
 *	Usage of these 2 defines is Temporary.  These values need
 *	to be calculated at run time, based on the amount of available
 *	memory.
 */
/* #define		SPL_NUMDNODES	2048 */
#define		SPL_NUMDNODES	1024
#define		SPL_NUMQJOBS	200


/*
 *	$Log: spooler.h,v $
 * Revision 1.4  1992/11/02  21:16:45  johnr
 * JR: Added the qj_hostip field to struct spl_qjobs.
 *
 * Revision 1.3  1992/10/13  20:17:08  johnr
 * JR: Added a constant, QJNS, which is used as the array size for names stored
 *     in the QJOB.  For testing purposes, reduced NUMDNODES from 2048 to 1024.
 *
 * Revision 1.2  1992/10/07  17:40:02  johnr
 * JR: Added several fields to the QJOB struct.  Added defines for
 *     the qj_proto field.  Changed the value of NUMQJOBS.
 *
 * Revision 1.1  1992/08/04  22:52:23  johnr
 * JR: Initial version of these files under 7.9.  All were originally
 *     developed under 7.1.
 *
 * Revision 1.1  1992/07/07  21:17:01  johnr
 * JR: structs, typedefs, and defines for the spooler
 *
 *
 */
