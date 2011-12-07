/* dot2ip.c - dot2ip */

#include <conf.h>
#include <kernel.h>
#include <network.h>

extern char	*index();

/*------------------------------------------------------------------------
 *  dot2ip - convert a dotted decimal string into an IP address
 *------------------------------------------------------------------------
 */
char*	dot2ip(ip, pdot)
char	*pdot;
IPaddr	ip;
{
	int	i;

	for (i=0; pdot && i<IP_ALEN; ++i) {
		ip[i] = atoi(pdot);
		if (pdot = index(pdot, '.'))
			++pdot;
	}
	while (i<IP_ALEN)
		ip[i++] = 0;
	return (char*)ip;
}
