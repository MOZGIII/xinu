/* getnok.c - getnok */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  getnok  --  get next-of-kin of given process
 *------------------------------------------------------------------------
 */
SYSCALL	getnok(pid)
int	pid;
{
	int 	ps;
	int	nok;
	struct	pentry	*pptr;

	if (isbadpid(pid))
		return(SYSERR);
	disable(ps);
	pptr = &proctab[pid];
	nok = pptr->pnxtkin;
	restore(ps);
	return(nok);
}
