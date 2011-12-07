/* tmleft.c - tmleft */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tmleft -  how much time left for this timer?
 *------------------------------------------------------------------------
 */
int tmleft(port, msg)
int port;
long msg;
{
	struct	tqent	*tq;
	int		timeleft = 0;
    if (tqhead == NULL) {
		return 0;
    }
	wait(tqmutex);
	for (tq = tqhead; tq != NULL; tq = tq->tq_next) {
		timeleft += tq->tq_timeleft;
		if (tq->tq_port == port && tq->tq_msg == msg) {
			xinu_signal(tqmutex);
			return timeleft;
		}
	}
	xinu_signal(tqmutex);
	return 0;
}
