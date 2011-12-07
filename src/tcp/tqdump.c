/* tqdump.c - tqdump, tqwrite */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tqdump -  dump a timer queue (for debugging)
 *------------------------------------------------------------------------
 */
int tqdump()
{
	struct	tqent	*tq;

	for (tq=tqhead; tq; tq=tq->tq_next) {
		kprintf("tq <D %d,T %d, (%d, %d)>\n", tq->tq_timeleft,
			tq->tq_time, TCB(tq->tq_msg), EVENT(tq->tq_msg));
	}
}

/*------------------------------------------------------------------------
 *  tqwrite - write the timer queue to the given device
 *------------------------------------------------------------------------
 */
tqwrite(xinu_stdout)
int	xinu_stdout;
{
	struct	tqent	*tq;
	char		str[80];

    if (tqhead == NULL) {
		write(xinu_stdout, "no entries\n", 11);
		return OK;
	}
    xinu_write(xinu_stdout, "timeleft  time  port  portlen  msg\n", 35);
	for (tq=tqhead; tq; tq=tq->tq_next) {
        xinu_sprintf(str, "%8D %5D  %4d  %7d  %3D\n",
			tq->tq_timeleft, tq->tq_time, tq->tq_port,
			tq->tq_portlen, tq->tq_msg);
		write(xinu_stdout, str, xinu_strlen(str));
	}
	return OK;
}
