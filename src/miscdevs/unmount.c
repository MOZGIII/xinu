/* unmount.c - unmount */

#include <conf.h>
#include <kernel.h>
#include <name.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  unmount  -  remove an entry from the name prefix mapping table
 *------------------------------------------------------------------------
 */
SYSCALL	unmount(prefix)
char	*prefix;
{
	struct	nament	*nptr;
	int 	ps;
	int	i;

    if (prefix == NULLPTR)
		prefix = NULLSTR;
	else if (xinu_strlen(prefix) >= NAMPLEN)
		return(SYSERR);
	disable(ps);
	for (i=0 ; i<Nam.nnames ; i++) {
		nptr = &Nam.nametab[i];
		if (strcmp(prefix, nptr->npre) == 0) {
			if (nptr->ntyp == REMOTE
			    && xinu_close(nptr->ndev) == SYSERR) {
				restore(ps);
				return(SYSERR);
			}
			for(Nam.nnames-- ; i<Nam.nnames ; i++)
				Nam.nametab[i] = Nam.nametab[i+1];
			restore(ps);
			return(OK);
		}
	}
	restore(ps);
	return(SYSERR);
}
