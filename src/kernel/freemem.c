/* freemem.c - freemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <dos.h>

/*------------------------------------------------------------------------
 *  freemem  --  free a memory block, returning it to memlist
 *------------------------------------------------------------------------
 */
SYSCALL freemem(bp, size)
char *bp;
word size;
{
	int	ps;
	char	*pp, *qp;
	char	*b, *p, *q, *top;
	int retsize;

	retsize = roundew(size);
	if ( retsize == 0 || bp == NULL/* || off(bp) != 0 */)
		return(SYSERR);
	b = bp;
	if ( b < endaddr || b > maxaddr || (maxaddr-endaddr) < retsize )
		return(SYSERR);
	disable(ps);
	q = 0;
	qp = (char *) &memlist;
	p = (char*)memnext(qp);
	while ( p != 0 && p < b ) {
		q = p;
		qp = memp(q);
		p = (char*)memnext(qp);
	}
	if ( q != 0 && (top=q+memlen(qp)) > b ) {
		restore(ps);
		return(SYSERR);
	}
	if ( p != 0 && b + retsize > p ) {
		restore(ps);
		return(SYSERR);
		
	}
	if ( q != 0 && top == b )
		memlen(qp) += retsize;	/* consolidate q & b blocks	*/
	else {
		memlen(bp) = retsize;
		memnext(bp) = (struct mblock*)p;
		memnext(qp) = (struct mblock*)b;
		q = b;
		qp = bp;
	}
	if ( p != 0 && q+memlen(qp) == p ) {
		pp = memp(p);		/* consolidate q & p blocks	*/
		memlen(qp) += memlen(pp);
		memnext(qp) = memnext(pp);
	}
	memlist.mlen += retsize;
	restore(ps);
	return(OK);
}
