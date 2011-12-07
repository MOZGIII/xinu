/* x_tftp.c  -  x_tftp */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <mem.h>
#include <tty.h>
#include <network.h>
#include <tftp.h>

#define TFTPPLEN    (TFTPHLEN+TFTPDLEN+XGHLEN) /* max size TFTP packet */
#define err(num, str)	tftperr(dev, wbuff, num, str)

#define WTP wtp->t_data
#define RTP rtp->t_data

LOCAL mkreq(union TFTP *tp, char *fname, char *mode);
static usage();
/*------------------------------------------------------------------------
 *  x_tftp  --  (command tftp) conduct one TFTP send/receive transaction
 *------------------------------------------------------------------------
 */
COMMAND	x_tftp(nargs, args)
int	nargs;
char	*args[];
{
	char		*lname, *fname;
	IPaddr		IP;
	int		opcode;
	int		dev;
	int		ldev;
    int     dgmode;
	int		bnum;
	int		len, rlen;
	union 		TFTP	*rtp, *wtp;
	struct	xgram	*xptr;
	char		*wbuff, *rbuff;
	
	if (nargs < 4 || nargs > 5) {
		usage();
		return(SYSERR);
	}

	if (strcmp(args[1],"put") == 0) {
		opcode = TFTP_WRQ;
		lname = args[3];
		fname = (nargs==5) ? args[4] : args[3];
	} else if (strcmp(args[1], "get") == 0) {
		opcode = TFTP_RRQ;
		fname = args[3];
		lname = (nargs==5) ? args[4] : args[3];
	} else {
		usage();
		return(SYSERR);
	}

	/* convert name to IPaddr format */
	name2ip(IP, args[2]);

    dev = newsiodv(xinu_open(UDP, ANYFPORT, ANYLPORT));
	if (isbaddev(dev))
		return(SYSERR);

    if ((wbuff=xmalloc(TFTPPLEN)) == NULL ||
        (rbuff=xmalloc(TFTPPLEN)) == NULL) {
		xinu_fprintf(STDERR, "tftp: out of memory\n");
	    	return(SYSERR);
	}

	/* set up input packet */
	xptr = (struct xgram *)rbuff;
	rtp = (union TFTP *)xptr->xg_data;

	/* build a request (output) packet */
	xptr = (struct xgram *)wbuff;
    blkcopy(xptr->xg_fip, IP, IP_ALEN);
    xptr->xg_fport = UP_TFTP;     /* TFTP request port */
    dgmode = DG_NMODE | DG_TMODE;
    control(dev, DG_SETMODE, &dgmode);
	control(dev, DG_CLEAR);
	wtp = (union TFTP *)xptr->xg_data;
	WTP.opcode = hs2net(opcode);
	rlen = mkreq(wtp, fname, "octet");

	/* proceed with the transfer */
	switch(opcode) {
	case TFTP_WRQ:
		if ((ldev=xinu_fopen(lname, "rbo")) == SYSERR) {
			xinu_fprintf(STDERR, "tftp: %s: file access error\n",
			    lname);
			return(SYSERR);
		}
		if (tftpsend(dev, wbuff, sizeof(short)+rlen,
            rbuff, TFTPPLEN, TFTP_ACK, 0) == SYSERR) {
			xinu_fprintf(STDERR, "tftp: connection error\n");
			return(SYSERR);
		}

		/* copy new faddr:fport info into xinugram write buffer */
		blkcopy(wbuff, rbuff, XGHLEN);

		/* all remaining tftp writes will be DATA packets */
		WTP.opcode = hs2net(TFTP_DATA);
		for (bnum=1 ; ; bnum++) {
			/* read current block from the local file */
			len = xinu_read(ldev, WTP.data, TFTPDLEN);
			if (len == EOF)
				len = 0;
			if (len < 0) {
				xinu_fprintf(STDERR, "tftp: error reading file\n");
				err(TFTP_NOTDEF, "Disk read error.");
				return(SYSERR);
			}
			WTP.blknum = hs2net(bnum);
            if (tftpsend(dev, wbuff, TFTPHLEN+len,
                rbuff, TFTPDLEN, TFTP_ACK, bnum) == SYSERR)
				return(SYSERR);
            if (len < TFTPDLEN)
				return(OK);
		}

	case TFTP_RRQ:
		if ((ldev=xinu_fopen(lname, "wb")) == SYSERR) {
			xinu_fprintf(STDERR, "tftp: %s: file access error\n",
			    lname);
			return(SYSERR);
		}
		if ((len=tftpsend(dev, wbuff, sizeof(short)+rlen,
		    rbuff, TFTPPLEN, TFTP_DATA, 1)) == SYSERR) {
			xinu_fprintf(STDERR, "tftp: connection error\n");
			return(SYSERR);
		}
		/* copy the new faddr:fport info into the write buffer */
		blkcopy(wbuff, rbuff, XGHLEN);

		/* all remaining tftp writes will be ACKs */
		WTP.opcode = hs2net(TFTP_ACK);
		for (bnum=1 ; ; bnum++) {
			/* write current block to the local file */
			len -= TFTPHLEN;	/* skip over the header */
			if (xinu_write(ldev,RTP.data,len) == SYSERR) {
				xinu_fprintf(STDERR, "tftp: error writing file\n");
				err(TFTP_NOTDEF, "Disk write error.");
				return(SYSERR);
			}
			if (len < TFTPDLEN) {
				/* send final ACK */
				WTP.blknum = hs2net(bnum);
				if (xinu_write(dev, wbuff, TFTPHLEN) == SYSERR)
					return(SYSERR);
				return(OK);
			}

			/* send the ack and wait for next data packet */
			WTP.blknum = hs2net(bnum);
			len = tftpsend(dev, wbuff, TFTPHLEN,
			    rbuff, TFTPPLEN, TFTP_DATA, bnum+1);
			if (len == SYSERR) {
				xinu_fprintf(STDERR, "tftp: connection error\n");
				return(SYSERR);
			}
		}
	}
}

/*------------------------------------------------------------------------
 *  mkreq  --  fill in the the filename & mode fields of TFTP packet tp.
 *------------------------------------------------------------------------
 */
LOCAL
mkreq(tp,fname,mode)
union TFTP *tp;
char *fname;			/* foreign filename */
char *mode;			/* mode string */
{
	register char	*cp;

	cp = tp->t_req.file;
	xinu_strcpy(cp,fname);
	cp += xinu_strlen(fname) + 1;
	xinu_strcpy(cp, mode);
	return(xinu_strlen(fname) + xinu_strlen(mode) + 2);
}

/*------------------------------------------------------------------------
 *  usage  --  print a usage message
 *------------------------------------------------------------------------
 */
static usage()
{
	xinu_fprintf(STDERR,
		"usage: tftp {put | get} ipaddr srcname [destname]\n");
}
