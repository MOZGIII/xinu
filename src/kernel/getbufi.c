/* getbufi.c - getbufi */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  getbufi  --  get a buffer from a preestablished buffer pool,
 *	return immediately if no buffer is available 
 *------------------------------------------------------------------------
 */
char *getbufi(poolid)
int poolid;
{
	int	ps;
	int	*buf;
	struct bpool	*bpp;

	disable(ps);
	if ( poolid<0 || poolid >= nbpools
#ifdef	MEMMARK
		|| unmarked(bpmark)
#endif
		) {
		restore(ps);
        return(NULLPTR);
	}
	bpp = &bptab[poolid];
    if (scount(bpp->bpsem) <= 0 || wait(bpp->bpsem) != OK) {
		restore(ps);
        return(NULLPTR);
	}
	buf = bpp->bpnext;
	bpp->bpnext = *( (int ** ) buf);
	*buf++ = poolid;
	restore(ps);
	return( (char *) buf );
}
