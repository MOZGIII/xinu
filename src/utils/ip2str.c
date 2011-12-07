/* ip2str.c - ip2str */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  ip2str  --  convert an IP address into dotted decimal string rep.
 *------------------------------------------------------------------------
 */
SYSCALL
ip2str(cp, IP)
char	*cp;
IPaddr	IP;
{
	int	i, temp;

	xinu_sprintf(cp, "%d.%d.%d.%d",
		IP[0]&0xff, IP[1]&0xff, IP[2]&0xff, IP[3]&0xff);
	return(OK);
}
