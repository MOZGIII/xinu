/* freebuf.c - freebuf */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <bufpool.h>
#include <dos.h>

/*------------------------------------------------------------------------
 *  freebuf  --  free a buffer that was allocated from a pool by getbuf
 *------------------------------------------------------------------------
 */
int freebuf(buf)
char *buf;
{
	int	ps;
	int	poolid;

	if ( buf == NULL ) {
		return(SYSERR);
	}
	disable(ps);
	buf = (char*)((int*)buf - 1);
	poolid = *buf;	
	
	if ( poolid<0 || poolid>=nbpools
#ifdef	MEMMARK
		|| unmarked(bpmark)
#endif
		) {
		restore(ps);
		return(SYSERR);
	}
/*kprintf("                                   freebuf: pid %d pool %d buf %x:%x\n",
        xinu_getpid(),poolid,FP_SEG(buf),FP_OFF(buf));
*/    *((int **) buf) = bptab[poolid].bpnext;
	bptab[poolid].bpnext = (int *) buf;
	xinu_signal(bptab[poolid].bpsem);
	restore(ps);
	return(OK);
}
