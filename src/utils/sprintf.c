/* sprintf.c - sprintf */

#include <conf.h>
#include <kernel.h>

static putbuf();

/*------------------------------------------------------------------------
 *  sprintf  --  write formatted output to a buffer
 *------------------------------------------------------------------------
 */
xinu_sprintf(buf,fmt,args)
char *buf;
char *fmt;
int args;
{
	int	putbuf();
	
	_doprnt(fmt, &args, putbuf, -1, buf);
	return(OK);
}

static int putbuf(n, ch, cp)
int n;					/* dummy parameter		*/
char ch;
char *cp;
{
	*cp = ch;
}
