/* sat_findn.c - sat_findnext, satcmp */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 * sat_findnext - for given iface, find next resolved arp entry
 *------------------------------------------------------------------------
 */
int sat_findnext(entry, iface)
int	entry;
int	iface;
{
	int	i, next;

	next = -1;
	for (i = 0; i < ARP_TSIZE; ++i) {
		struct arpentry	*pae = &arptable[i];

		if (pae->ae_state == AS_FREE ||
		    pae->ae_pni != &nif[iface] ||
		    (entry >= 0 && satcmp(pae, &arptable[entry]) <= 0))
			continue;
		if (next < 0 || satcmp(pae, &arptable[next]) < 0)
			next = i;
	}
	return next;
}

/*------------------------------------------------------------------------
 * satcmp - compare two ARP table entries in SNMP lexicographic order
 *------------------------------------------------------------------------
 */
int satcmp(pae1, pae2)
struct	arpentry	*pae1, *pae2;
{
	int	rv;

	if (rv = (pae1->ae_prlen - pae2->ae_prlen))
		return rv;
	return blkcmp(pae1->ae_pra, pae2->ae_pra, pae1->ae_prlen);
}
