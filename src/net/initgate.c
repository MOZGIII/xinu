/* initgate.c - initgate */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

#ifdef	OTHER1
static  int itod[] = { NI_LOCAL, EC0, OTHER1, OTHER2 };
static  IPaddr  iftoip[] = { {0, 0, 0, 0}, {0, 0, 0, 0},
			{130, 10, 59, 0}, {209, 6, 36, 0} };
#else/*	OTHER1*/
static  int itod[] = { -1, EC0 };
#endif/*	OTHER1*/

/*------------------------------------------------------------------------
 * initgate  --  initialize a gateway's interface structures
 *------------------------------------------------------------------------
 */
int initgate()
{
	int	i;

	Net.netpool = mkpool(MAXNETBUF, NETBUFS);
	Net.lrgpool = mkpool(MAXLRGBUF, LRGBUFS);
	Net.sema = screate(1);
	Net.nif = NIF;

	for (i=0; i<Net.nif; ++i) {
		/* start off with all 0's (esp. statistics) */
		bzero(&nif[i], sizeof(nif[i]));

		nif[i].ni_state = NIS_DOWN;
		nif[i].ni_admstate = NIS_UP;
		nif[i].ni_ivalid = nif[i].ni_nvalid = FALSE;
		nif[i].ni_svalid = FALSE;
		nif[i].ni_dev = itod[i];
		if (i == NI_LOCAL) {
            /* maxbuf - ether header */
            nif[i].ni_mtu = MAXLRGBUF-EP_HLEN;
			blkcopy(nif[i].ni_ip, ip_anyaddr, IP_ALEN);
			continue;
		}
		switch(nif[i].ni_dev) {
#if	Noth > 0
		case OTHER1:
		case OTHER2:
				ofaceinit(i);
				break;
#endif	/* Noth > 0 */
        case EC0: efaceinit(i);
				break;
		};
	}

	return OK;
}

#if	Noth > 0
ofaceinit(iface)
int	iface;
{
	struct otblk	*otptr;
	struct etblk	*etptr;
	int		i;

	blkcopy(nif[iface].ni_ip, iftoip[iface], IP_ALEN);

	otptr = (struct otblk *)devtab[nif[iface].ni_dev].dvioblk;
	otptr->ot_intf = iface;

	nif[iface].ni_descr = otptr->ot_descr;
	nif[iface].ni_mtype = 1;	/* RFC 1156, "other" :-)	*/
	nif[iface].ni_speed = 400000000; /* ~25Mz 32 bit mem copy :-)	*/

	/* fill in physical net addresses */
	blkcopy(nif[iface].ni_hwa.ha_addr, otptr->ot_paddr, EP_ALEN);
	blkcopy(nif[iface].ni_hwb.ha_addr, otptr->ot_baddr, EP_ALEN);
	nif[iface].ni_hwa.ha_len = nif[iface].ni_hwb.ha_len = EP_ALEN;

	/* set the mtu */

	if (nif[iface].ni_dev == OTHER2)
		nif[iface].ni_mtu = SMALLMTU;
	else
		nif[iface].ni_mtu = EP_DLEN;

	etptr = (struct etblk *)devtab[otptr->ot_pdev].dvioblk;
	nif[iface].ni_outq = etptr->etoutq;

	/* net num comes from ether addr */

	nif[iface].ni_ip[IP_ALEN-1] = etptr->etpaddr[EP_ALEN-1];
	netnum(nif[iface].ni_net, nif[iface].ni_ip);
	nif[iface].ni_ivalid = TRUE;

	/* set the mask (same for both) */

	setmask(iface, "\377\377\377\0");

	/* host */
	rtadd(nif[iface].ni_ip, ip_maskall, nif[iface].ni_ip, 0,
		NI_LOCAL, RT_INF);
	/* broadcast (all 0's, no subnet) */
	rtadd(nif[iface].ni_net, ip_maskall, nif[iface].ni_ip, 0,
		NI_LOCAL, RT_INF);
}
#endif	/* Noth > 0 */

efaceinit(iface)
int	iface;
{
	IPaddr	junk;

    struct etblk *etptr = (struct etblk *)
			devtab[nif[iface].ni_dev].dvioblk;
    blkcopy(nif[iface].ni_hwa.ha_addr, etptr->etpaddr, EP_ALEN);
	blkcopy(nif[iface].ni_hwb.ha_addr, etptr->etbcast, EP_ALEN);
	nif[iface].ni_descr = etptr->etdescr;
	nif[iface].ni_mtype = 6;	/* RFC 1156, Ethernet CSMA/CD	*/
	nif[iface].ni_speed = 10000000;	/* bits per second		*/
	nif[iface].ni_hwa.ha_len = EP_ALEN;
	nif[iface].ni_hwb.ha_len = EP_ALEN;
	nif[iface].ni_mtu = EP_DLEN;
    nif[iface].ni_outp = etptr->etoutp;
    etptr->etintf = iface;
}
