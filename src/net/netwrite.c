/* netwrite.c - netwrite */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <q.h>
#include <bufpool.h>

struct	arpentry	*arpalloc(), *arpfind();

/*------------------------------------------------------------------------
 * netwrite - write a packet on an interface, using ARP if needed
 *------------------------------------------------------------------------
 */
int netwrite(pni, pep, len)
struct	netif	*pni;
struct	ep	*pep;
int		len;
{
	struct	arpentry 	*pae;
    int         ps;
	int			i;
	if (pni->ni_state != NIS_UP) {
		freebuf(pep);
		return SYSERR;
	}
    pep->ep_len = len;
    if (pni == &nif[NI_LOCAL])
        return local_out(pep);
	else if (isbrc(pep->ep_nexthop)) {
        blkcopy(pep->ep_dst, pni->ni_hwb.ha_addr, EP_ALEN);
		write(pni->ni_dev, pep, len);
		return OK;
	}
	/* else, look up the protocol address... */
    disable(ps);
	pae = arpfind(pep->ep_nexthop, pep->ep_type, pni);
    if (pae && pae->ae_state == AS_RESOLVED) {
		blkcopy(pep->ep_dst, pae->ae_hwa, pae->ae_hwlen);
        restore(ps);
		write(pni->ni_dev, pep, len);
		return OK;
	}
    if (pae == NULL) {
		pae = arpalloc();
        pae->ae_hwtype = AR_HARDWARE;
		pae->ae_prtype = EPT_IP;
		pae->ae_hwlen = EP_ALEN;
		pae->ae_prlen = IP_ALEN;
		pae->ae_pni = pni;
		pae->ae_queue = EMPTY;
		blkcopy(pae->ae_pra, pep->ep_nexthop, pae->ae_prlen);
		pae->ae_attempts = 0;
		pae->ae_ttl = ARP_RESEND;
		arpsend(pae);
	}
	if (pae->ae_queue == EMPTY)
		pae->ae_queue = newq(ARP_QSIZE, QF_NOWAIT);
	if (enq(pae->ae_queue, pep, 0) < 0)
		freebuf(pep);
    restore(ps);
	return OK;
}
