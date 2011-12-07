/* fsrename.c - fsrename */

#include <fs.h>

/*------------------------------------------------------------------------
 *  fsrename  -  handle rename request and prepare reply
 *------------------------------------------------------------------------
 */
fsrename(fpacptr, flen)
struct	fpacket	*fpacptr;
int	flen;
{
	struct	fphdr	*fptr;

	fptr = &fpacptr->fp_h;
	fs_del(fptr->f_name);
	fs_del(fpacptr->fpdata);
	if (rename(fptr->f_name, fpacptr->fpdata) < 0) {
		fptr->f_op = FS_ERROR;
		return(FHDRLEN);
	}
	return(flen);
}
