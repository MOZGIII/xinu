/* clkint.c - clkint */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sleep.h>
#include <io.h>
#include <sys/time.h>
extern long    ticks_10ms;    /* counts in 100ths of second 0-INF  */
extern unsigned long SysUpTime;
extern struct timeval prevTime;
/*------------------------------------------------------------------------
 *  clkint  --  clock service routine
 *  called at every clock tick and when starting the deferred clock
 *------------------------------------------------------------------------
 */
INTPROC clkint(mdevno)
int mdevno;				/* minor device number		*/
{
    /*
	We should to the following here:
	1) Increase clktime by number of seconds
	2) Increase ticks_10ms by number of 10ms passed from last tick
    */
	struct timeval curTime;
	gettimeofday(&curTime, NULL);
	
        ticks_10ms += (curTime.tv_usec - prevTime.tv_usec) / 10000;
	clktime += (curTime.tv_sec - prevTime.tv_sec);

	/* In miliseconds */
	SysUpTime += (curTime.tv_usec - prevTime.tv_usec) / 1000; /*tod;*/                    /* this is for SNMP */
	prevTime = curTime;
	
	if (defclk) {
		clkdiff++;
		return;
	}
	if (slnempty)
		if ( --*sltop <= 0 )
			wakeup();
	if ( --preempt <= 0 )
		resched();
}
