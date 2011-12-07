/* fs_getrq.c - fs_getrq */

#include <fs.h>

extern long net2hl(long);

/*------------------------------------------------------------------------
 *  fs_getrq  -  read a request packet from network and return its length
 *------------------------------------------------------------------------
 */
fs_getrq(dgd, xgp)
int	dgd;
struct	xgram	*xgp;
{
	int	len;
	struct	fphdr	*fptr;
	struct	fpacket *fpacptr;

	fpacptr = (struct fpacket *)(xgp->xg_data);
	fptr = &fpacptr->fp_h;
	while ((len=read(dgd, xgp, sizeof(struct fpacket))) < 0)
		if (len == SYSERR)
			return(SYSERR);

	/* Convert from network byte order to host byte order */

	fptr->f_pos = net2hl(fptr->f_pos);
	fptr->f_op = net2hs(fptr->f_op);
	fptr->f_count = net2hs(fptr->f_count);

	return(len);
}
