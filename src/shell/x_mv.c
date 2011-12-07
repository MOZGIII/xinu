/* x_mv.c - x_mv */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 *  x_mv  --  (command mv) move (rename) a file
 *------------------------------------------------------------------------
 */
COMMAND	x_mv(nargs, args)
int	nargs;
char	*args[];
{
	if ( nargs != 3 ) {
		xinu_fprintf(STDERR, "Usage: mv file tofile\n");
		return(SYSERR);
	}
	if ( rename(args[1],args[2]) == SYSERR ) {
		xinu_fprintf(STDERR,"mv: cannot move %s\n",args[1]);
		return(SYSERR);
	}
	return(OK);
}
