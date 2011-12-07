/* dgwrite.c - dgwrite */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>
#include <dos.h>

/*------------------------------------------------------------------------
 *  dgwrite  -  write one datagram to a datagram pseudo-device
 *------------------------------------------------------------------------
 */
int dgwrite(pdev, pxg, len)
struct	devsw	*pdev;
struct	xgram	*pxg;
int		len;
{
    struct  ep  *pep;
	struct	ip	*pip;
	struct	udp	*pudp;
	struct	dgblk	*pdg;
	int		dst;		/* destination UDP port		*/
	char		*pipa;		/* destination IP address	*/

    if (len < 0 || len > U_MAXLEN)
		return SYSERR;
	pdg = (struct dgblk *) pdev->dvioblk;
    pep = (struct ep*)getbuf(Net.netpool);

    if (pep == NULL)
		return SYSERR;
	pip = (struct ip *) pep->ep_data;

	pudp = (struct udp *) pip->ip_data;
    dst = pdg->dg_fport;
	pipa = (char *) pdg->dg_fip;
	if ((pdg->dg_mode & DG_NMODE) != 0) {
		if (dst == 0) {
			dst = pxg->xg_fport;
			pipa = (char *) pxg->xg_fip;
		}
		blkcopy(pudp->u_data, pxg->xg_data, len);
	} else {
		if ( dst == 0) {
            freebuf(pep);
			return SYSERR;
		}
		blkcopy(pudp->u_data, pxg, len);
	}
    return udpsend(pipa, dst, pdg->dg_lport, pep, len,
		pdg->dg_mode & DG_CMODE);
}
