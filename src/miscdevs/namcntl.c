/* namcntl.c - namcntl */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  namcntl  -  control an object based on the name
 *------------------------------------------------------------------------
 */
namcntl(devptr, op, filenam, addr2)
struct	devsw	*devptr;
int op;
char *filenam;
char *addr2;
{
	int	dev;
	int	ret;
	char	*newname;

    if ((newname=xmalloc(NAMLEN)) == NULLPTR)
		return(SYSERR);
	if ((dev=nammap(newname, filenam)) == SYSERR) {
		xfree(newname);
		return(SYSERR);
	}
	ret = control(dev, op, newname, addr2);
	xfree(newname);
	return(ret);
}
