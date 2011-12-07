/* fs_del.c - fs_del */

#include <fs.h>

/*------------------------------------------------------------------------
 *  fs_del - delete the cached entry for a file and close the file
 *------------------------------------------------------------------------
 */
fs_del(name)
char	*name;
{
	int	rfd;

	for(rfd=0 ; rfd<MAXNFILES ; rfd++) {
		if (strcmp(name, fcache[rfd].name) == 0) {
			fs_close(rfd);
			return(OK);
		}
	}
	return(SYSERR);
}
