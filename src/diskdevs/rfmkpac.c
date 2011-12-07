/* rfmkpac.c - rfmkpac */

#include <conf.h>
#include <kernel.h>
#include <network.h>

static	struct	fpacket	packet;

extern long net2hl(long);
extern long hl2net(long);

/*------------------------------------------------------------------------
 *  rfmkpac  --  make a remote file request packet and send it
 *------------------------------------------------------------------------
 */
rfmkpac(rop, rname, rpos, buff, len, rfmptr)
int	rop;
char	*rname;
long	*rpos;
char	*buff;
int	len;
struct	rfmastr	*rfmptr;
{
	struct	fphdr	*fptr;
	int	reqlen, rplylen;
	int	ps;

	disable(ps);
	if (rfmptr->rstatus != RFMOPENED || wait(rfmptr->rmutex) != OK){
		restore(ps);
		return(SYSERR);
	}
	fptr = (struct fphdr *) &rfmptr->rpacket;
	fptr->f_op = hs2net(rop);
	fptr->f_pos = hl2net(*rpos);
	fptr->f_count = hs2net(len);
	strncpy(fptr->f_name, rname, RNAMLEN);
	reqlen = rplylen = FPHLEN + len;
	switch (rop) {

	    case FS_WRITE:
	    case FS_RENAME:
		if (len > RDATLEN) {
			xinu_signal(rfmptr->rmutex);
			restore(ps);
			return(SYSERR);
		}
		blkcopy(rfmptr->rpacket.fpdata, buff, len);
		rplylen = FPHLEN;
		break;

	    case FS_CLOSE:
	    case FS_OPEN:
	    case FS_REMOVE:
	    case FS_MKDIR:
	    case FS_RMDIR:
	    case FS_ACCESS:
		rplylen = FPHLEN;
		/* fall through */
		
	    case FS_READ:
		if (len > RDATLEN) {
			xinu_signal(rfmptr->rmutex);
			restore(ps);
			return(SYSERR);
		}
		reqlen = FPHLEN;
		break;

	    default:
	    	;
	}
	if (rfsend(fptr, reqlen, rplylen, rfmptr->rnetdev) == SYSERR ||
	    net2hs(fptr->f_op) == FS_ERROR) {
		xinu_signal(rfmptr->rmutex);
		restore(ps);
		return(SYSERR);
	}
	switch (rop) {

	    case FS_READ:
		blkcopy(buff, rfmptr->rpacket.fpdata, len);
		/* fall through */

	    case FS_WRITE:
		*rpos = net2hl(fptr->f_pos);
		len = net2hs(fptr->f_count);
		break;

	    default:
		len = OK;
	}
	xinu_signal(rfmptr->rmutex);
	restore(ps);
	return(len);
}
