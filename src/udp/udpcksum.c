/* udpcksum.c - udpcksum */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	UDP_ALEN	IP_ALEN		/* length of src+dst, in shorts	*/

/*------------------------------------------------------------------------
 *  udpcksum -  compute a UDP pseudo-header checksum
 *------------------------------------------------------------------------
 */
unsigned short udpcksum(pip)
struct	ip	*pip;
{
	struct	udp	*pudp = (struct udp *)pip->ip_data;
	unsigned	short	*psh;
	unsigned	long	sum;
	short		len = net2hs(pudp->u_len);
	int		i;

	sum = 0;

	psh = (unsigned short *) pip->ip_src;
	for (i=0; i<UDP_ALEN; ++i)
		sum += *psh++;

	psh = (unsigned short *)pudp;
	sum += hs2net(pip->ip_proto + len);
	if (len & 0x1) {
		((char *)pudp)[len] = 0;	/* pad */
		len += 1;	/* for the following division */
	}
	len /= 2;	/* convert to length in shorts */

	for (i=0; i<len; ++i)
		sum += *psh++;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);

	return (short)(~sum & 0xffff);
}
