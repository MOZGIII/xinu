/* fs_seek.c - fs_seek */

#include <fs.h>

/*------------------------------------------------------------------------
 *  fs_seek - seek in a file given its index in the file cache table
 *------------------------------------------------------------------------
 */
fs_seek(rfd, newpos)
int	rfd;
long	newpos;
{
	if ( (seek(fcache[rfd].fd, newpos)) < 0)
		return(SYSERR);
	fcache[rfd].currpos = newpos;
	return(OK);
}
