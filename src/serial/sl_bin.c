/* sl_bin.c - sl_bin */

#include <kernel.h>
#include <conf.h>
#include <proc.h>
#include <sl.h>
#include <mem.h>

extern int  sysmsg;

/*------------------------------------------------------------------------
 *  sl_bin  --  put character in read buffer
 *------------------------------------------------------------------------
 */
sl_bin(rdchr,slp)
register int rdchr;
register struct slblk *slp;
{
	if( isbadpid(slp->sl_rpid) ) {
		slp->sl_nord++;
	     	return;
	}
	
	*slp->sl_rawptr++ = rdchr;
	slp->sl_rawct++;
	
	if ( rdchr == SL_END ) {
		if ( slp->sl_rct == 0 ) {	/* Empty packet?       */
			slp->sl_badfend++;
			return;
		}
		slp->sl_rcv++;
		slp->sl_rstat = OK;
		ready(slp->sl_rpid);
		slp->sl_rpid = BADPID;
		return;
	}
	if ( rdchr >= SL_ESC && rdchr <= SL_TRANS_ESC ) {
		switch (rdchr) {
		case SL_ESC:			/* Deal with the ESC char */
			slp->sl_resc = TRUE;
			slp->sl_rescct++;
			return;
	
		case SL_TRANS_ESC:
			if ( slp->sl_resc ) {
				slp->sl_resc = FALSE;
				rdchr = SL_ESC;
			}
			break;
	
		case SL_TRANS_END:
			if ( slp->sl_resc ) {
				slp->sl_resc = FALSE;
				rdchr = SL_END;
			}
			break;
		}
	} else {
		if ( slp->sl_resc ) {
			slp->sl_resc = FALSE;
			slp->sl_badesc++;
		}
	}
	if ( ++slp->sl_rct > slp->sl_rlen ) {
		slp->sl_drop++;
		slp->sl_rstat = SYSERR;
		ready(slp->sl_rpid);
		slp->sl_rpid = BADPID;
	}
	*slp->sl_rptr++ = rdchr;
}
