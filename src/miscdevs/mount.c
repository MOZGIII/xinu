/* mount.c - mount */

#include <conf.h>
#include <kernel.h>
#include <name.h>


/*------------------------------------------------------------------------
 *  mount  -  give meaning to a prefix in the abstract name space
 *------------------------------------------------------------------------
 */
SYSCALL	mount(prefix, dev, replace, typ)
char	*prefix;
char	*replace;
int	dev;
int	typ;
{
	struct	nament	*nptr;
	struct	nament	*last;
	int	i;
	int 	ps;

    if (prefix == NULLPTR)
		prefix = NULLSTR;
    if (replace == NULLPTR)
		replace = NULLSTR;
	if (xinu_strlen(prefix) >= NAMPLEN || xinu_strlen(replace) >= NAMRLEN)
		return(SYSERR);
	disable(ps);
	for (i=0 ; i<Nam.nnames ; i++) {
		nptr = &Nam.nametab[i];
		if (strcmp(prefix,nptr->npre) != 0)
			continue;
		if (nptr->ntyp == REMOTE) {
			restore(ps);
			return(SYSERR);
		}
		xinu_strcpy(nptr->nrepl, replace);
		nptr->ndev = dev;
		nptr->ntyp = typ;
		restore(ps);
		return(OK);
	}
	if (Nam.nnames >= NNAMES) {
		restore(ps);
		return(SYSERR);
	}
	nptr = last = &Nam.nametab[Nam.nnames++];
	if (Nam.nnames > 1) {	/* preserve last name prefix */
		nptr = last - 1;
		*last = *nptr;
	}
	xinu_strcpy(nptr->npre, prefix);
	xinu_strcpy(nptr->nrepl, replace);
	nptr->ndev = dev;
	nptr->ntyp = typ;
	restore(ps);
	return(OK);
}
