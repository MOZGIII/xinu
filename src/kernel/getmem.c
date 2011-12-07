/* getmem.c - getmem & normal */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <dos.h>

/*------------------------------------------------------------------------
 *  getmem  --  allocate heap storage, returning lowest integer address
 *------------------------------------------------------------------------
 */
char *getmem(nbytes)
word nbytes;
{
	int	ps;
	char	*pp, *qp, *leftp;
	/*struct mblock*/char *p, *q, *left;
	word	alloc, nalloc;

	alloc = roundew(nbytes);
	if ( alloc == 0 )
    	    return(NULLPTR);
	disable(ps);
	
	qp = (char *) &memlist;
	for (;;) {
		p = (char*)memnext(qp);
		if ( p == 0 ) 
		{
		    restore(ps);
        	    return(NULLPTR);
		}
		
		pp = (char*)p;
		
		if ( memlen(pp) < alloc ) {
			qp = pp;
			continue;
		}
		nalloc = memlen(pp) - alloc;
		if ( nalloc == 0 ) { 	/* block is an exact match	*/
			left = (char*)memnext(pp);
			break;
		}
		/* must break up block */
		left = p + alloc;
		leftp = memp(left);
		memnext(leftp) = memnext(pp);
		memlen(leftp) = nalloc;
		break;
	}
	memnext(qp) = (struct mblock*)left;
	memlist.mlen -= alloc;
	restore(ps);
    return(pp);
}
