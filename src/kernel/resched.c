/* resched.c - resched */
extern int debug_log;
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <dos.h>
#include <q.h>
#include <sleep.h>
#include <fcntl.h>
/*------------------------------------------------------------------------
 *  resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRCURR.
 *------------------------------------------------------------------------
 */
int	resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	register int pflag;
	char *oldStackPtr = NULL;
    proctab[currpid].time = proctab[currpid].time + tod
                            - proctab[currpid].oldtime;

    optr = &proctab[currpid];
    pflag = sys_pcxget();
    if ( optr->pstate == PRCURR ) {
		/* no switch needed if current prio. higher than next	*/
		/* or if rescheduling is disabled ( pflag == 0 )	*/
		if ( pflag == 0 || lastkey(rdytail) < optr->pprio )
			return;
		/* force context switch */
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	} else if ( pflag == 0 ) {
		kprintf("pid=%d state=%d name=%s",
			currpid,optr->pstate,optr->pname);
		panic("Reschedule impossible in this state");
	}

	/* remove highest priority process at end of ready list */

	nptr = &proctab[(currpid=getlast(rdytail))];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
	preempt = QUANTUM;          /* reset preemption counter */
	_pglob = nptr->pglob;		/* retrieve global environment	*/

        proctab[currpid].oldtime=tod;
	if (!optr->pregs || !nptr->pregs)
	{
	    kprintf("Rescheduling failed: pregs = 0\n");
	    return;
	}
/*	if (!debug_log)
	    debug_log = xinu_open("resched.log", O_RDWR|O_CREAT|O_TRUNC);
	dprintf(1, "Old: %s, New: %s\n", optr->pname, nptr->pname);*/
	oldStackPtr = optr->pregs;
	ctxsw(&optr->pregs,&nptr->pregs);

	if (currpid != 0 && optr->pregs > optr->pbase + optr->plen)
		panic("stack overflow");
		
	if (optr->phastrap) {
		optr->phastrap = FALSE;	/* mark trap as serviced	*/
        if (optr->ptfn != NULL)
			(*optr->ptfn)(optr->ptarg);
	}

	/* The OLD process returns here when resumed. */
	return;
}
