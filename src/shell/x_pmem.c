/* x_pmem.c  -  x_pmem */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <mem.h>
#include <dos.h>

/*------------------------------------------------------------------------
 *  x_pmem  --  (command pmem) print process's memory statistics
 *------------------------------------------------------------------------
 */
COMMAND  x_pmem(nargs,args)
int  nargs;
char *args[];
{
    struct pentry    *pptr;
	int		xm, xb, xf;
	XBLOCK		*xbp, *xfp;
    int n, total, ps, pid;

    if ( nargs != 2 ) {
        xinu_fprintf(STDERR,"Usage: pmem pid\n");
		return(SYSERR);
	}
    pid = atoi(args[1]);
    if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
        return SYSERR;
	}
	pptr = &proctab[pid];
    xb = (int)pptr->pmlist;
    if (xb == 0) {
        xinu_printf("pmem: missing stack block [%d]",pid);
        return SYSERR;
    }
    xbp = (XBLOCK *)memp(xb);
    n = total = 0;
    while (TRUE) {
        xf = (int)xbp->xbfore;
        if (xf == 0) {
            xinu_printf("pmem: invalid memory list [%d]",pid);
            return SYSERR;
        }
        n += 1;
        total += xbp->xblen*16;
        xinu_printf("%4x %6d\n", xbp->xbfore, xbp->xblen*16);
        if ((int)xbp->xbfore == xb) break;
        xbp = (XBLOCK *)memp(xf);
    }    
    xinu_printf("blocks: %d total: %d\n", n, total);
	return(OK);
}
