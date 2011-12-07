/* tcpcksum.c - tcpcksum */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tcpcksum -  compute a TCP pseudo-header checksum
 *------------------------------------------------------------------------
 */
unsigned short tcpcksum(pip)
struct	ip	*pip;
{
	struct	tcp	*ptcp = (struct tcp *)pip->ip_data;
	unsigned	short	*sptr, len;
	unsigned	long tcksum;
	int		i;

	tcksum = 0;

	sptr = (unsigned short *) pip->ip_src;
	/* 2*IP_ALEN octets = IP_ALEN shorts... */
	/* they are in net order.		*/
	for (i=0; i<IP_ALEN; ++i)
		tcksum += *sptr++;
	sptr = (unsigned short *)ptcp;
	len = pip->ip_len - IP_HLEN(pip);
	tcksum += hs2net(IPT_TCP + len);
	if (len % 2) {
		((char *)ptcp)[len] = 0;	/* pad */
		len += 1;	/* for the following division */
	}
	len >>= 1;	/* convert to length in shorts */

	for (i=0; i<len; ++i)
		tcksum += *sptr++;
	tcksum = (tcksum >> 16) + (tcksum & 0xffff);
	tcksum += (tcksum >> 16);

	return (short)(~tcksum & 0xffff);
}
