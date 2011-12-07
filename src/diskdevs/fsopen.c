/* fsopen.c - fsopen */

#include <fs.h>

/*------------------------------------------------------------------------
 *  fsopen  -  handle open request and prepare reply
 *------------------------------------------------------------------------
 */
 
fsopen(fpacptr, flen)
struct	fpacket	*fpacptr;
int	flen;
{
	struct	fphdr	*fptr;
	int 	rfd;
	
	fptr = &fpacptr->fp_h;
	if ((rfd=fs_open(fptr->f_name, fptr->f_count)) < 0 )
 		fptr->f_op = FS_ERROR;
 	else
 		fptr->f_pos = 0L;
	return(FHDRLEN);
}
