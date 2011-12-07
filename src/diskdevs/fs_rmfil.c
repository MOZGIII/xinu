/* fs_rmfil.c - fs_rmfil */

#include <fs.h>
#include <string.h>

/*------------------------------------------------------------------------
 *  fs_rmfil  -  remove a file given its name, & delete parent directories
 *------------------------------------------------------------------------
 */
fs_rmfil(name)
char	*name;
{
	int	dev;
	int	key = 0;
	char 	*subdir;

	if (remove(name) != OK)
		return(SYSERR);
	rmdirs(name);
	return(OK);
}
