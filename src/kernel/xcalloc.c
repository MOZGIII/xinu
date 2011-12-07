/* xcalloc.c -- xcalloc */

#include <conf.h>
#include <kernel.h>
#include <mem.h>

/*---------------------------------------------------------------------------
 * xcalloc -- allocate and clear a memory block
 *---------------------------------------------------------------------------
 */
char *xcalloc(m,n)
word m,n;
{
	char	*memp;
	char	*wp;

	m = m*n;
	if ( m == 0 )
        return(NULLPTR);
	if ( (memp=xmalloc(m)) == NULL )
        return(NULLPTR);
	/* assumes longs can be aligned on any byte boundary */
	for ( wp = memp ; m >= sizeof(long) ; ) {
		wp = (char*)((long*)wp + 1);
		*wp = 0L;
		m -= sizeof(long);
	}
	for ( ; m > 0 ; ) {
		*wp++ = 0;
		m--;
	}
	return(memp);
}
