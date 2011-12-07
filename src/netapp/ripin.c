/* ripin.c - rip */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  rip  -  do the RIP route exchange protocol
 *------------------------------------------------------------------------
 */
PROCESS rip()
{
	struct	xgram	ripbuf;
	struct	rip	*prip;
	int		fd, len;

	fd = xinu_open(UDP, ANYFPORT, UP_RIP);
	if (fd == SYSERR)
		panic("rip: cannot open rip port");
	riplock = screate(1);
	if (gateway)
		resume(create(ripout, RIPSTK, RIPPRI, RIPONAM, RIPOARGC));

	while (TRUE) {
		len = xinu_read(fd, &ripbuf, sizeof(ripbuf));
		if (len == SYSERR)
			continue;
		prip = (struct rip *)ripbuf.xg_data;
		if (ripcheck(prip, len) != OK)
			continue;
		switch (prip->rip_cmd) {
		case RIP_RESPONSE:
			if (ripbuf.xg_fport == UP_RIP)
				riprecv(prip, len, ripbuf.xg_fip);
			break;
		case RIP_REQUEST:
			if (gateway || ripbuf.xg_fport != UP_RIP)
				riprepl(prip, len, ripbuf.xg_fip,
					ripbuf.xg_fport);
			break;
		default:
			break;
		}
	}
}

Bool	dorip = FALSE;
int	rippid = BADPID;
int	riplock;
