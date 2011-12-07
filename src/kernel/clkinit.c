/* clkinit.c - clkinit */

#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <sleep.h>
#include <sys/time.h>

int	clmutex;
long    clktime;
long    ticks_10ms;        /* counts in 100ths of second 0-INF  */
struct timeval prevTime;
/*------------------------------------------------------------------------
 *  clkinit  --  initialize the clock and sleep queue (called at startup)
 *------------------------------------------------------------------------
 */
clkinit()
{
    gettimeofday(&prevTime, NULL);
    clmutex = screate(1);   /* clock semaphore      */
    clktime = 0L;           /* initial time of day      */
    ticks_10ms = 0L;       /* 1/100 sec ticks */
    tod = 0L;               /* seconds since startup    */
    preempt = QUANTUM;		/* initial time quantum		*/
    slnempty = FALSE;		/* initially, no process asleep	*/
    clkdiff = 0;			/* zero deferred ticks		*/
    defclk = 0;             /* clock is not deferred    */
    clockq = newqueue();    /* allocate clock queue in q    */
}
