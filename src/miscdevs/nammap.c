/* nammap.c - nammap */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  nammap  -  using namespace, iteratively map name onto (newname,device) 
 *------------------------------------------------------------------------
 */
SYSCALL	nammap(newname, name)
char	*name;
char	*newname;
{
	int 	ps,i;
	int	dev;
	char	*tmpnam;

    if ((tmpnam=xmalloc(NAMLEN)) == NULLPTR) {
		return(SYSERR);
	}
	disable(ps);
	dev = namrepl(newname, name);
	while (dev == NAMESPACE) {
		xinu_strcpy(tmpnam, newname);
		dev = namrepl(newname, tmpnam);
	}
	xfree(tmpnam);
	restore(ps);
	return(dev);
}
