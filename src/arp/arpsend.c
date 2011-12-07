/* arpsend.c - arpsend */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 * arpsend - broadcast an ARP request
 *	N.B. Assumes interrupts disabled
 *------------------------------------------------------------------------
 */
int arpsend(pae)
struct	arpentry	*pae;
{
	struct	netif	*pni = pae->ae_pni;
	struct	ep	*pep;
	struct	arp	*parp;
	int	arplen;

    pep = (struct ep *) getbuf(Net.netpool);   /* getbuf is ok here */
    if ((unsigned)pep == NULL)
		return SYSERR;
	blkcopy(pep->ep_dst, pni->ni_hwb.ha_addr, pae->ae_hwlen);
	pep->ep_type = EPT_ARP;
	parp = (struct arp *) pep->ep_data;
	parp->ar_hwtype = hs2net(pae->ae_hwtype);
	parp->ar_prtype = hs2net(pae->ae_prtype);
	parp->ar_hwlen = pae->ae_hwlen;
	parp->ar_prlen = pae->ae_prlen;
	parp->ar_op = hs2net(AR_REQUEST);
	blkcopy(SHA(parp), pni->ni_hwa.ha_addr, pae->ae_hwlen);
	blkcopy(SPA(parp), pni->ni_ip, pae->ae_prlen);
	bzero(THA(parp), pae->ae_hwlen);
	blkcopy(TPA(parp), pae->ae_pra, pae->ae_prlen);
	arplen = sizeof(struct arp) + 2*(parp->ar_hwlen + parp->ar_prlen);
    xinu_write(pni->ni_dev, pep, arplen);
    return OK;
}
