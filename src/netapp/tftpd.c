/* tftpd.c - tftpd */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <network.h>
#include <tftp.h>
#include <mem.h>
#include <proc.h>

#define err(num, str)	tftperr(dev, buff, num, str)

/*------------------------------------------------------------------------
 *  tftpd  --  TFTP daemon process
 *------------------------------------------------------------------------
 */
PROCESS tftpd()
{
	int		dev;
	int		lport;
	int		tftptd();
	char		*buff;
	union TFTP	*tp;
	struct xgram	*xptr;
	int		npid;
	int		synch;

    lport = UP_TFTP;
    dev = newsiodv(xinu_open(UDP, ANYFPORT, lport));
    if (isbaddev(dev)) {
        xinu_fprintf(STDERR, "tftpd: UDP open fails\n");
		return(SYSERR);
	}

	if ((buff=xmalloc(sizeof(struct xgram))) == NULL) {
		xinu_fprintf(STDERR, "tftpd: not enough memory.\n");
		return(SYSERR);
	}

	xptr = (struct xgram *)buff;
	tp = (union TFTP *)xptr->xg_data;
	if ((synch=screate(0)) == SYSERR) {
		xinu_fprintf(STDERR, "tftpd: semaphore creation error.\n");
		return(SYSERR);
	}

	/* close standard I/O devices */
	xinu_close(STDIN);
	xinu_close(STDOUT);
    /* hook STDERR to the null device */
	setpdev(xinu_getpid(), STDERR, NULLDEV);

	/* read and process requests */
	for (;;) {
        if (xinu_read(dev, buff, TFTPHLEN+TFTPDLEN) < 0)
			return(SYSERR);
		switch (net2hs(tp->t_req.opcode)) {
		case TFTP_WRQ:
		case TFTP_RRQ:
            if ((npid=create(tftptd, INITSTK, INITPRIO, "TFTPTD", 
				3, buff, synch)) == SYSERR) {
				err(TFTP_NOTDEF,
				    "Could not create tftptd daemon.");
				continue;
			}
			break;

		default:
			err(TFTP_ILLOP,"Illegal operation.");
			continue;
		}
		resume(npid);
		wait(synch);
	}
}
