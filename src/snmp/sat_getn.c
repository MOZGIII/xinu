/* sat_getn.c - sat_getn */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <snmp.h>
#include <mib.h>
#include <asn1.h>

/*------------------------------------------------------------------------
 * sat_getn - do a getnext on a variable in the Address Translation Table
 *------------------------------------------------------------------------
 */
int sat_getn(bindl, mip, numifaces)
struct snbentry	*bindl;
struct mib_info	*mip;
int		numifaces;
{
	int 	entry, iface, field, oidi, i;
	int		tmp_entry;
	
	if (sat_match(bindl, &iface, &entry, &field, numifaces) == SYSERR)
		return SERR_NO_SUCH;
	for (i=1; field <= SNUMF_ATTAB && i<=numifaces; ++i) {
		if ((tmp_entry = sat_findnext(entry, iface)) >= 0) {
			entry = tmp_entry;
			break;
		}
		else
			entry = sat_findnext(-1,iface);

		if (++iface > numifaces) {
			iface = 1;
			++field;
		}
	}
	if (field > SNUMF_ATTAB)
		entry = tmp_entry;
	if (entry < 0)
		return (*mip->mi_next->mi_func)
				(bindl, mip->mi_next, SOP_GETF);
	oidi = SAT_OIDLEN;	/* 3.1.1 */

    bindl->sb_oid.id[oidi++] = (unsigned short) field;
    bindl->sb_oid.id[oidi++] = (unsigned short) iface;
    bindl->sb_oid.id[oidi++] = (unsigned short) 1;
	sip2ocpy(&bindl->sb_oid.id[oidi], arptable[entry].ae_pra);
	bindl->sb_oid.len = oidi + IP_ALEN;
	
	return sat_get(bindl, numifaces);
}
