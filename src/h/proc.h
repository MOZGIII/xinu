/* proc.h - isbadpid */
/* 8086 version */
#include <mem.h>

#ifndef Nsio
#include <sio.h>
#endif

/* process table declarations and defined constants			*/

#ifndef	NPROC				/* set the number of processes	*/
#define	NPROC		30		/*  allowed if not already done	*/
#endif

/* process state constants */

#define PRCURR      '\01'   /* process is currently running */
#define PRFREE      '\02'   /* process slot is free     */
#define PRREADY     '\03'   /* process is on ready queue    */
#define PRRECV      '\04'   /* process waiting for message  */
#define PRSLEEP     '\05'   /* process is sleeping      */
#define PRSUSP      '\06'   /* process is suspended     */
#define PRWAIT      '\07'   /* process is on semaphore queue*/
#define PRTRECV     '\010'  /* proces is timing a receive   */

/* miscellaneous process definitions */

#define	PNMLEN		9		/* length of process "name"	*/
#define	NULLPROC	0		/* id of the null process; it	*/
                            /*  is always eligible to run   */

#define BADPID      (-1)    /* used when invalid pid needed */                  

#define	isbadpid(x)	(x<=0 || x>=NPROC)

/* process table entry */

struct	pentry	{
	char	pstate;			/* process state: PRCURR, etc.	*/
    int pprio;              /* process priority     */
    int psem;               /* semaphore if process waiting */
    int pwaitret;           /* return value from wait   */
    long pmsg;              /* message sent to this process */
    int phasmsg;            /* nonzero iff pmsg is valid    */
    int pimmortl;           /* nonzero iff process immortal */
	char	*pregs;			/* saved environment for ctxsw	*/
	char	*pbase;			/* base of run time stack	*/
	word	plen;			/* stack length	in bytes	*/
	char	*pglob;			/* ptr to private global data	*/
	struct mblock	*pmlist;			/* allocated memory list	*/
	int	pmalloc;		/* bytes of allocated memory	*/
    int ptarg;              /* argument to ptfn     */
    int (*ptfn)();          /* trap function        */
    int phastrap;           /* nonzero iff trap is pending  */
	char	pname[PNMLEN+1];	/* process name			*/
    int pargs;              /* initial number of arguments  */
    int (*paddr)();         /* initial code address     */
	int  	pnxtkin;		/* next-of-kin notified of death*/
	Bool	ptcpumode;		/* proc is in TCP urgent mode	*/
    int     pdevs[Nsio];    /* devices to close upon exit   */
    long    oldtime;        /* time at which resched made it current */
    long    time;           /* CPU time ticks used */
};

extern	struct	pentry proctab[];
extern	int	numproc;		/* currently active processes	*/
extern	int	nextproc;		/* search point for free slot	*/
extern	int	currpid;		/* currently executing process	*/
extern  char    *_pglob;    /* ptr to private global data   */
