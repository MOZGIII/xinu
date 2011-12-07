/* mkdirs.c - mkdirs */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <disk.h>
#include <file.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  mkdirs  --  make a directory path based on name
 *------------------------------------------------------------------------
 */
SYSCALL	mkdirs(name)
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
		ret = control(dev, FLMKDIRS, newname);
	xfree(newname);
	return(ret);
}
