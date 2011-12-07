/* setmask.c - setmask */

#include <conf.h>
#include <kernel.h>
#include <network.h>

extern	int	bsdbrc;		/* use Berkeley (all-0's) broadcast	*/
/*------------------------------------------------------------------------
 *  setmask - set the net mask for an interface
 *------------------------------------------------------------------------
 */
int setmask(inum, mask)
int	inum;
IPaddr	mask;
{
	IPaddr	aobrc;		/* all 1's broadcast */
	IPaddr	defmask;
	int	i;

	if (nif[inum].ni_svalid) {
		/* one set already-- fix things */

		rtdel(nif[inum].ni_subnet, nif[inum].ni_mask);
		rtdel(nif[inum].ni_brc, ip_maskall);
		rtdel(nif[inum].ni_subnet, ip_maskall);
	}
	blkcopy(nif[inum].ni_mask, mask, IP_ALEN);
	nif[inum].ni_svalid = TRUE;
	netmask(defmask, nif[inum].ni_ip);

	for (i=0; i<IP_ALEN; ++i) {
		nif[inum].ni_subnet[i] =
			nif[inum].ni_ip[i] & nif[inum].ni_mask[i]; 
		if (bsdbrc) {
			nif[inum].ni_brc[i] = nif[inum].ni_subnet[i];
			aobrc[i] = nif[inum].ni_subnet[i] |
				~nif[inum].ni_mask[i];
		} else
			nif[inum].ni_brc[i] = nif[inum].ni_subnet[i] |
				~nif[inum].ni_mask[i];
		/* set network (not subnet) broadcast */
		nif[inum].ni_nbrc[i] =
			nif[inum].ni_ip[i] | ~defmask[i];
	}

	/* install routes */
	/* net */
    rtadd(nif[inum].ni_subnet, nif[inum].ni_mask, nif[inum].ni_ip,
		0, inum, RT_INF);
    if (bsdbrc)
		rtadd(aobrc, ip_maskall, nif[inum].ni_ip, 0,
			NI_LOCAL, RT_INF);
	else	/* broadcast (all 1's) */
		rtadd(nif[inum].ni_brc, ip_maskall, nif[inum].ni_ip, 0,
			NI_LOCAL, RT_INF);
	/* broadcast (all 0's) */
	rtadd(nif[inum].ni_subnet, ip_maskall, nif[inum].ni_ip, 0,
		NI_LOCAL, RT_INF);
	return OK;
}

IPaddr	ip_maskall = { 255, 255, 255, 255 };
