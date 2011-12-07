/* access.c - access */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <disk.h>
#include <file.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  access  --  determine accessability given file name and desired mode
 *------------------------------------------------------------------------
 */
SYSCALL	access(name, mode)
char	*name;
int	mode;
{
	char	*newname;
	int	dev;
	int	ret;

	if ((newname=xmalloc(NAMLEN)) == NULL)
		return(SYSERR);
	if ((dev=nammap(newname, name)) == SYSERR)
		ret = SYSERR;
	else
		ret = control(dev, FLACCESS, newname, mode);
	xfree(newname);
	return(ret);
}
