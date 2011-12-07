/* slwstrt.c - slwstrt */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sl.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  slwstrt  --   daemon for serial line device write requests
 *------------------------------------------------------------------------
 */
PROCESS slwstrt(sptr)
struct 	slblk	*sptr;
{
	int	k;
	int	ps;
    char *packet;
    
	disable(ps);
	while ( TRUE ) {
		wait(sptr->sl_wstsem);
        packet = sptr->sl_wbuf;
        recvclr();
		sptr->sl_wpid = currpid;

		for(k=0; k<SL_RETRIES; k++) {

			sptr->sl_wct   = 0;
            sptr->sl_wptr = (char *)packet;
			sptr->sl_wesc  = FALSE;
			sptr->sl_wstat = SYSERR;

			wake_serial(sptr);
            if ( recvtim(SL_SNDTMOUT) == SL_XMITDONE )
				break;
				
			sptr->sl_sndtmo++;
		}
		sptr->sl_wpid = BADPID;
		freebuf(sptr->sl_wbuf);
		xinu_signal(sptr->sl_wsem);
	}
}
