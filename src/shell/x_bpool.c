/* x_bpool.c - x_bpool */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  x_bpool  -  (command bpool) format and print buffer pool information
 *------------------------------------------------------------------------
 */
COMMAND	x_bpool(nargs, args)
int	nargs;
char	*args[];
{
	struct	bpool	*bpptr;
	int	i;

	if ( nargs > 1 ) {
		xinu_fprintf(STDERR,"Usage: bpool\n");
		return(SYSERR);
	}
	for (i=0 ; i<nbpools ; i++) {
		bpptr = &bptab[i];
		xinu_printf("pool=%2d. bsize=%4d, sem=%2d, count=%d\n",
			i, bpptr->bpsize, bpptr->bpsem,scount(bpptr->bpsem));
	}
	return(OK);
}
