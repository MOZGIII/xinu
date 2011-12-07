/* ipfsend.c - ipfsend */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  ipfsend -  send one fragment of an IP datagram
 *------------------------------------------------------------------------
 */
int ipfsend(pni, nexthop, pep, offset, maxdlen, offindg)
struct	netif	*pni;
IPaddr		nexthop;
struct	ep	*pep;
int		offset, maxdlen, offindg;
{
	struct	ep	*pepnew;
	struct	ip	*pip, *pipnew;
	int		hlen, len;

	pepnew = (struct ep *)getbuf(Net.netpool);
	if (pepnew == (struct ep *)SYSERR)
		return SYSERR;
	hlen = ipfhcopy(pepnew, pep, offindg);	/* copy the headers */

	pip = (struct ip *)pep->ep_data;
	pipnew = (struct ip *)pepnew->ep_data;
	pipnew->ip_fragoff = IP_MF | (offindg>>3);
	pipnew->ip_len = len = maxdlen + hlen;
	pipnew->ip_cksum = 0;

	iph2net(pipnew);
	pipnew->ip_cksum = cksum(pipnew, hlen>>1);

	blkcopy(&pepnew->ep_data[hlen],
		&pep->ep_data[IP_HLEN(pip)+offset], maxdlen);
	blkcopy(pepnew->ep_nexthop, nexthop, IP_ALEN);

	return netwrite(pni, pepnew, EP_HLEN+len);
}
