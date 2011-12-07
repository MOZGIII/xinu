/* uqalloc.c - uqalloc */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  uqalloc -  allocate a TCP urgent queue entry
 *------------------------------------------------------------------------
 */
struct uqe *uqalloc()
{
	static	int	last = 0;
	struct	uqe	*puqe;
	int		count;

	if (!uqidone)
		uqinit();
	wait(uqmutex);
	for (count=0; count<UQTSIZE; ++count) {
		if (++last >= UQTSIZE)
			last = 0;
		puqe = &uqtab[last];
		if (puqe->uq_state == UQS_FREE) {
			puqe->uq_state = UQS_ALLOC;
			xinu_signal(uqmutex);
			return puqe;
		}
	}
	xinu_signal(uqmutex);
	return (struct uqe *)SYSERR;
}

Bool	uqidone = FALSE;
int	uqmutex;
struct uqe uqtab[UQTSIZE];
