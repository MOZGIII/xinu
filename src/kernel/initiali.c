/* initialize.c - main, sysinit */
int debug_log = 0;
#include <dos.h>
#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <proc.h>
#include <sem.h>
#include <q.h>
#include <shell.h>
#include <butler.h>
#include <pc.h>
#include <kbdio.h>
#include <network.h>
#include <bufpool.h>
#include <sys/time.h>
#include <signal.h>
char vers[]="IBM PC XINU 7.9";

LOCAL sysinit();
#ifdef  Ntty
#include <tty.h>
int winofcur;               /* define the current input window  */
struct tty  tty[Ntty];      /* window buffers and mode control  */
#endif

#ifdef	Ndsk
#include <disk.h>
#endif

#ifdef 	Nmf
#include <mffile.h>
struct mfblk mftab[Nmf];
#endif

/* Declarations of major kernel variables */

struct  pentry  proctab[NPROC];     /* process table            */
int     nextproc;                   /* next process slot to use in create */
struct  sentry  semaph[NSEM];       /* semaphore table          */
int     nextsem;                    /* next semaphore slot to use in screate*/
struct  qent    q[NQENT];           /* q table (see queue.c)        */
int     nextqueue;                  /* next slot in q structure to use  */

MBLOCK  memlist;                    /* list of free memory blocks       */
char *endaddr;              /* beginning of free memory (was para)*/
char *maxaddr;              /* end of free memory (was para)*/

/* PC-specific variables */

int     nmaps;                      /* no. of active intmap entries     */

/* active system state */

int     numproc;                    /* number of live user processes    */
int     currpid;                    /* id of currently running process  */
char    *_pglob;                    /* global environment of current proc.  */

/* real-time clock variables and sleeping process queue pointers	*/

long    tod;                        /* time-of-day (tics since startup) */
long    startup;
int     defclk;                     /* non-zero, then deferring clock count */
int     clkdiff;                    /* deferred clock ticks         */
int     slnempty;                   /* FALSE if the sleep queue is empty    */
int     *sltop;                     /* address of key part of top entry in  */
                                    /* the sleep queue if slnonempty==TRUE  */
int     clockq;                     /* head of queue of sleeping processes  */
int     preempt;                    /* preemption counter.  Current process */
                                    /* is preempted when it reaches zero;   */
                                    /* set in resched; counts in ticks  */

/* local typedef declaration */

typedef	unsigned long	big;

/* miscellaneous system variables */

int     butlerpid;                  /* pid of butler process        */
int     sysmsg;                     /* message queue            */
int     rdyhead,rdytail;            /* head/tail of ready list (q indexes)  */

#define NULLNM  "**NULL**"          /* null process name            */

extern _stklen = 8192;

/************************************************************************/
/***                NOTE:                                             ***/
/***                                                                  ***/
/***   This is where the system begins after the C environment has    ***/
/***   been established.  Interrupts are initially DISABLED, and      ***/
/***   must eventually be enabled explicitly.  This routine turns     ***/
/***   itself into the null process after initialization.  Because    ***/
/***   the null process must always remain ready to run, it cannot    ***/
/***   execute code that might cause it to be suspended, wait for a   ***/
/***   semaphore, or put to sleep, or exit.  In particular, it must   ***/
/***   not do I/O unless it uses kprintf for console output.          ***/
/***                                                                  ***/
/************************************************************************/
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

void int_sig_handler(int sig)
{
    if (sys_get_int_flag())
    {
	cbrkint();
    }	
}

void vt_sig_handler(int sig)
{
    /* Emulation of Keyboard interrupt */
    if (sys_get_int_flag() && kbhit())
    {
        ttyiin(0);
    }
    
    if (sys_get_int_flag())
    {
        clkint(0);
    }
}

/*------------------------------------------------------------------------
 *  main  --  initialize system and become the null process (id==0)
 *------------------------------------------------------------------------
 */
main(argc,argv)                     /* babysit CPU when no one home */
int argc;
char *argv[];
{
    int     xmain();                /* user's main procedure    */
    int     butler();               /* BUTLER process       */
    int     ps;                     /* save processor flags     */
    int     pcx;                    /* reschedule flag      */
    big     maxmem;                 /* maximum memory location  */
    big     endmem;                 /* start of free memory     */
    int     xmainpid;               /* pid of xmain process     */

    while ( kbdgetc() != NOCH )     /* eat remaining kbd chars  */
	;
    init_vidio();
    kprintf("Initializing . . .\n");
    xdisable(pcx);
    disable(ps);
    if ( sysinit() == SYSERR ) {    /* initialize all of PC-Xinu    */
		kprintf("PC-Xinu initialization error\n");
		maprestore();
		restore(ps);
		exit(1);
	}

    restore(ps);
    gettime(&startup);
    kprintf("\nLinXINU Version %s\n", VERSION);
    kprintf("XINU memory starts at: 0x%X\n", endaddr);
    kprintf("XINU memory ends at  : 0x%X\n", maxaddr);
    kprintf("Total amount of XINU memory: %lu bytes\n", maxaddr - endaddr);
    kprintf("\nHit any key to continue . . . ");
    kgetc(0);                       /* wait for keyboard input  */
    scrollup(0,0x184f,0, WB);         /* clear the screen     */
    putcsrpos(0,0);                 /* home the cursor      */
    xrestore(pcx);

    /* start the butler process */
    resume( butlerpid=create(butler,BTLRSTK,BTLRPRIO,BTLRNAME,0) ); 

    /* create the user process */

    xmainpid=create(xmain,INITSTK,INITPRIO,"xmain",3,argc,argv);

#ifdef NETWORK

    /* start the network input daemon process */
    if (argc > 1) {
        resume(xmainpid);
    } else 
        resume(create(netstart, NETSTK, NETPRI, NETNAM, NETARGC, xmainpid));
#else
    resume(xmainpid);
#endif

    while (TRUE)                    /* run forever without actually */
        sys_wait();                    /*  executing instructions  */
}

#define UL	unsigned long

/*------------------------------------------------------------------------
 *  sysinit  --  initialize all PC-Xinu data structures and devices
 *------------------------------------------------------------------------
 */

LOCAL sysinit()
{
	int		i,j;
	struct pentry	*pptr;
	struct sentry	*sptr;
	struct mblock	*mptr;    
	struct sigaction act, oldact;
	struct itimerval ti, oldti;
	
    int     xdone();                /* terminate xinu       */
    char    *namep;                 /* null process name pointer    */
    int    sizmem;                 /* memory sizing        */
    char    *memptr;                /* pointer to memory        */
    struct devsw *dvptr;            /* pointer to devtab entry  */
    word    *vp;                    /* interrupt vector pointer */
    word    vec;                    /* interrupt vector number  */
    unsigned long avmem;

    nmaps=0;                        /* no. of active intmap entries */
    numproc = 0;                    /* initialize system variables  */
	nextproc = NPROC-1;
	nextsem = NSEM-1;
    nextqueue = NPROC;              /* q[0..NPROC-1] are processes  */
    _pglob = NULLPTR;               /* no current environment   */
    
    init_intmap();

    init_keyboard();

    /* Since we don't care about memory under Linux, get 10 MB and be happy */
    avmem = coreleft();                 /* determine bytes of memory left */
    memptr = malloc(avmem);        /* get the lot */
    endaddr = memptr;           /* start of free memory (para)  */
    maxaddr = endaddr + avmem;         /* top of free memory (para)    */

    memlist.mnext = (struct mblock*)endaddr; /* init free memory list  */
    memlist.mlen = avmem;   /* length is in bytes */

    mptr = (MBLOCK *)(endaddr);
	mptr->mnext = 0;
	mptr->mlen = avmem;
    
    for (i=0 ; i<NPROC ; i++)       /* initialize process table */
		proctab[i].pstate = PRFREE;

    pptr = &proctab[NULLPROC];      /* initialize null process entry */
	pptr->pstate = PRCURR;
	pptr->pprio = 0;
    pptr->oldtime = 0;
    pptr->time = 0;
    
#ifndef TURBOC
    sys_stkinit();                  /* disable stack checking   */
#endif
	pptr->pbase = (char*)malloc(2048);             /* null process pbase stack ptr */
	pptr->pregs = (char*)malloc(2048);

	namep=NULLNM;
	for (j=0; j<PNMLEN; j++)
		pptr->pname[j] = (*namep ? *namep++ : ' ');
	pptr->pname[PNMLEN] = '\0';
	pptr->paddr = main;
	pptr->pargs = 0;

	/* STDIN, STDOUT, STDERR */

	for (i=0 ; i < Nsio ; i++)
		pptr->pdevs[i] = (i<3) ? CONSOLE : BADDEV;
	currpid = NULLPROC;

    for (i=0 ; i<NSEM ; i++) {      /* initialize semaphores    */
		(sptr = &semaph[i])->sstate = SFREE;
		sptr->sqtail = 1 + (sptr->sqhead = newqueue());
	}
    rdytail=1+(rdyhead=newqueue()); /* initialize ready list */

#ifdef	MEMMARK
    _mkinit();                      /* initialize memory marking    */
#else
    pinit();                        /* initialize ports     */
    poolinit();                     /* initialize the buffer pools  */
#endif

    clkinit();                      /* initialize real-time clock   */

#ifdef	Ndsk
    dskdbp=mkpool(DBUFSIZ, NDBUFF); /* initialize disk buffers     */
	dskrbp= mkpool(DREQSIZ, NDREQ);
#endif

#ifdef	Ntty
    winofcur = 0;                   /* initialize current window    */
#endif

/* set up low-level interrupt vectors */

    mapinit(DB0VEC, _panic, DB0VEC);        /* divide by zero   */
	mapinit(SSTEPVEC, _panic, SSTEPVEC);	/* single step		*/
    mapinit(BKPTVEC, _panic, BKPTVEC);      /* breakpoint       */
	mapinit(OFLOWVEC, _panic, OFLOWVEC);	/* overflow		*/
    mapinit(CBRKVEC, cbrkint, CBRKVEC);     /* ctrl-break       */

#ifdef	NDEVS
	for ( i=0 ; i<NDEVS ; i++ ) {	/* initialize devices		*/
		dvptr = &devtab[i];
		if ( dvptr->dvivec && mapinit(dvptr->dvivec,
				dvptr->dviint, dvptr->dvminor) == SYSERR )
			goto uninit;
		if (dvptr->dvovec && mapinit(dvptr->dvovec,
				dvptr->dvoint, dvptr->dvminor) == SYSERR )
			goto uninit;
		if (init(i,1) == OK)	/* initialize the device */
			continue;
uninit:
		kprintf("initiali: error initializing device %d\n", i);
		while (--i > 0)
			init(i,0);
		return(SYSERR);
	}
#endif

	if (mapinit(CLKVEC|BIOSFLG, clkint, 0) == SYSERR)
		return(SYSERR);

		
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = vt_sig_handler;	
	sigaddset(&act.sa_mask, SIGIO);
	ti.it_interval.tv_sec = 0;
	ti.it_interval.tv_usec = TICK_FREQ;
	ti.it_value.tv_sec = 0;
	ti.it_value.tv_usec = TICK_FREQ;	
	sigaction(SIGALRM, &act, &oldact);
	act.sa_handler = int_sig_handler;
	sigaction(SIGINT, &act, &oldact);
	
	setitimer(ITIMER_REAL, &ti, &oldti);	
	return(OK);
}
