/* fs_close.c - Close */

#include <fs.h>

/*------------------------------------------------------------------------
 *  Close - close a file given its index in the file cache table
 *------------------------------------------------------------------------
 */
fs_close(rfd)
int	rfd;
{
	if (xinu_close(fcache[rfd].fd) != OK)
		return(SYSERR);
	fcache[rfd].name[0] = NULLCH;
	fcache[fcache[rfd].prev].next = fcache[rfd].next;
	fcache[fcache[rfd].next].prev = fcache[rfd].prev;
	fcache[rfd].prev = NOFILE;
	fcache[rfd].next = freelst;
	freelst = rfd;
	return(OK);
}
