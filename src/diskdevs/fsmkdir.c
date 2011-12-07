/* fsmkdir.c - fsmkdir */

#include <fs.h>

/*------------------------------------------------------------------------
 *  fsmkdir  -  handle mkdir request and prepare reply
 *------------------------------------------------------------------------
 */
fsmkdir(fpacptr, flen)
struct	fpacket	*fpacptr;
int	flen;
{
	struct	fphdr	*fptr;

	fptr = &fpacptr->fp_h;
	if (mkdir(fptr->f_name) < 0)
		fptr->f_op = FS_ERROR;
	else
		fs_del(fptr->f_name);
	return(FHDRLEN);
}
