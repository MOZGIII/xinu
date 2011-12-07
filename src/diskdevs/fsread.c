/* fsread.c - fsread */

#include <fs.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 *  fsread  -  handle read request and prepare reply
 *------------------------------------------------------------------------
 */
fsread(fpacptr, flen)
struct	fpacket	*fpacptr;
int	flen;
{
	struct	fphdr	*fptr;
	int	rfd;
	int	n;

	fptr = &fpacptr->fp_h;
	if ((rfd=fs_open(fptr->f_name, FLOLD|FLREAD)) < 0
	    || fs_seek(rfd, fptr->f_pos) < 0
	    || (n=read(fcache[rfd].fd, fpacptr->fpdata,
		    fptr->f_count)) == SYSERR) {
		fptr->f_op = FS_ERROR;
		return(FHDRLEN);
	}
	if (n == EOF)
		n = 0;		/* for UNIX server compatibility */
	fcache[rfd].currpos = fptr->f_pos += fptr->f_count = n;
	return(n + FHDRLEN);
}
