/* x_int.c  -  x_int */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

extern long startup;

/*------------------------------------------------------------------------
 *  x_int  --  (command int) print time spent handling interrupts
 *------------------------------------------------------------------------
 */
COMMAND  x_int(nargs,args)
int  nargs;
char *args[];
{
    int     ps,i;
    long    ptime = 0;
    long    now;
	struct	pentry *pptr;

    disable(ps);
	for ( i=0; i<NPROC; i++ ) {
		pptr = &proctab[i];
/*        if (pptr->pstate != PRFREE) */
            ptime = ptime + pptr->time;               
	}
    restore(ps);
    gettime(&now);
    xinu_printf("%D secs elapsed\n",now-startup);
    xinu_printf("%D secs processing\n",ptime*10/182);
    xinu_printf("%D secs spent on interrupts\n", (now-startup)-ptime*10/182);
    return OK;
}
