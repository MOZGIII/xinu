/* uqinit.c - uqinit */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  uqinit -  initialize TCP urgent queue data
 *------------------------------------------------------------------------
 */
int uqinit()
{
	int	i;

	uqidone = TRUE;
	uqmutex = screate(0);
	for (i=0; i<UQTSIZE; ++i)
		uqtab[i].uq_state = UQS_FREE;
	xinu_signal(uqmutex);
	return OK;
}
