/* x_date.c - x_date */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <sleep.h>

/*------------------------------------------------------------------------
 *  x_date  -  (command date) print the date and time
 *------------------------------------------------------------------------
 */
COMMAND	x_date(nargs, args)
int	nargs;
char	*args[];
{
	char	*str;
	long	now;

	if ( nargs != 1 ) {
		xinu_fprintf(STDERR,"Usage: %s\n",args[0]);
		return(SYSERR);
	}
	if ((str=xmalloc(128)) == NULL) {
		xinu_fprintf(STDERR,"date: out of memory\n");
		return(SYSERR);
	}
	gettime(&now);
	ascdate(now, str);
	xinu_write(STDOUT, str, xinu_strlen(str));
	xinu_fprintf(STDOUT,"\n");
	return(OK);
}
