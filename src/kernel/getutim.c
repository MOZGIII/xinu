/* getutim.c - getutim */

#include <conf.h>
#include <kernel.h>
#include <dos.h>
#include <sleep.h>
#include <date.h>
#include <sys/time.h>


/*#define NETWORK*/

#ifdef NETWORK
#include <network.h>
#endif

extern long net2hl(long);

/*------------------------------------------------------------------------
 * getutim  --  obtain time in seconds past Jan 1, 1970, ut (gmt)
 *------------------------------------------------------------------------
 */
SYSCALL	getutim(timvar)
long	*timvar;
{
static	int	utinit = 0;
#ifdef NETWORK
	int	dev;
	int dgmode = DG_TMODE | DG_DMODE;
	int	len;
	long	now;
	long	utnow;
	char	*msg = "No time server response";
#endif
	int	year;
	int	i;
	union	REGS regs;
	long	tmptime;
	extern	struct datinfo Dat;	/* defined in ascdate.c */
	struct timeval tv;


	wait(clmutex);
	if (utinit != 0) {	/* no need to initialize */
		*timvar = clktime;
		xinu_signal(clmutex);
        return(OK);
	}

#ifdef NETWORK   /* quick patch, change to #ifdef later */
    dev = BADDEV;
    if (Net.tsvalid == TRUE &&
	    (dev=xinu_open(INTERNET, Net.tserver, ANYLPORT)) != SYSERR &&
    		control(dev,DG_SETMODE,&dgmode) == OK) {
		write(dev, msg, 2);	/* send junk packet to prompt */
		if (xinu_read(dev,&utnow,4) == 4) {
			xinu_close(dev);
			clktime = net2xt( net2hl(utnow) );
			utinit = 1;
			*timvar = clktime;
			xinu_signal(clmutex);
			return(OK);
		}
	}
	xinu_close(dev);
#endif
	gettimeofday(&tv, NULL);	
	*timvar = tv.tv_sec;
	utinit = 1;
	clktime = tv.tv_sec;
	xinu_signal(clmutex);
	return(OK);
}
