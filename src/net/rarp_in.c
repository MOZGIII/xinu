/* rarp_in.c - rarp_in */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  rarp_in  -  handle RARP packet coming in from Ethernet network
 *------------------------------------------------------------------------
 */
int rarp_in(pni, packet)
struct	netif	*pni;
struct	ep	*packet;
{
    int     ps;
	int		pid, ret;
	int		device = pni->ni_dev;
	struct	arp	*parp;
	struct	etblk	*etptr;
	parp = (struct arp *) packet->ep_data;
	parp->ar_op = net2hs(parp->ar_op);
	parp->ar_hwtype = net2hs(parp->ar_hwtype);
	parp->ar_prtype = net2hs(parp->ar_prtype);
	if (parp->ar_op == RA_REPLY) {
		etptr = (struct etblk *)devtab[device].dvioblk;
		if (blkequ(THA(parp), etptr->etpaddr, EP_ALEN)) {
			IPaddr mask;
			int i;

			blkcopy(pni->ni_ip, TPA(parp), IP_ALEN);
			netnum(pni->ni_net, pni->ni_ip);
			pni->ni_ivalid = TRUE;
			netmask(mask, pni->ni_ip);
			setmask(etptr->etintf, mask);
			disable(ps);
			pid = rarppid;
			if (!isbadpid(pid)) {
				rarppid = BADPID;
                send(pid, OK);
			}
			restore(ps);
		}
		ret = OK;
	} else
		ret = SYSERR;
	freebuf(packet);
	return ret;
}
