/* x_ping.c - x_ping */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <dos.h>

/*------------------------------------------------------------------------
 *  x_ping  -  do ICMP echo to a given destination
 *------------------------------------------------------------------------
 */
COMMAND x_ping(nargs, args)
int     nargs;
char	*args[];
{
	char	str[80], *p;
	IPaddr	dst;
    int got, i;
	int	np, ds;		/* # packets to send, data size */

	ds = 56;
	if (nargs == 3) {
		ds = atoi(args[2]);
        if (isodd(ds)) ds++;
    } else if (nargs != 2) {
		xinu_sprintf(str, "usage: ping host [size]\n");
        xinu_write(STDERR, str, xinu_strlen(str));
		return OK;
	}
    if (name2ip(dst, args[1]) == SYSERR) {
        xinu_fprintf(STDERR, "ping: couldn't resolve %s\n", args[1]);
        return SYSERR;
    }        
	got = 0;
    np = 10;
	i = 0;
	while(TRUE) {
		struct ep *packet;
        packet = (struct ep *)recvtim(10);  /* wait up to 1 sec */
        if (packet != (struct ep *)TIMEOUT) {
            struct ip *pip = (struct ip *)packet->ep_data;
			struct icmp *pic = (struct icmp *)pip->ip_data;
			xinu_sprintf(str, "%d octets from %u.%u.%u.%u: icmp_seq %d\n",
				pip->ip_len - IP_HLEN(pip),
				pip->ip_src[0]&0xff, pip->ip_src[1]&0xff,
				pip->ip_src[2]&0xff, pip->ip_src[3]&0xff,
				pic->ic_seq);
            xinu_write(STDOUT, str, xinu_strlen(str));
            freebuf(packet);
			got++;
		} else {
			if (i >= np)
				break;
            icmp(ICT_ECHORQ, 0 , dst, (long) i, (long) ds);
			i++;
		}
	}
	xinu_sprintf(str, "received %d/%d packets (%d %% loss)\n", got, 10,
		(100 * (10-got))/10);
    xinu_write(STDOUT, str, xinu_strlen(str));
	return(OK);
}
