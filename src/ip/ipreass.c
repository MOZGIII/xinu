/* ipreass.c - ipreass */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <q.h>
#include <bufpool.h>

struct	ep	*ipfjoin();

/*------------------------------------------------------------------------
 *  ipreass  -  reassemble an IP datagram, if necessary
 *	returns packet, if complete; 0 otherwise
 *------------------------------------------------------------------------
 */
struct ep *ipreass(pep)
struct	ep	*pep;
{
	struct	ep	*pep2;
	struct	ip	*pip;
	int		firstfree;
	int		i;

	pip = (struct ip *)pep->ep_data;

	wait(ipfmutex);

	if ((pip->ip_fragoff & (IP_FRAGOFF|IP_MF)) == 0) {
		xinu_signal(ipfmutex);
		return pep;
	}
	IpReasmReqds++;
	firstfree = -1;
	for (i=0; i<IP_FQSIZE; ++i) {
		struct	ipfq	*piq = &ipfqt[i];

		if (piq->ipf_state == IPFF_FREE) {
			if (firstfree == -1)
				firstfree = i;
			continue;
		}
		if (piq->ipf_id != pip->ip_id)
			continue;
		if (!blkequ(piq->ipf_src, pip->ip_src, IP_ALEN))
			continue;
		/* found a match */
		if (ipfadd(piq, pep) == 0) {
			xinu_signal(ipfmutex);
			return 0;
		}
		pep2 = ipfjoin(piq);
		xinu_signal(ipfmutex);
		return pep2;
		
	}
	/* no match */

	if (firstfree < 0) {
		/* no room-- drop */
		freebuf(pep);
		xinu_signal(ipfmutex);
		return 0;
	}
	ipfqt[firstfree].ipf_q = newq(IP_FQSIZE, QF_WAIT);
	if (ipfqt[firstfree].ipf_q < 0) {
		freebuf(pep);
		xinu_signal(ipfmutex);
		return 0;
	}
	blkcopy(ipfqt[firstfree].ipf_src, pip->ip_src, IP_ALEN);
	ipfqt[firstfree].ipf_id = pip->ip_id;
	ipfqt[firstfree].ipf_ttl = IP_FTTL;
	ipfqt[firstfree].ipf_state = IPFF_VALID;
	ipfadd(&ipfqt[firstfree], pep);
	xinu_signal(ipfmutex);
	return 0;
}

int	ipfmutex;
struct	ipfq	ipfqt[IP_FQSIZE];
