/* xfree.c -- xfree */

#include <conf.h>
#include <kernel.h>
#include <dos.h>
#include <mem.h>
#include <proc.h>

/*--------------------------------------------------------------------------
 * xfree -- free memory allocated by xmalloc or xcalloc
 *--------------------------------------------------------------------------
 */
SYSCALL xfree(mp)
char *mp;
{
	XBLOCK	*xmp;
	word	nbytes;
	int	ret;
	int	ps;

	if (mp == NULL)
		return(SYSERR);
	disable(ps);
	xmp = ((XBLOCK *)mp)-1;
	if (off(xmp) != 0) {
		restore(ps);
		return(SYSERR);
	}
	nbytes = xbdelete(xmp, currpid);
	if (freemem((char*)xmp, nbytes) == SYSERR)
		panic("xfree: inconsistent memory allocation");
	restore(ps);
	return(OK);
}
