/* tftpsend.c  -  tftpsend */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <network.h>
#include <tftp.h>
#include <mem.h>

/*------------------------------------------------------------------------
 *  tftpsend -- send a TFTP packet and wait for an acknowledgment
 *------------------------------------------------------------------------
 */
tftpsend(dev, wbuff, wlen, rbuff, rlen, rtype, bnum)
int dev;
char *wbuff;
int wlen;
char *rbuff;
int rlen;
int rtype;
int bnum;
{
	int		len;
	struct xgram	*xptr;
	union	TFTP	*tp;
	int 		rtrys;
	int		opcode;
	int		errcode;

	rtrys = 0;
	if (xinu_write(dev,wbuff,wlen) == SYSERR) {
		xinu_fprintf(STDERR, "error writing to UDP port\n");
		return(SYSERR);
	}

	for (;;) {
        len = xinu_read(dev,rbuff,rlen);

		if (len == SYSERR) {
			xinu_fprintf(STDERR,"error reading block #%d\n", bnum);
			return(SYSERR);
		}

		if (len == TIMEOUT) {
			if ( ++rtrys == TFTP_RETRY ) {
				xinu_fprintf(STDERR,"timeout on %s of block #%d\n",
				    (rtype==TFTP_ACK)?"ack":"read", bnum);
				return(SYSERR);
			}
			if (xinu_write(dev,wbuff,wlen) == SYSERR) {
				xinu_fprintf(STDERR, "error writing to UDP port\n");
				return(SYSERR);
			}
			continue;
		}

		if (len < TFTPHLEN) {
			xinu_fprintf(STDERR,"illegal TFTP packet\n");
			return(SYSERR);
		}

		/* real TFTP packet if we get here */

		xptr = (struct xgram *)rbuff;
		tp   = (union TFTP *)xptr->xg_data;
#define TPD tp->t_data
#define TPE tp->t_err
		opcode = net2hs(TPD.opcode);
		if (opcode == TFTP_ERR) {
			errcode = net2hs(TPE.errcode);
			if (errcode == TFTP_NOTDEF)
				xinu_fprintf(STDERR,"%s\n",TPE.errmsg);
			else
				xinu_fprintf(STDERR,"Received error #%d\n",errcode);
			return(SYSERR);
		}

		if (opcode == rtype && net2hs(TPD.blknum) == bnum)
			return(len);
	}
}
