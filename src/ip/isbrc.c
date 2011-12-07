/* isbrc.c - isbrc */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  isbrc  -  Is "dest" a broadcast address?
 *------------------------------------------------------------------------
 */
int isbrc(dest)
IPaddr	dest;
{
	int	inum;

	/* all 0's and all 1's are broadcast */

	if (blkequ(dest, ip_anyaddr, IP_ALEN) ||
	    blkequ(dest, ip_maskall, IP_ALEN))
		return TRUE;

	/* check real broadcast address and BSD-style for net & subnet 	*/

	for (inum=0; inum < Net.nif; ++inum)
		if (blkequ(dest, nif[inum].ni_brc, IP_ALEN) ||
		    blkequ(dest, nif[inum].ni_nbrc, IP_ALEN) ||
		    blkequ(dest, nif[inum].ni_subnet, IP_ALEN) ||
		    blkequ(dest, nif[inum].ni_net, IP_ALEN))
			return TRUE;

	return FALSE;
}
