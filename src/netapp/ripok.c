/* ripok.c - ripok */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  ripok  -  determine if a received RIP route is ok to install
 *------------------------------------------------------------------------
 */
int ripok(rp)
struct	riprt	*rp;
{
	if (rp->rr_family != AF_INET)
		return FALSE;
	if (rp->rr_metric > RIP_INFINITY)
		return FALSE;
	if (IP_CLASSD(rp->rr_addr) || IP_CLASSE(rp->rr_addr))
		return FALSE;
	if (rp->rr_addr[0] == 0 &&
	    !blkequ(rp->rr_addr, ip_anyaddr, IP_ALEN))
		return FALSE;		/* net 0, host non-0		*/
	if (rp->rr_addr[0] == 127)
		return FALSE;		/* loopback net			*/
	return TRUE;
}
