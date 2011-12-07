/* x_rm.c - x_rm */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 *  x_rm  --  (command rm) remove a file given its name
 *------------------------------------------------------------------------
 */
COMMAND	x_rm(nargs, args)
int	nargs;
char	*args[];
{
	if ( nargs != 2 ) {
		xinu_fprintf(STDERR,"Usage: rm file\n");
		return(SYSERR);
	}
	if ( remove(args[1]) == SYSERR ) {
		xinu_fprintf(STDERR,"rm: cannot remove %s\n", args[1]);
		return(SYSERR);
	}
	return(OK);
}
