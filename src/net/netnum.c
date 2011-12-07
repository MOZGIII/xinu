/* netnum.c - netnum */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  netnum  -  compute the network portion of a given IP address
 *------------------------------------------------------------------------
 */
int netnum(net, ipa)
IPaddr	net, ipa;
{
	int	bc;

	blkcopy(net, ipa, IP_ALEN);
	if (IP_CLASSA(net)) bc = 1;
	if (IP_CLASSB(net)) bc = 2;
	if (IP_CLASSC(net)) bc = 3;
	if (IP_CLASSD(net)) bc = 4;
	if (IP_CLASSE(net)) bc = 4;
	for (; bc < IP_ALEN; ++bc)
		net[bc] = 0;
	return OK;
}
