/* treceive.c - treceive */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  treceive  --  establish a trap procedure
 *------------------------------------------------------------------------
 */
SYSCALL	treceive(fp)
int	(*fp)();
{
	struct	pentry	*pptr;
	int	ps;

	disable(ps);
	pptr = &proctab[currpid];
	pptr->ptfn = fp;
	pptr->phastrap = FALSE;
	restore(ps);
	return(OK);
}
