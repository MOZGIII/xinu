/* rmdirs.c - rmdirs */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <disk.h>
#include <file.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  rmdirs  --  remove a directory path based on name
 *------------------------------------------------------------------------
 */
SYSCALL	rmdirs(name)
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
		ret = control(dev, FLRMDIRS, newname);
	xfree(newname);
	return(ret);
}
