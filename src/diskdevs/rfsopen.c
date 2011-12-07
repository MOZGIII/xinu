/* rfsopen.c - rfsopen */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <name.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rfsopen  --  allocate a remote file master device
 *------------------------------------------------------------------------
 */
rfsopen(devptr, ip, rp)
struct devsw *devptr;
IPaddr ip;			/* remote host IP address */
int *rp;			/* pointer to remote port number */
{
	int		i;
	int		slot;
	int		port;
	int		dev;	/* device control block number */
	int		dgmode;
	struct rfmastr	*rfmptr;
	int		ps;

	port = *rp;
	disable(ps);
	slot = -1;
	for (i=0 ; i<Nrfm ; i++)  {
		rfmptr = &Rf[i];
		if (rfmptr->rstatus == RFMCLOSED) {
			slot = i;
			continue;
		}
        if (blkequ(rfmptr->ripaddr,ip,IPMHLEN) && rfmptr->rport==port) {
			/* this server is already open */
			restore(ps);
			return(SYSERR);
		}
	}
	if (slot < 0) {
		/* no slots left to allocate */
		restore(ps);
		return(SYSERR);
	}
	rfmptr = &Rf[slot];
	rfmptr->rstatus = RFMLIMBO;
    blkcopy(rfmptr->ripaddr, ip, IPMHLEN);
	rfmptr->rport = port;
    dev = xinu_open(UDP, ANYFPORT, ANYLPORT);
	if (isbaddev(dev)) {
		rfmptr->rstatus = RFMCLOSED;
		restore(ps);
		return(SYSERR);
	}
	dgmode = DG_DMODE | DG_TMODE;
	control(dev, DG_SETMODE, &dgmode);
	control(dev, DG_SETFPORT, &port);
	control(dev, DG_SETFADDR, ip);
	rfmptr->rnetdev = dev;
	rfmptr->rcount = 0;
	sreset(rfmptr->rmutex, 1);
	rfmptr->rstatus = RFMOPENED;
	restore(ps);
	return(rfmptr->rdev);
}
