/* sl_bout.c - sl_bout */

#include <kernel.h>
#include <conf.h>
#include <proc.h>
#include <sl.h>

/*------------------------------------------------------------------------
 *  sl_bout  --  returns the character which is to be transmitted, or 
 *               SYSERR to indicate that there should be no character 
 *               transmitted, since it is possibe for us to get a
 *               transmit interrupt when we don't really want one.
 *------------------------------------------------------------------------
 */
int
sl_bout(sptr)
register struct slblk *sptr;
{
	int	wrchr, c;

	if ( isbadpid(sptr->sl_wpid) ) {
		sptr->sl_notx++;
		return(SYSERR);
	}

	if ( sptr->sl_wesc ) {
		sptr->sl_wesc = FALSE;
		return(sptr->sl_wechar);
	}

	if ( sptr->sl_wct == sptr->sl_wlen ) {
		sptr->sl_snd++;
		return(SL_END);
	}

	sptr->sl_wct++;
	switch ( wrchr = (*sptr->sl_wptr++) & 0xff ) {
	case SL_ESC:
		sptr->sl_wesc = TRUE;
		sptr->sl_wechar = SL_TRANS_ESC;
		sptr->sl_wescct++;
		break;
		
	case SL_END:
		wrchr = SL_ESC;
		sptr->sl_wesc = TRUE;
		sptr->sl_wechar = SL_TRANS_END;
		sptr->sl_wescct++;
		break;
	}
	return(wrchr);
}
