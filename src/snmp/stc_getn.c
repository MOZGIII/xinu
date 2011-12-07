/* stc_getn.c - stc_getn */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <snmp.h>
#include <mib.h>
#include <asn1.h>

/*------------------------------------------------------------------------
 * stc_getn - do a getnext on a variable in the TCP connection table
 *------------------------------------------------------------------------
 */
stc_getn(bindl, mip)
struct snbentry	*bindl;
struct mib_info	*mip;
{
	int	oidi, field, tcbn, ttcbn;

	if (stc_match(bindl,&field,&tcbn) == SYSERR)
		return SERR_NO_SUCH;
	/* search for next connection */
	if ((tcbn = stc_findnext(tcbn)) < 0) {
		tcbn = stc_findnext(-1);
		if (++field > SNUMF_TCTAB)
			return((*mip->mi_next->mi_func)
				(bindl, mip->mi_next, SOP_GETF));
	}
	oidi = STC_OIDLEN; 	/* 6.13.1 */

    bindl->sb_oid.id[oidi++] = (unsigned short) field;
	sip2ocpy(&bindl->sb_oid.id[oidi], tcbtab[tcbn].tcb_lip);
	oidi += IP_ALEN;
    bindl->sb_oid.id[oidi++] = (unsigned short) tcbtab[tcbn].tcb_lport;
	sip2ocpy(&bindl->sb_oid.id[oidi], tcbtab[tcbn].tcb_rip);
	oidi += IP_ALEN;
    bindl->sb_oid.id[oidi++] = (unsigned short) tcbtab[tcbn].tcb_rport;
	bindl->sb_oid.len = oidi;

	return stc_get(bindl);
}
