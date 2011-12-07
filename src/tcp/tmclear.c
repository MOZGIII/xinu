/* tmclear.c - tmclear */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tmclear -  clear the indicated timer
 *------------------------------------------------------------------------
 */
int tmclear(port, msg)
int port;
long msg;
{
	struct	tqent	*prev, *ptq;
    long    timespent;

	wait(tqmutex);
	prev = 0;
	for (ptq = tqhead; ptq != NULL; ptq = ptq->tq_next) {
		if (ptq->tq_port == port && ptq->tq_msg == msg) {
            timespent = ticks_10ms - ptq->tq_time;
			if (prev)
				prev->tq_next = ptq->tq_next;
			else
				tqhead = ptq->tq_next;
			if (ptq->tq_next)
				ptq->tq_next->tq_timeleft +=
						ptq->tq_timeleft;
			xinu_signal(tqmutex);
			freemem(ptq, sizeof(struct tqent));
			return timespent;
		}
		prev = ptq;
	}
	xinu_signal(tqmutex);
	return SYSERR;
}  
