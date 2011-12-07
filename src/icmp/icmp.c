/* icmp.c - icmp */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

struct	ep	*icsetbuf();
void        icsetsrc();
/*
 * ICT_REDIRECT	- pa2 == gateway address
 * ICT_PARAMP	- pa2 == (packet) pointer to parameter error
 * ICT_MASKRP	- pa2 == mask address
 * ICT_ECHORQ	- pa1 == seq, pa2 == data size
 */

/*------------------------------------------------------------------------
 *  icmp -  send an ICMP message
 *------------------------------------------------------------------------
 */
icmp(type, code, dst, pa1, pa2)
int   type, code;
IPaddr	dst;
char	*pa1, *pa2;
{
	struct	ep	*pep;
	struct	ip	*pip;
	struct	icmp	*pic;
	Bool		isresp, iserr;
	IPaddr		src, tdst;
	int		i, datalen;
	IcmpOutMsgs++;
	blkcopy(tdst, dst, IP_ALEN);	/* worry free pass by value	*/

	pep = icsetbuf(type, pa1, &isresp, &iserr);

    if (pep == NULL) {
		IcmpOutErrors++;
		return SYSERR;
	}
	pip = (struct ip *)pep->ep_data;
	pic = (struct icmp *) pip->ip_data;

	datalen = IC_HLEN;

	/* we fill in the source here, so routing won't break it */

	if (isresp) {
		if (iserr) {
			if (!icerrok(pep)) {
                freebuf(pep);
				return OK;
			}
			blkcopy(pic->ic_data, pip, IP_HLEN(pip)+8);
			datalen += IP_HLEN(pip)+8;
		}
		icsetsrc(pip);
	} else
		blkcopy(pip->ip_src, ip_anyaddr, IP_ALEN);
	blkcopy(pip->ip_dst, tdst, IP_ALEN);

	pic->ic_type = (char) type;
	pic->ic_code = (char) code;
	if (!isresp) {
		if (type == ICT_ECHORQ)
			pic->ic_seq = (int) pa1;
		else
			pic->ic_seq = 0;
		pic->ic_id = xinu_getpid();
	}
	datalen += icsetdata(type, pip, pa2);

	pic->ic_cksum = 0;
	pic->ic_cksum = cksum(pic, (datalen+1)>>1);

	pip->ip_proto = IPT_ICMP;	/* for generated packets */
    ipsend(tdst, pep, datalen);
    return OK;
}
