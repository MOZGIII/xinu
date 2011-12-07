/* tmset.c - tmset */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <mem.h>

/*------------------------------------------------------------------------
 *  tmset -  set a fast timer
 *------------------------------------------------------------------------
 */
int tmset(port, portlen, msg, time)
int port, portlen;
long time, msg;
{
	struct	tqent	*ptq, *newtq, *tq;

	newtq = (struct tqent *)getmem(sizeof(struct tqent));
	newtq->tq_timeleft = time;
    newtq->tq_time = ticks_10ms;
	newtq->tq_port = port;
	newtq->tq_portlen = portlen;
	newtq->tq_msg = msg;
	newtq->tq_next = NULL;
	/* clear duplicates */
	(void) tmclear(port, msg);
	wait(tqmutex);
	if (tqhead == NULL) {
		tqhead = newtq;
		resume(tqpid);
		xinu_signal(tqmutex);
		return OK;
	}
	/* search the list for our spot */

	for (ptq=0, tq=tqhead; tq; tq=tq->tq_next) {
		if (newtq->tq_timeleft < tq->tq_timeleft)
			break;
		newtq->tq_timeleft -= tq->tq_timeleft;
		ptq = tq;
	}
	newtq->tq_next = tq;
	if (ptq)
		ptq->tq_next = newtq;
	else
		tqhead = newtq;
	if (tq)
		tq->tq_timeleft -= newtq->tq_timeleft;
	xinu_signal(tqmutex);
	return OK;
}
