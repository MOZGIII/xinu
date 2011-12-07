/* namrepl.c - namrepl */

#include <conf.h>
#include <kernel.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  namrepl  -  using namespace, replace name with (newname,device)
 *------------------------------------------------------------------------
 */
SYSCALL	namrepl(newname, name)
char	*newname;
char	*name;
{
	register struct	nament	*nptr;
	int	plen, rlen;
	int	n;
	int 	ps;

	disable(ps);
	for (n=0, nptr=Nam.nametab ; n<Nam.nnames ; n++,nptr++) {
		plen = xinu_strlen(nptr->npre);
		if (strncmp(nptr->npre, name, plen) == 0) {
			rlen = xinu_strlen(nptr->nrepl);
			if ((rlen+xinu_strlen(name)-plen) >= NAMLEN)
				break;
			xinu_strcpy(newname, nptr->nrepl);
			strcat(newname, name + plen);
			restore(ps);
			return(nptr->ndev);
		}
	}
	xinu_strcpy(newname, "");
	restore(ps);
	return(SYSERR);
}
