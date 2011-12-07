/* create.c - create, newpid */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <proc.h>
#include <dos.h>
#include <mem.h>

typedef int (*fptr)();

#define	INITF	0x0200	/* initial flag register - set interrupt flag,	*/
                        /* clear direction and trap flags       */
#define INITPCX 1       /* set pcxflag                  */

extern	int	INITRET();	/* location to return upon termination	*/
LOCAL	newpid();

/*------------------------------------------------------------------------
 *  create  --  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL create(procaddr,ssize,priority,namep,nargs,args)
fptr procaddr;			/* procedure address			*/
word ssize;             /* stack size in bytes          */
short priority;			/* process priority > 0			*/
char *namep;			/* name (for debugging)			*/
int nargs;              /* number of args that follow       */
int args;               /* arguments (treated like an array)    */
{
	int	pid;			/* stores new process id	*/
	struct	pentry	*pptr;		/* pointer to proc. table entry */
        int i;              /* loop variable        */
	int	*currdevs;		/* current device table		*/
	int	dev;			/* device number for stdio	*/
	int *a;             /* points to list of args   */
        char    *saddr;     /* start of stack address   */
	int	*sp;			/* stack pointer		*/
    int ps;             /* saved processor status   */
    XBLOCK  *xbp;       /* stack adjustment     */
	int	die();			/* execute upon process term.	*/

	disable(ps);
	if (ssize < MINSTK)
		ssize = MINSTK;
    if (priority < 1 || (pid=newpid()) == SYSERR ||
        ((saddr=getstk(ssize)) == NULL ) ) {
		restore(ps);
        return(SYSERR);
	}
	numproc++;
	pptr = &proctab[pid];
	pptr->pstate = PRSUSP;
	pptr->pimmortl = TRUE;		/* just for the time being...	*/
	for (i=0 ; i<PNMLEN ; i++)
		pptr->pname[i] = (*namep ? *namep++ : ' ');
	pptr->pname[PNMLEN]='\0';
	pptr->pprio = priority;
	pptr->pwaitret = 0;
	pptr->phasmsg = FALSE;		/* no message			*/
	pptr->pimmortl = FALSE;		/* not immortal			*/
	pptr->pnxtkin = BADPID;		/* no next-of-kin		*/
	pptr->pglob = NULLPTR;		/* no global environment	*/
	pptr->pbase = saddr;
	pptr->plen = ssize;
	pptr->pmlist = (struct mblock*)saddr;	/* stack is only allocated mem.	*/
	xbp = (XBLOCK *)saddr;
	xbp->xbback = (struct xblock*)pptr->pmlist;	/* points to tail of list	*/
	xbp->xbfore = (struct xblock*)pptr->pmlist;	/* circular list		*/
	xbp->xblen = ssize;
	pptr->pmalloc = ssize;		/* no. of allocated bytes	*/
    pptr->ptarg = 0L;
    pptr->oldtime = 0L;          /* resched hasn't run it yet */
    pptr->time = 0L;             /* clear CPU time used */
    pptr->ptfn = die;           /* initialize trap to die   */
	pptr->phastrap = FALSE;		/* no trap yet			*/
	currdevs = proctab[currpid].pdevs;	/* parent device table	*/
	/* new process inherits open I/O devices of parent */
	for (i=0 ; i<Nsio ; i++) {
        dev = xinu_open(currdevs[i], NULLPTR);
        if (isbaddev(dev)) 
			pptr->pdevs[i] = BADDEV;
		else
            pptr->pdevs[i] = dev; 
	}
	pptr->pimmortl = FALSE;		/* back to normal		*/
	sp = (int *)(saddr+ssize);	/* simulate stack pointer	*/
        sp -= 4;                    /* a little elbow room      */
	pptr->pargs = nargs;
	a = (&args) + nargs;		/* point past last argument	*/
	for ( ; nargs > 0 ; nargs--)	/* machine dependent; copy args	*/
    	    *(--sp) = *(--a);           /*  onto created process' stack */	
	sp = (int*)((fptr*)sp - 1);
	*sp = (int)INITRET;
	sp = (int*)((fptr*)sp - 1);
	*sp = (int)procaddr;
	--sp ;                      /* 1 word for bp        */
	*(--sp) = INITF;            /* FLAGS value          */
	sp -= 2;                    /* 2 words for si and di    */
	pptr->pregs = (char *)sp;	/* save for context switch	*/
	pptr->paddr = procaddr;
	restore(ps);
	return(pid);
}

/*------------------------------------------------------------------------
 *  newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
    int pid;                    /* process id to return     */
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
