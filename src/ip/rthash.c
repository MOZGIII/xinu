/* rthash.c - rthash */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rthash  -  compute the hash for "net"
 *------------------------------------------------------------------------
 */
int rthash(net)
IPaddr	net;
{
	int	bc;	/* # bytes to count	*/
    long hv; /* hash value       */

	hv = 0;
	if (IP_CLASSA(net)) bc = 1;
	if (IP_CLASSB(net)) bc = 2;
	if (IP_CLASSC(net)) bc = 3;
	if (IP_CLASSD(net)) bc = 4;
	if (IP_CLASSE(net)) bc = 4;
	while (bc--)
		hv += net[bc] & 0xff;
	return hv % RT_TSIZE;
}
