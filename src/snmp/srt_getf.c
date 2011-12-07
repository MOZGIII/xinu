/* srt_getf.c - srt_getf */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <snmp.h>
#include <mib.h>
#include <asn1.h>

/*------------------------------------------------------------------------
 * srt_getf - perform a getfirst on a variable in the Routing Table
 *------------------------------------------------------------------------
 */
int srt_getf(bindl, mip, numifaces)
struct snbentry	*bindl;
struct mib_info	*mip;
int		numifaces;
{
	int 		rtl, oidi;
	struct route	*rtp, *srt_findnext();

	rtl = -1;	/* use first field, first route */
    if ((rtp = srt_findnext(rtp, &rtl)) == NULL) {
		if (mip->mi_next)
			return (*mip->mi_next->mi_func)
				(bindl, mip->mi_next, SOP_GETF);
		return SERR_NO_SUCH;	/* no next node */
	}
	blkcopy(bindl->sb_oid.id, mip->mi_objid.id, mip->mi_objid.len*2);
	oidi = mip->mi_objid.len;
	
    bindl->sb_oid.id[oidi++] = (unsigned short) 1;     /* field */
	sip2ocpy(&bindl->sb_oid.id[oidi], rtp->rt_net);
	bindl->sb_oid.len = oidi + IP_ALEN;
	
	return srt_get(bindl, numifaces);
}
