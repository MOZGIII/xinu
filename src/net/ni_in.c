/* ni_in.c - ni_in */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  ni_in - network interface input function
 *------------------------------------------------------------------------
 */
int ni_in(pni, pep, len)
struct	netif	*pni;		/* the interface	*/
struct  ep      *pep;       /* the packet       */
int             len;        /* length, in octets    */
{
	int	rv;
    
    if (bootp_active) {
        struct ip *pip;
        struct udp *pup;
        struct bootp_msg *bptr;
        int pid;
    
        pip  = (struct ip *) pep->ep_data;
        pup  = (struct udp *) pip->ip_data;
        bptr = (struct bootp_msg *) pup->u_data;
        if ((EPT_IP == pep->ep_type) &&
            (IPT_UDP == pip->ip_proto) && 
            (NBOOTPC == net2hs(pup->u_dst)) &&
            (BOOTREPLY == bptr->op)) {
            if (bootp_parse(pni, pep) == OK) {
                pid = bootp_pid;
                if (!isbadpid(bootp_pid)) {
                    bootp_pid = BADPID;
                    send(pid, OK);          /* this is safe during ints */
                }
                freebuf(pep);
            } else {
                freebuf(pep);
                }
        }
        goto unknown;
    }
    switch (pep->ep_type) {
	case EPT_ARP:	rv = arp_in(pni, pep);	break;
	case EPT_RARP:	rv = rarp_in(pni, pep);	break;
	case EPT_IP:	rv = ip_in(pni, pep);	break;
    default: unknown: pni->ni_iunkproto++;
                      freebuf(pep);
                      return OK;
	}
    pni->ni_ioctets += len;
	if (blkequ(pni->ni_hwa.ha_addr, pep->ep_dst, EP_ALEN))
        pni->ni_inucast++;
	else
        pni->ni_iucast++;
	return rv;
}
