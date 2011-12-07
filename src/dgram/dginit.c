/* dginit.c - dginit */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  dginit  -  initialize datagram protocol pseudo device marking it free
 *------------------------------------------------------------------------
 */
int dginit(pdev, flag)
struct	devsw	*pdev;
int flag;
{
	struct	dgblk	*pdg;

    if (!flag) return OK;
	pdev->dvioblk = (char *) (pdg = &dgtab[pdev->dvminor]);
	pdg->dg_dnum = pdev->dvnum;
	pdg->dg_state = DGS_FREE;
	dgmutex = screate(1);
	return OK;
}

#ifdef	Ndg
struct	dgblk	dgtab[Ndg];		/* dg device control blocks	*/
#endif
int	dgmutex;
