/* othwrite.c - othwrite */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  othwrite - write a single packet to the ethernet
 *------------------------------------------------------------------------
 */
int othwrite(devptr, pep, len)
struct	devsw	*devptr;
struct	ep	*pep;
int	len;
{
#ifdef NOTYET
	STATWORD ps;
        struct	otblk	*otptr;
        struct	etblk	*etptr;

	if (len > EP_MAXLEN) {
		freebuf(pep);
		return SYSERR;
	}
	/* subtract the local header */
	len -= (int)&pep->ep_eh - (int)pep;

	if (len < EP_MINLEN)
		len = EP_MINLEN;

	otptr = (struct otblk *)devptr->dvioblk;
	blkcopy(pep->ep_src, otptr->ot_paddr, EP_ALEN);
	etptr = (struct etblk *)devtab[otptr->ot_pdev].dvioblk;

#ifdef	DEBUG
	if (pep->ep_type == EPT_IP)
	{
		struct ip *pip = (struct ip *)pep->ep_data;
		if (pip->ip_proto == IPT_TCP) {
			kprintf("othwrite(%X, %d)\n", pep, len);
			ipdump(pep);
		}
	}
#endif	/* DEBUG */
	pep->ep_len = len;
	if (enq(etptr->etoutq, pep, 0) < 0) {
		freebuf(pep);
		return SYSERR;
	}
	ethwstrt(etptr);
#endif
kprintf("othwrite: ****************\n");
    return OK;
}
