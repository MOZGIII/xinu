/* pglob.c - setpglob, getpglob */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  setpglob  --  set per-process global data structure pointer
 *------------------------------------------------------------------------
 */
SYSCALL
setpglob(pid, memp)
int	pid;
char	*memp;
{
	struct pentry	*pptr;
	int		ps;

	disable(ps);
	if (isbadpid(pid) || (pptr=&proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pglob = memp;
	if (pid == currpid)
		_pglob = memp;
	restore(ps);
	return(OK);
}

/*------------------------------------------------------------------------
 *  getpglob  --  get per-process global data structure pointer
 *------------------------------------------------------------------------
 */
char *
getpglob(pid)
int	pid;
{
	struct pentry	*pptr;
	char		*gp;
	int		ps;

	disable(ps);
	if (isbadpid(pid) || (pptr=&proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(NULLPTR);
	}
	gp = pptr->pglob;
	restore(ps);
	return(gp);
}
