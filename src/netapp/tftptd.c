/* tftptd.c - tftptd */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <network.h>
#include <tftp.h>
#include <mem.h>
#include <proc.h>

#define TFTPPLEN    (TFTPHLEN+TFTPDLEN+XGHLEN) /* max size TFTP packet */
#define err(num, str)	tftperr(dev, wbuff, num, str)

#define WTP wtp->t_data
#define RTP rtp->t_data

/*------------------------------------------------------------------------
 *  tftptd  --  TFTP transfer daemon process (created by tftpd)
 *------------------------------------------------------------------------
 */
PROCESS tftptd(buff,synch)
struct xgram *buff;
int synch;
{
	int		dev;
	int		fdev;
	int		dmode;
	int		opcode;
	int		bnum;
	int		len;
	union TFTP	*rtp, *wtp;
	struct xgram	*xptr;
	char		*rbuff, *wbuff;
	char		*fname;
	char		*mode;

    dev = newsiodv(xinu_open(UDP, ANYFPORT, ANYLPORT));
	if (isbaddev(dev)) {
		xinu_signal(synch);
		return(SYSERR);		/* Datagram open failure */
	}

    if ((char *)(wbuff=xmalloc(TFTPPLEN)) == NULL ||
        (char *)(rbuff=xmalloc(TFTPPLEN)) == NULL) {
		xinu_signal(synch);
		return(SYSERR);		/* Out of memory */
	}

	/* copy the request xinugram */
    blkcopy(rbuff, buff, TFTPPLEN);
	blkcopy(wbuff, buff, XGHLEN);	/* copy xinugram foreign addr/port */
	xinu_signal(synch);

	xptr = (struct xgram *)rbuff;
	rtp = (union TFTP *)xptr->xg_data;

	dmode = DG_NMODE | DG_TMODE;
    control(dev, DG_SETMODE, &dmode);
	control(dev, DG_CLEAR);

	opcode = net2hs(rtp->t_req.opcode);
	fname = rtp->t_req.file;		/* file name string */
	mode = fname + xinu_strlen(fname) + 1;	/* point to mode string */
	if (strcasecmp(mode, "octet") != 0) {
		err(TFTP_NOTDEF, "Octet mode support only.");
		return(SYSERR);
	}

	xptr = (struct xgram *)wbuff;
	wtp = (union TFTP *)xptr->xg_data;

	/* proceed with the transfer */
	switch (opcode) {
	case TFTP_RRQ:
		if ((fdev=xinu_fopen(fname,"rbo")) == SYSERR) {
			err(TFTP_FNOTF,"File not found.");
			return(SYSERR);
		}
		WTP.opcode = hs2net(TFTP_DATA);
		for (bnum=1 ; ; bnum++) {
			len = xinu_read(fdev, WTP.data, TFTPDLEN);
			if (len == EOF)
				len = 0;
			if (len < 0) {
				err(TFTP_NOTDEF,"Error reading file.");
				return(SYSERR);
			}
			WTP.blknum = hs2net(bnum);
			if (tftpsend(dev,wbuff,TFTPHLEN+len,rbuff,TFTPHLEN,
			    TFTP_ACK,bnum) == SYSERR)
				return(SYSERR);
			if (len < TFTPDLEN)
				return(OK);
		}
		
	case TFTP_WRQ:
		if ((fdev=xinu_fopen(fname,"wb")) == SYSERR) {
			err(TFTP_FNOTF,"File not found.");
			return(SYSERR);
		}
		WTP.opcode = hs2net(TFTP_ACK);
		for (bnum=1 ; ; bnum++) {
			WTP.blknum = hs2net(bnum-1);
			len = tftpsend(dev,wbuff,TFTPHLEN,rbuff,TFTPPLEN,
			    TFTP_DATA,bnum);
			if (len == SYSERR)
				return(SYSERR);
			len -= TFTPHLEN;	/* skip over the header */
			if (xinu_write(fdev, RTP.data, len) == SYSERR) {
				err(TFTP_DSKFULL,"Disk write error.");
				return(SYSERR);
			}
			if (len < TFTPDLEN) {
				/* send final ACK */
				WTP.blknum = hs2net(bnum);
				if (xinu_write(dev,wbuff,TFTPHLEN) == SYSERR)
					return(SYSERR);
				return(OK);
			}
		}

	default:
		err(TFTP_NOTDEF, "Illegal request.\n");
		return(SYSERR);
	}
}
