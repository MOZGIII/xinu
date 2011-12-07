/* tftperr.c - tftperr */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <tftp.h>
#include <mem.h>

/*------------------------------------------------------------------------
 *  tftperr  --  send a TFTP error packet.
 *------------------------------------------------------------------------
 */
tftperr(dev,xp,errnum,errmsg)
int dev;
struct xgram *xp;
int errnum;
char *errmsg;
{
	char	*buff, *cp;
	union	TFTP	*tp;
	struct	xgram	*xptr;
	int	len = 0;
	
	if (((char *)(buff=xmalloc(sizeof(struct xgram)))) == NULL)
		return(SYSERR);

	blkcopy(buff, xp, XGHLEN);
	xptr = (struct xgram *)buff;

	tp = (union TFTP *)xptr->xg_data;
	tp->t_err.opcode  = hs2net(TFTP_ERR);
	tp->t_err.errcode = hs2net(errnum);
	if (errmsg != NULL) {
		len = xinu_strlen(errmsg);
		if (len >= TFTPDLEN)
			len = TFTPDLEN-1;
		strncpy(tp->t_err.errmsg,errmsg,len);
	} else
		len = 0;
	tp->t_err.errmsg[len] = 0;
	write(dev,buff,TFTPHLEN+len+1);
	xfree(buff);
	return(OK);
}
