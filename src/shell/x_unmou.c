/* x_unmou.c -  x_unmou */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  x_unmou  --  (command unmount) remove a prefix from the namespace table
 *------------------------------------------------------------------------
 */
COMMAND	x_unmou(nargs, args)
int	nargs;
char	*args[];
{
	if (nargs != 2) {
		xinu_fprintf(STDERR, "Usage: unmount name\n");
		return(SYSERR);
	}
	if (unmount(args[1]) == SYSERR) {
		xinu_fprintf(STDERR, "unmount: failed to unmount %s\n",args[1]);
		return(SYSERR);
	}
	return(OK);
}
