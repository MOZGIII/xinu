/* arp_in.c - arp_in */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

struct	arpentry	*arpfind(), *arpadd();

/*------------------------------------------------------------------------
 *  arp_in  -  handle ARP packet coming in from Ethernet network
 *	N.B. - Called by ni_in-- SHOULD NOT BLOCK
 *------------------------------------------------------------------------
 */
int arp_in(pni, pep)
struct	netif	*pni;
struct	ep	*pep;
{
	struct	arp		*parp = (struct arp *)pep->ep_data;
	struct	arpentry	*pae;
	int			arplen;

	parp->ar_hwtype = net2hs(parp->ar_hwtype);
	parp->ar_prtype = net2hs(parp->ar_prtype);
	parp->ar_op = net2hs(parp->ar_op);
    if (parp->ar_hwtype != pni->ni_hwtype ||
	    parp->ar_prtype != EPT_IP) {
		freebuf(pep);
        return OK;
	}

	if (pae = arpfind(SPA(parp), parp->ar_prtype, pni)) {
		blkcopy(pae->ae_hwa, SHA(parp), pae->ae_hwlen);
		pae->ae_ttl = ARP_TIMEOUT;
	}
	if (!blkequ(TPA(parp), pni->ni_ip, IP_ALEN)) {
		freebuf(pep);
        return OK;
	}
    if (pae == NULL)
		pae = arpadd(pni, parp);
	if (pae->ae_state == AS_PENDING) {
		pae->ae_state = AS_RESOLVED;
		arpqsend(pae);
	}
	if (parp->ar_op == AR_REQUEST) {
		parp->ar_op = AR_REPLY;
		blkcopy(TPA(parp), SPA(parp), parp->ar_prlen);
		blkcopy(THA(parp), SHA(parp), parp->ar_hwlen);
		blkcopy(pep->ep_dst, THA(parp), EP_ALEN);
		blkcopy(SHA(parp), pni->ni_hwa.ha_addr,
			pni->ni_hwa.ha_len);
		blkcopy(SPA(parp), pni->ni_ip, IP_ALEN);

		parp->ar_hwtype = hs2net(parp->ar_hwtype);
		parp->ar_prtype = hs2net(parp->ar_prtype);
		parp->ar_op = hs2net(parp->ar_op);

		arplen = sizeof(struct arp) +
			2*(parp->ar_prlen + parp->ar_hwlen);
		write(pni->ni_dev, pep, arplen);
	} else
		freebuf(pep);
	return OK;
}
