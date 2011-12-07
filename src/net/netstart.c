/* netstart.c - netstart */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <network.h>
#include <snmp.h>
#include <tftp.h>
#include <q.h>

extern	int	bsdbrc;

extern int  ethout();

int udpecho();
int udpqd();
int rlogind();

/*------------------------------------------------------------------------
 *  netstart - initialize network software (using BOOTP)
 *------------------------------------------------------------------------
 */
PROCESS netstart(userpid)
int	userpid;			/* user process to resume	*/
{
	char str[128];
	struct	upq	*pup;
	long	now;
	int	i;
    IPaddr temp;

	/* initialize ports */

	for (i=0 ; i<UPPS ; i++)
		upqs[i].up_valid = FALSE;
	udpmutex = screate(1);

	arpinit();
	ipfinit();	/* init the IP frag queue */

	/* these need a command to set/clear them. FIX ME!!! */

    gateway = 0;
	IpForwarding = 2;	/* == 2 if not a gateway */
	IfNumber = Net.nif - 1;

    bsdbrc = 1;

    if (gateway) {
        initgate();
        temp[0]=137;
        temp[1]=92;
        temp[2]=11;
        temp[3]=54;
        rtadd(RT_DEFAULT, RT_DEFAULT, temp, RTM_INF-1, 1,
            RT_INF);
    } else
        inithost();
    rtadd(RT_LOOPBACK, ip_maskall, RT_LOOPBACK, 0, NI_LOCAL, RT_INF);

/* add a default route to point to our gateway */
        temp[0]=137;
        temp[1]=92;
        temp[2]=11;
        temp[3]=50; /* was .11.50 */

/*    rtadd(RT_DEFAULT, ip_maskall, temp, RTM_INF-1, 1, RT_INF);*/
    
	for (i=0; i<Net.nif; ++i) {
		nif[i].ni_ipinq = newq(NI_INQSZ, QF_NOWAIT);
		nif[i].ni_hwtype = AR_HARDWARE;		/* for ARP */
		/* no OTHER's for now */
        if (i < 2) 
            nif[i].ni_state = NIS_UP;
	}

	/*
	 * wait()'s synchronize to insure initialization is done
	 * before proceeding.
     */
    resume(create(slowtimer, STSTK, STPRI, STNAM, STARGC));
	wait(Net.sema);
    resume(create(ipproc, 1024, IPPRI, IPNAM, IPARGC));
	wait(Net.sema);
    resume(create(tcptimer, TMSTK, TMPRI, TMNAM, TMARGC));
	wait(Net.sema);
    resume(create(tcpinp, TCPISTK, TCPIPRI, TCPINAM, TCPIARGC));
	wait(Net.sema);
	resume(create(tcpout, TCPOSTK, TCPOPRI, TCPONAM, TCPOARGC));
	wait(Net.sema);
    for (i=1; i<Net.nif; i++) {
        resume(create(ethout, INITSTK, NETIPRI, "ethout", 1, i));
        wait(Net.sema);
        if (bootp(&nif[i]) == SYSERR)
            panic("Netstart: BOOTP failed\n");
/*        rtadd(RT_DEFAULT, ip_maskall, nif[i].ni_mask, 
                RTM_INF-1, 1, RT_INF);
*/        rtadd(RT_DEFAULT, ip_maskall, nif[i].ni_gateway, 
                RTM_INF-1, 1, RT_INF);
/* host route */
           rtadd(nif[i].ni_ip, ip_maskall, nif[i].ni_ip, 0,
                NI_LOCAL, RT_INF);
  
    }
    /* get addrs & names */
	for (i=0; i<Net.nif; ++i) {
		IPaddr junk;
		char junk2[64];
		if (i == NI_LOCAL)
			continue;
        if (nif[i].ni_state != NIS_UP)
			continue;
		xinu_sprintf(str,
        "NIF[%d] IP %u.%u.%u.%u ETH %02x:%02x:%02x:%02x:%02x:%02x ",
			i,
			nif[i].ni_ip[0]&0xff, nif[i].ni_ip[1]&0xff,
			nif[i].ni_ip[2]&0xff, nif[i].ni_ip[3]&0xff,
			nif[i].ni_hwa.ha_addr[0]&0xff,
			nif[i].ni_hwa.ha_addr[1]&0xff,
			nif[i].ni_hwa.ha_addr[2]&0xff,
			nif[i].ni_hwa.ha_addr[3]&0xff,
			nif[i].ni_hwa.ha_addr[4]&0xff,
            nif[i].ni_hwa.ha_addr[5]&0xff);
        xinu_write(CONSOLE, str, xinu_strlen(str));

        setmask(i, "\377\377\377\0");          /* fudge for testing */

        getiname(i, nif[i].ni_name);
        
        xinu_fprintf(CONSOLE, "%s\n", nif[i].ni_name);
    }

/*    resume(create(rip, INITSTK, RIPPRI, RIPNAM, RIPARGC)); */
      resume(create(snmpd, INITSTK, SNMPPRI, SNMPDNAM, 0)); 

    getutim(&now);
/*    rwho(); */
    resume(create(FINGERD, INITSTK, FNGDPRI, FNGDNAM, FNGDARGC));
    resume(create(ECHOD, INITSTK, ECHOPRI, ECHODNAM, 0));
    resume(create(udpecho, INITSTK, 50, "udpecho", 0));
    resume(create(tftpd,INITSTK,100,"tftpd",0));
    resume(create(rlogind,INITSTK,50,"rlogind",0));
    resume(userpid);
    return OK;
}

struct	netif		nif[NIF];
struct	netinfo		Net;
