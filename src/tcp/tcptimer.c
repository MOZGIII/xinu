/* tcptimer.c - tcptimer */

#include <conf.h>
#include <kernel.h>
#include <network.h>

int	tqmutex;
int	tqpid;
struct	tqent	*tqhead;
int psend(int, long);

/*------------------------------------------------------------------------
 *  tcptimer -  TCP timer process
 *------------------------------------------------------------------------
 */
PROCESS tcptimer()
{
	long	now, lastrun;		/* times from system clock	*/
    long    delta;              /* time since last iteration    */
	struct	tqent	*tq;		/* temporary delta list ptr	*/

    lastrun = ticks_10ms;       /* initialize to "now"      */
	tqmutex = screate(1);		/* mutual exclusion semaphore	*/
	tqpid = xinu_getpid();		/* record timer process id	*/
	xinu_signal(Net.sema);		/* start other network processes*/

	while (TRUE) {
        sleep10(TIMERGRAN); /* real-time delay      */
		if (tqhead == 0)	/* block timer process if delta	*/
			suspend(tqpid);	/*   list is empty		*/

		wait(tqmutex);
        now = ticks_10ms;
        delta = now - lastrun;  /* compute elapsed time     */

		/* Note: check for possible clock reset (time moved	*/
		/* backward or delay was over an order of magnitude too	*/
		/* long)						*/

        if (delta < 0 || delta > TIMERGRAN*100)
			delta = TIMERGRAN*10;	/* estimate the delay	*/
		lastrun = now;
		while (tqhead != 0  &&  tqhead->tq_timeleft <= delta) {
			delta -= tqhead->tq_timeleft;
			if (pcount(tqhead->tq_port) <= tqhead->tq_portlen)
				psend(tqhead->tq_port, tqhead->tq_msg);
			tq = tqhead;
			tqhead = tqhead->tq_next;
			freemem(tq, sizeof(struct tqent));
		}
		if (tqhead)
			tqhead->tq_timeleft -=delta;
		xinu_signal(tqmutex);
	}
}
