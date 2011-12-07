/* printf.c - printf */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 *  printf  --  write formatted output to STDOUT
 *------------------------------------------------------------------------
 */
xinu_printf(fmt,args)
char *fmt;
int args;
{
	int	putc();
	
	_doprnt(fmt, &args, putc, STDOUT);
	return(OK);
}
