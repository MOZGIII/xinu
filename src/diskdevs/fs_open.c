/* fs_open.c - fs_open */

#include <fs.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 *  fs_open - open a file given its name and Xinu mode; return cache index
 *------------------------------------------------------------------------
 */
fs_open(name, xmode)
char	*name;
int	xmode;
{
	int	rfd;

	if (*name == NULLCH)
    		return(SYSERR);
	for (rfd = 0; rfd < MAXNFILES; ++rfd)
		if (strcmp(fcache[rfd].name, name) == 0)
			break;
	if (rfd < MAXNFILES
	    && (fcache[rfd].mode & xmode & FLRW) == (xmode&FLRW)) {
		/* file already opened appropriately	*/
		fcache[fcache[rfd].prev].next = fcache[rfd].next;
		fcache[fcache[rfd].next].prev = fcache[rfd].prev;
	} else {
		if (rfd < MAXNFILES)	/* file must be reopened	*/
			fs_close(rfd);
		if (freelst == NOFILE)
			fs_close(OLDEST);
		rfd = freelst;
		if ((fcache[rfd].fd = fs_openf(name, xmode)) < 0) 
			return(SYSERR);
		if (seek(fcache[rfd].fd,0L) < 0) {
			xinu_close(fcache[rfd].fd);
			return(SYSERR);
		}
		fcache[rfd].currpos = 0L;
		xinu_strcpy(fcache[rfd].name, name);
		fcache[rfd].mode  = xmode;
		freelst = fcache[freelst].next;
	}
	fcache[rfd].prev = NEWEST;
	fcache[rfd].next = NOFILE;
	fcache[NEWEST].next = rfd;
	NEWEST = rfd;
	return(rfd);
}
