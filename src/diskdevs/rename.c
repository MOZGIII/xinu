/* rename.c - rename */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <disk.h>
#include <file.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  rename  -  rename a file
 *------------------------------------------------------------------------
 */
SYSCALL	rename(old, new)
char	*old;
char	*new;
{
	char	*oldname;
	char	*newname;
	int	dev1, dev2;
	int	ret;

	/* map names through namespace and restrict to single device */

	if ((oldname=xmalloc(NAMLEN)) == NULL)
		return(SYSERR);
	if ((newname=xmalloc(NAMLEN)) == NULL) {
		xfree(oldname);
		return(SYSERR);
	}
	if ((dev1 = nammap(oldname, old)) == SYSERR ||
	    (dev2 = nammap(newname, new)) == SYSERR ||
	    dev1 != dev2)
		ret = SYSERR;
	else
		ret = control(dev1, FLRENAME, oldname, newname);
	xfree(oldname);
	xfree(newname);
	return(ret);
}
