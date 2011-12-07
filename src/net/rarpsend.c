/* rarpsend.c - rarpsend */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <bufpool.h>

static struct  ep  *mkarp(int ifn, short type, short op, IPaddr spa, IPaddr tpa);
/*------------------------------------------------------------------------
 *  rarpsend  -  broadcast a RARP packet to obtain my IP address
 *------------------------------------------------------------------------
 */
int rarpsend(ifn)
int	ifn;		/* the interface number */
{
    int     ps;
    struct  netif *pni = &nif[ifn];
    struct  ep *pep;
    int     i, mypid, pid, resp;
    IPaddr  junk;                       /* arg to mkarp; never used */
    IPaddr  mask;
    extern  IPaddr ip_maskall;
    char    buf[16];
    
    mypid = xinu_getpid();
	for (i=0; i<ARP_MAXRETRY; ++i) {
		pep = mkarp(ifn, EPT_RARP, RA_REQUEST, junk, junk);
        if (pep == (struct ep *)SYSERR)
			break;
		disable(ps);
		rarppid = mypid;
		restore(ps);
		recvclr();
        xinu_write(pni->ni_dev, pep, EP_MINLEN);
        /* ARP_RESEND is in secs, recvtim uses 1/10's of secs   */
        resp = recvtim(10*ARP_RESEND<<i);
        if (resp != TIMEOUT) {
			/* host route */
            rtadd(pni->ni_ip, ip_maskall, pni->ni_ip, 0,
                NI_LOCAL, RT_INF);
  
			/* non-subnetted route */
           rtadd(pni->ni_net, ip_maskall, pni->ni_ip, 0,
                NI_LOCAL, RT_INF);
      
			return OK;
		}
	}

/* The following code is for testing Xinu without a RARP server */

    xinu_printf("No RARP response, enter IP addr: ");
    i=read(CONSOLE,buf,sizeof(buf)); 
    buf[i-1]='\0';
    if (name2ip(pni->ni_ip, buf) != SYSERR) {
        netnum(pni->ni_net, pni->ni_ip);
        pni->ni_ivalid = TRUE;
        netmask(mask, pni->ni_ip);
        setmask(ifn, mask);
        disable(ps);
        pid = rarppid;
        if (!isbadpid(pid))
            rarppid = BADPID;
        restore(ps);
        /* host route */
        rtadd(pni->ni_ip, ip_maskall, pni->ni_ip, 0,
            NI_LOCAL, RT_INF);
        return OK;
    }
    panic("No response to RARP");
    return SYSERR;
}


/*------------------------------------------------------------------------
 *  mkarp  -  allocate and fill in an ARP or RARP packet
 *------------------------------------------------------------------------
 */
static struct  ep  *mkarp(int ifn, short type, short op, IPaddr spa,IPaddr tpa)
{
    struct arp *parp;
	struct	ep	*pep;
    pep = (struct ep *) getbuf(Net.netpool);
    if (pep == NULL)
		return (struct ep *)SYSERR;
    blkcopy(pep->ep_dst, nif[ifn].ni_hwb.ha_addr, EP_ALEN);
    pep->ep_type = type;
	parp = (struct arp *)pep->ep_data;
    parp->ar_hwtype = hs2net(AR_HARDWARE);
	parp->ar_prtype = hs2net(EPT_IP);
	parp->ar_hwlen = EP_ALEN;
	parp->ar_prlen = IP_ALEN;
	parp->ar_op = hs2net(op);
    blkcopy(SHA(parp), nif[ifn].ni_hwa.ha_addr, EP_ALEN);
    blkcopy(SPA(parp), spa, IP_ALEN);
    blkcopy(THA(parp), nif[ifn].ni_hwa.ha_addr, EP_ALEN);
    blkcopy(TPA(parp), tpa, IP_ALEN);
    return pep;
}
