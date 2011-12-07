/* getnet.c - getnet, getinet */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  getnet  -  obtain the network portion of this system's IP address
 *------------------------------------------------------------------------
 */
SYSCALL getnet(ip)
IPaddr	ip;
{
	return getiaddr(NI_PRIMARY, ip);
}

/*------------------------------------------------------------------------
 *  getinet  -  obtain the network portion of this interface's IP address
 *------------------------------------------------------------------------
 */
SYSCALL
getinet(inum, ip)
int	inum;
IPaddr	ip;
{
	struct	netif *intf;

	if (inum < 0 || inum >= Net.nif)
		return SYSERR;
	intf = &nif[inum];
	if (!intf->ni_ivalid && getiaddr(inum, ip) == SYSERR)
			return SYSERR;
	blkcopy(ip, intf->ni_net, IP_ALEN);
	return OK;
}
