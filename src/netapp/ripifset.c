/* ripifset.c - ripifset */

#include <conf.h>
#include <kernel.h>
#include <network.h>

struct  route   *rtget();

/*------------------------------------------------------------------------
 * ripifset - set the per-interface data for a RIP update
 *------------------------------------------------------------------------
 */
int ripifset(rqinfo, gw, port)
struct	rq	rqinfo[];
IPaddr		gw;		/* remote gateway (FFFFFFFF => all)	*/
unsigned short	port;		/* remote port				*/
{
	struct	route	*prt;
	int		ifn;

	if (!blkequ(gw, ip_maskall, IP_ALEN)) {
		for (ifn=0; ifn<Net.nif; ++ifn)
			rqinfo[ifn].rq_active = FALSE;
		prt = (struct route *)rtget(gw, RTF_LOCAL);
		if (prt == 0)
			return SYSERR;
		ifn = prt->rt_ifnum;
		rtfree(prt);
		if (ifn == NI_LOCAL)
			return SYSERR;
		blkcopy(rqinfo[ifn].rq_ip, gw, IP_ALEN);
		rqinfo[ifn].rq_port = port;
		rqinfo[ifn].rq_active = TRUE;
		rqinfo[ifn].rq_cur = -1;
		rqinfo[ifn].rq_nrts = MAXRIPROUTES;
		return OK;
	}
	/* else, all interfaces */
	for (ifn=0; ifn<Net.nif; ++ifn) {
		blkcopy(rqinfo[ifn].rq_ip, nif[ifn].ni_brc, IP_ALEN);
		rqinfo[ifn].rq_port = port;
		rqinfo[ifn].rq_active = TRUE;
		rqinfo[ifn].rq_cur = -1;
		rqinfo[ifn].rq_nrts = MAXRIPROUTES;
	}
	rqinfo[NI_LOCAL].rq_active = FALSE;	/* never do this one	*/
	return OK;
}
