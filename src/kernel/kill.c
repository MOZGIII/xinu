/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <dos.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(pid)
int	pid;			/* process to kill			*/
{
	struct	pentry	*pptr;
	int 	ps;
	int	die();

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (pid == currpid)
        die(pid);
	if (pptr->pimmortl) {
		send(pid, TMSGKILL);
		restore(ps);
		return(0);
	}
	switch (pptr->pstate) {
	case PRWAIT:
		semaph[pptr->psem].semcnt++;
		/* fall through */
	case PRREADY:
		dequeue(pid);
		break;

	case PRSLEEP:
	case PRTRECV:
		unsleep(pid);
		break;

	default:
		break;
	}
	pptr->pprio = MAXINT;
	pptr->pstate = PRREADY;
	pptr->ptfn = die;
    pptr->ptarg = pid;
	pptr->phastrap = TRUE;
	insert(pid,rdyhead,MAXINT);
	resched();
	restore(ps);
	return(OK);
}
