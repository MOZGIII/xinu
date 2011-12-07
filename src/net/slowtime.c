/* slowtimer.c - slowtimer */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

#define	STGRAN	1		/* Timer granularity (delay) in seconds	*/

/*------------------------------------------------------------------------
 * slowtimer - handle long-term periodic maintenance of network tables
 *------------------------------------------------------------------------
 */
PROCESS	slowtimer()
{
	long	lasttime, now;	/* previous and current times in seconds*/
	int	delay;		/* actual delay in seconds		*/

	xinu_signal(Net.sema);

    gettime(&lasttime);
	while (1) {
		sleep(STGRAN);
        gettime(&now);
		delay = now - lasttime;
		lasttime = now;

		if (delay == 0)
			delay = 1;

		arptimer(delay);
		ipftimer(delay);
		rttimer(delay);
	}
}
