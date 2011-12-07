/* rmdir.c - rmdir */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <disk.h>
#include <file.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  rmdir  --  remove a directory based on name
 *------------------------------------------------------------------------
 */
SYSCALL	rmdir(name)
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
		ret = control(dev, FLRMDIR, newname);
	xfree(newname);
	return(ret);
}
