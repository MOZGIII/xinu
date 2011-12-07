/* xmalloc.c -- xmalloc */

#include <kernel.h>
#include <conf.h>
#include <proc.h>
#include <dos.h>
#include <mem.h>

/*---------------------------------------------------------------------------
 * xmalloc -- allocate memory using a malloc-like technique
 *---------------------------------------------------------------------------
 */
char *
xmalloc(nbytes)
word	nbytes;
{
	char	*mp;
	XBLOCK	*xmp;
	int	ps;

    if (nbytes == 0) 
	panic("xmalloc: 0 bytes requested\n");
/*        return(NULLPTR); */
	nbytes += sizeof(XBLOCK);
	disable(ps);
    if ((mp = getmem(nbytes)) == NULLPTR) {
        kprintf("xmalloc: out of memory, %d bytes requested (RETURN)\n",nbytes);
        kgetc();
        panic("xmalloc: memory depleted\n");
        restore(ps);
        return(NULLPTR);
	}
	xmp = (XBLOCK *)mp;
	xbinsert(xmp, nbytes, currpid);
	restore(ps);
	return ((char *)(xmp+1));
}
