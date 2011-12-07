/* fs_sndrp.c - fs_sndrp */

#include <fs.h>

extern long hl2net(long);

/*------------------------------------------------------------------------
 *  fs_sndrp  -  send a reply packet to a  specified foreign address
 *------------------------------------------------------------------------
 */
 
fs_sndrp(dgd, xgp, len)
int	dgd;				/* socket on which to write	*/
struct	xgram	*xgp;			/* xinugram pointer		*/
int	len;				/* length of data to write	*/
{
	struct	fphdr	*fptr;
	struct	fpacket *fpacptr;

	fpacptr = (struct fpacket *)(xgp->xg_data);
	fptr = &fpacptr->fp_h;
	/* Convert inetgers from host byte order to network byte order	*/
	fptr->f_op = hs2net(fptr->f_op);
	fptr->f_pos = hl2net(fptr->f_pos);
	fptr->f_count = hs2net(fptr->f_count);

	if (xinu_write(dgd, xgp, len) < 0)
		return(SYSERR);
	return(OK);
}
