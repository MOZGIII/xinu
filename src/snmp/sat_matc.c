/* sat_match.c - sat_match */
    
#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <snmp.h>
#include <mib.h>

/*------------------------------------------------------------------------
 * sat_match - check if a variable exists in the Addr Translation Table
 *------------------------------------------------------------------------
 */
int sat_match(bindl, iface, entry, field, numifaces)
struct snbentry	*bindl;
int		*iface;
int		*entry;
int		*field;
int		numifaces;
{
	int 	oidi;
	struct	arpentry *pae;

	oidi = SAT_OIDLEN;
	if ((*field = bindl->sb_oid.id[oidi++]) > SNUMF_ATTAB)
		return SYSERR;
	if ((*iface = bindl->sb_oid.id[oidi++]) > numifaces)
		return SYSERR;
	oidi++;		/* skip over the 1 */
	/*
	 * oidi now points to IPaddr.  Read it and match it against
	 * the correct arp cache entry to get entry number
	 */
	for (*entry = 0; *entry < ARP_TSIZE; (*entry)++) {
		pae = &arptable[*entry];
		if (pae->ae_state != AS_FREE &&
		    pae->ae_pni == &nif[*iface] &&
		    soipequ(&bindl->sb_oid.id[oidi],pae->ae_pra,IP_ALEN))
			break;
	}
	if (*entry >= ARP_TSIZE)
		return SYSERR;
	if (oidi + IP_ALEN != bindl->sb_oid.len)
		return SYSERR;	/*  oidi is not at end of objid */
	return OK;
}
