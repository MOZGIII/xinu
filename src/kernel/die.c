/* die.c - die */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <dos.h>
#include <io.h>

/*------------------------------------------------------------------------
 * die  --  terminate the current process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL die(pid)
int pid;
{
	struct	pentry	*pptr;
	int	i;
	int 	ps;
	int	nok;
	int	xb;
	XBLOCK	*xbp;
	word	alloc;
	int	bytes;

	disable(ps);
	pptr = &proctab[pid];       /* was currpid */
	pptr->pprio = MAXINT;		/* make sure we go away fast	*/
	pptr->pimmortl = TRUE;		/* disable any further kills	*/

	/* close standard devices */
	for (i=0 ; i<Nsio ; i++)
		xinu_close(pptr->pdevs[i]);

	/* inform next of kin about our death */
	nok = pptr->pnxtkin;
	pptr->pnxtkin = BADPID;
    if ( nok != pid )           /* was currpid */
        send(nok, pid);         /* was currpid */

	/* free allocated memory */
	xb = (int)pptr->pmlist;
	if (xb == 0)
		panic("die: missing stack block");
	xbp = (XBLOCK *)memp(xb);	/* location of stack block */
	xb = (int)xbp->xbback;		/* tail of list */
	xbp->xbback = 0;		/* make sure chase ends! */
	alloc = 0;
	while (xb != 0) {
		xbp = (XBLOCK *)memp(xb);
		bytes = xbp->xblen;
		alloc += bytes;
		xb = (int)xbp->xbback;	/* work backwards to stack block */
		if (freemem((char *)xbp, bytes) == SYSERR)
			panic("die: unable to free allocated memory");
	}
	if (alloc != pptr->pmalloc)
		panic("die: inconsistent memory allocation");
	if ( --numproc == 0 )
		xdone();

	pptr->pstate = PRFREE;	/* suicide */
	resched();
	restore(ps);		/* never gets here */
}
