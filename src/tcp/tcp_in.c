/* tcp_in.c - tcp_in */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

int psend(int, struct ep *);

/*------------------------------------------------------------------------
 *  tcp_in - deliver an inbound TCP packet to the TCP process
 *------------------------------------------------------------------------
 */
int tcp_in(pni, pep)
struct	netif	*pni;
struct	ep	*pep;
{
	/* drop instead of blocking on psend */ 
	TcpInSegs++;
    if (pcount(tcps_iport) >= TCPQLEN) {
		freebuf(pep);
		return SYSERR;
	}
	psend(tcps_iport, pep);
	return OK;
}
