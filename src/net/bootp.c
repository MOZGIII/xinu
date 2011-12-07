/* bootp.c - bootp_parse, bootp, make_bootp_packet, write_bootp_packet */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <proc.h>
#include <bufpool.h>

static char ip_str[] = "%u.%u.%u.%u";
static char ip_port_str[] = "%u.%u.%u.%u:%u";
extern IPaddr ip_maskall;

/*-------------------------------------------------------------------------
 * bootp_parse - parse a bootp response
 *-------------------------------------------------------------------------
 */
bootp_parse(pni, pep)
     struct netif *pni;
     struct ep *pep;
{
    
    unsigned char *ptr;
    unsigned long cookie;
    struct ip *pip;
    struct udp *pup;
    struct bootp_msg *bptr;
    unsigned char *vendata;
    IPaddr  mask;
    
    pip = (struct ip *) pep->ep_data;
    pup = (struct udp *) pip->ip_data;
    bptr = (struct bootp_msg *)pup->u_data;
    vendata = (unsigned char *)bptr->vend;
    
    ptr = vendata;
    cookie = *(unsigned long *)ptr;
    ptr += 4;
    if (net2hl(cookie) != RFC1084) {
	kprintf("bootp_parse: Incorrect RFC 1084 magic cookie in BOOTP reply.\n");
	return SYSERR;
    }
    
    /* grab my IP addr */
    blkcopy(pni->ni_ip, &bptr->yiaddr, IP_ALEN);
    pni->ni_ivalid = TRUE;
    
    while (ptr - vendata < SZVENDOR) {
	switch (*ptr++) {
	  case END:
	    return OK;
	  case PAD:
	    break;
	    
	  case TIME_SERVER:
        blkcopy(pni->ni_tserver, &ptr[1], IP_ALEN);
	    ptr = ptr + *ptr + 1;
	    break;
	  case DOMAIN_NAMESERVER:
        blkcopy(pni->ni_nserver, &ptr[1], IP_ALEN);
	    ptr = ptr + *ptr + 1;
        break;
	  case GATEWAY:
        blkcopy(pni->ni_gateway, &ptr[1], IP_ALEN);
	    ptr = ptr + *ptr + 1;
	    break;

      case SUBNETMASK:
        blkcopy(pni->ni_mask, &ptr[1], IP_ALEN);
/*        netmask(mask, pni->ni_mask);
        netnum(pni->ni_net, pni->ni_ip); 
        setmask(pni->ni_num, mask);
        pni->ni_svalid = TRUE;
*/        ptr = ptr + *ptr + 1;
        break;
        
	  default:
	    /* skip over by using the length field */
	    ptr = ptr + *ptr + 1;
	    break;
	}
    }
    return(OK);
}

int bootp_active;
int bootp_pid;

/*-------------------------------------------------------------------------
 * bootp - execute the BOOT PROTOCOL for a particular network interface
 *-------------------------------------------------------------------------
 */
bootp(pni)
    struct netif *pni;
{
    int tries;
    int status;
    int ret;

    bootp_pid = xinu_getpid();
    for (tries = 0; tries < BOOTP_MAXRETRY; ++tries) {
        recvclr();
        if (SYSERR == write_bootp_packet()) {
            ret = SYSERR;
            break;
        }
/*        status = recvtim((10 * BOOTP_RESEND) << tries); */
        status = recvtim(20);
        if (status == SYSERR) {
            ret = SYSERR;
            break;
        }
        else    if (status == TIMEOUT) {
                    ret = SYSERR;
                }
                else {                          /* bootp complete */
                    ret = status;
                    break;
                }
    }
    bootp_pid = BADPID;
    bootp_active = FALSE;
    return(ret);
}

/*-------------------------------------------------------------------------
 * bootp_eth_addr - 
 *-------------------------------------------------------------------------
 */
char *bootp_eth_addr()
{
    struct etblk *etptr;
    
    etptr = &eth[0];
    return etptr->etpaddr;
}


/*-------------------------------------------------------------------------
 * make_bootp_packet - 
 *-------------------------------------------------------------------------
 */
make_bootp_packet(bptr)
     struct bootp_msg *bptr;
{
    extern long hl2net(long);
    long magic = RFC1084;
    long netmagic = hl2net(magic);
    char *vp;
        
    bzero(bptr, BOOTP_SIZE);
    bptr->op = BOOTREQUEST;
    bptr->htype = ETHERNET;
    bptr->hlen = ELEN;
    bptr->xid = 47;		/* just a random number that's nonzero */
    blkcopy(bptr->chaddr, bootp_eth_addr(), EP_ALEN);
    blkcopy(bptr->vend, &netmagic, 4);

    vp=bptr->vend + 4;
    xinu_sprintf(vp,"\1\4\0\0\0\0"); /* subnet mask */
    vp +=6;
    xinu_sprintf(vp,"\3\4\0\0\0\0"); /* gateway */
    vp +=6;
    xinu_sprintf(vp,"\6\4\0\0\0\0"); /* domain name server */
    vp +=6;
    return OK;
}

/*-------------------------------------------------------------------------
 * write_bootp_packet - 
 *-------------------------------------------------------------------------
 */
write_bootp_packet()
{
    int tries;          /* THREE attempts */
    struct bootp_msg bppacket;
    struct ep *pep;
    struct ip *pip;
    struct udp *pup;

    make_bootp_packet(&bppacket);
    for (tries = 0; tries < 3; ++tries) {
        pep = (struct ep *) getbuf(Net.netpool);
        if (pep == (struct ep *) NULLPTR)
            return SYSERR;
	
        pip = (struct ip *)  pep->ep_data;
	
        pup = (struct udp *) pip->ip_data;
	
        /* set the BOOTP data */
        blkcopy(pup->u_data, &bppacket, BOOTP_SIZE);
	
        /* set the UDP header */
        pup->u_src = hs2net(NBOOTPC);
        pup->u_dst = hs2net(NBOOTPS);
        pup->u_len = hs2net(U_HLEN + BOOTP_SIZE);
        pup->u_cksum = 0;
	
        /* set the IP header */
        pip->ip_proto = IPT_UDP;
        pip->ip_verlen = (IP_VERSION<<4) | IP_MINHLEN;
        pip->ip_tos = 0;
        pip->ip_len = hs2net(IP_HLEN(pip) + U_HLEN + BOOTP_SIZE);
        pip->ip_id = 0;
        pip->ip_fragoff = 0;
        pip->ip_ttl = IP_TTL;
        bzero(pip->ip_src, IP_ALEN);
        blkcopy(pip->ip_dst, ip_maskall, IP_ALEN);
        pip->ip_cksum = 0;
        pip->ip_cksum = cksum(pip, IP_HLEN(pip)>>1);
	
        /* set the Ethernet header*/
        blkcopy(pep->ep_eh.eh_dst, EP_BRC, EP_ALEN);
        pep->ep_eh.eh_type = EPT_IP;
        bootp_active = TRUE;
        if (xinu_write(EC0, pep, EP_HLEN+U_HLEN+IP_HLEN(pip)+BOOTP_SIZE)
            != SYSERR) {
            return OK;
        }
    }
    return SYSERR;
}
