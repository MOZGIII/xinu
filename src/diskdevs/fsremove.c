/* fsremove.c - fsremove */

#include <fs.h>

/*------------------------------------------------------------------------
 *  fsremove  -  handle remove request and prepare reply
 *------------------------------------------------------------------------
 */
fsremove(fpacptr, flen)
struct	fpacket	*fpacptr;
int	flen;
{
	struct	fphdr	*fptr;
	int	rfd;

	fptr = &fpacptr->fp_h;
	if (fs_rmfil(fptr->f_name) < 0)
		fptr->f_op = FS_ERROR;
	else
		fs_del(fptr->f_name);
	return(flen);
}
