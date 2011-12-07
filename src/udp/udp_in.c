/* udp_in.c - udp_in */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  udp_in -  handle an inbound UDP datagram
 *------------------------------------------------------------------------
 */
int udp_in(pni, pep)
struct	netif	*pni;
struct	ep	*pep;
{
	struct	ip	*pip = (struct ip *)pep->ep_data;
	struct	udp	*pudp = (struct udp *)pip->ip_data;
	struct	upq	*pup;
	unsigned short	dst;
	int		i;

	if (pudp->u_cksum && udpcksum(pip)) {
		freebuf(pep);
		return SYSERR;			/* checksum error */
	}
	udpnet2h(pudp);		/* convert UDP header to host order */
	dst = pudp->u_dst;
	wait(udpmutex);
	for (i=0 ; i<UPPS ; i++) {
		pup = &upqs[i];
		if (pup->up_port == dst) {
			/* drop instead of blocking on psend */ 
			if (pcount(pup->up_xport) >= UPPLEN) {
				xinu_signal(udpmutex);
				freebuf(pep);
				UdpInErrors++;
				return SYSERR;
			}
			psend(pup->up_xport, pep);
			UdpInDatagrams++;
			if (!isbadpid(pup->up_pid)) {
				send(pup->up_pid, OK);
				pup->up_pid = BADPID;
			}
			xinu_signal(udpmutex);
			return OK;
		}
	}
	xinu_signal(udpmutex);
	UdpNoPorts++;
    icmp(ICT_DESTUR, ICC_PORTUR, pip->ip_src, pep);
    return OK;
}

int	udpmutex;
