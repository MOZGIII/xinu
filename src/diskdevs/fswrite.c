/* fswrite.c - fswrite */

#include <fs.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 *  fswrite  -  handle write request and prepare reply
 *------------------------------------------------------------------------
 */
 
fswrite(fpacptr, flen)
struct	fpacket	*fpacptr;
int	flen;
{
	struct	fphdr	*fptr;
	int	rfd;

	fptr = &fpacptr->fp_h;
	if ((rfd=fs_open(fptr->f_name, FLOLD|FLWRITE)) < 0
	    || fs_seek(rfd, fptr->f_pos) < 0
	    || (fptr->f_count=write(fcache[rfd].fd,
		    fpacptr->fpdata, fptr->f_count)) < 0) {
		fptr->f_op = FS_ERROR;
	} else
		fcache[rfd].currpos = fptr->f_pos += fptr->f_count;
	return(FHDRLEN);
}
