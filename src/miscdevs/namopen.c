/* namopen.c - namopen */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  namopen  -  open an object (e.g., remote file) based on the name
 *------------------------------------------------------------------------
 */
namopen(devptr, filenam, mode)
struct	devsw	*devptr;
char	*filenam;
char	*mode;
{
	int	dev;
	int	ret;
	char	*newname;

    if ((newname=xmalloc(NAMLEN)) == NULLPTR)
		return(SYSERR);
	if ( (dev=nammap(newname, filenam)) == SYSERR) {
		xfree(newname);
        return(SYSERR);
	}
    ret = nopen(dev, newname, mode);
	xfree(newname);
	return(ret);
}
