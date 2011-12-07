/* uqfree.c - uqfree */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  uqfree -  free an urgent queue entry
 *------------------------------------------------------------------------
 */
int uqfree(puqe)
struct	uqe	*puqe;
{
	wait(uqmutex);
	if (puqe->uq_data)
		freebuf(puqe->uq_data);
	puqe->uq_state = UQS_FREE;
	xinu_signal(uqmutex);
	return OK;
}
