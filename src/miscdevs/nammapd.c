/* nammapd.c - nammapd */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  nammapd  -  using namespace, iteratively map name onto device 
 *------------------------------------------------------------------------
 */
SYSCALL	nammapd(name)
char	*name;
{
	int 	ps,i;
	int	dev;
	char	*newname, *tmpnam;

    if ((tmpnam=xmalloc(NAMLEN)) == NULLPTR)
		return(SYSERR);
    if ((newname=xmalloc(NAMLEN)) == NULLPTR) {
		xfree(tmpnam);
		return(SYSERR);
	}
	disable(ps);
	dev = namrepl(newname, name);
	while (dev == NAMESPACE) {
		xinu_strcpy(tmpnam, newname);
		dev = namrepl(newname, tmpnam);
	}
	xfree(newname);
	xfree(tmpnam);
	restore(ps);
	return(dev);
}
