/* remove.c - remove */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <disk.h>
#include <file.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  remove  --  remove a file by name
 *------------------------------------------------------------------------
 */
SYSCALL	remove(name)
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
		ret = control(dev, FLREMOVE, newname);
	xfree(newname);
	return(ret);
}
