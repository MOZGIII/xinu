/* netmatch.c - netmatch */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  netmatch  -  Is "dst" on "net"?
 *------------------------------------------------------------------------
 */
int netmatch(dst, net, mask, islocal)
IPaddr	dst, net, mask;
Bool	islocal;
{
    int i;

    for (i=0; i<IP_ALEN; i++)
        if ((mask[i] & dst[i]) != net[i])
            return FALSE;
    /*
	 * local srcs should not match broadcast addresses (host routes)
	 */
    if (islocal)
        if (isbrc(dst))
            return !blkequ(mask, ip_maskall, IP_ALEN);
	return TRUE;
}
