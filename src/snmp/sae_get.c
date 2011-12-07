/* sae_get.c - sae_get */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <snmp.h>
#include <mib.h>
#include <asn1.h>
     
/*------------------------------------------------------------------------
 * sae_get - perform a get on a variable in the IP Address Entry Table
 *------------------------------------------------------------------------
 */
int sae_get(bindl, numifaces)
struct snbentry	*bindl;
int		numifaces;
{
	int 	iface, field;
	
	if (sae_match(bindl, &iface, &field, numifaces) == SYSERR)
		return SERR_NO_SUCH;
	switch (field) {
	case 1:		/* ipAdEntAddr */
		SVTYPE(bindl) = ASN1_IPADDR;
		blkcopy(SVIPADDR(bindl), nif[iface].ni_ip, IP_ALEN);
		return SNMP_OK;
	case 2:		/* ipAdEntIfIndex */
		SVTYPE(bindl) = ASN1_INT;
		SVINT(bindl) = iface;
		return SNMP_OK;
	case 3:		/* ipAdEntNetMask */
		SVTYPE(bindl) = ASN1_IPADDR;
		blkcopy(SVIPADDR(bindl), nif[iface].ni_mask, IP_ALEN);
		return SNMP_OK;
	case 4:		/* ipAdEntBcastAddr */
		SVTYPE(bindl) = ASN1_INT;
		SVINT(bindl) = (nif[iface].ni_brc[IP_ALEN - 1] & 0x01);
		return SNMP_OK;
	default:
		break;
	}
	return SERR_NO_SUCH;
}
