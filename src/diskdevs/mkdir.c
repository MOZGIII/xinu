/* mkdir.c - mkdir */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <disk.h>
#include <file.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  mkdir  --  make a directory based on name
 *------------------------------------------------------------------------
 */
SYSCALL	mkdir(name)
char	*name;
{
	char	*newname;
	int	dev;
	int	ret;

	if ((newname=xmalloc(NAMLEN)) == NULL)
		return(SYSERR);
	if ((dev=nammap(newname, name)) == SYSERR)
		ret = SYSERR;
	else
		ret = control(dev, FLMKDIR, newname);
	xfree(newname);
	return(ret);
}
