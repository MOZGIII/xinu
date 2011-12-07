/* x_dgstat.c - x_dgstat */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  x_dgstat  -  (command dgstat) print info on open datagram devices
 *------------------------------------------------------------------------
 */
COMMAND
x_dgstat(nargs, args)
int     nargs;
char	*args[];
{
	struct	dgblk	*pdg;
	char	str[80];
	int	i;

	for (i=0 ; i<Ndg ; i++) {
		pdg = &dgtab[i];
		if (pdg->dg_state == DGS_FREE)
			continue;
		xinu_sprintf(str, "Dev=%2d: lport=%4d, fport=%4d, ",
			pdg->dg_dnum, pdg->dg_lport, pdg->dg_fport);

		xinu_sprintf(&str[xinu_strlen(str)],
			"mode=%03o, xport=%2d addr=%d.%d.%d.%d\n",
				pdg->dg_mode, pdg->dg_xport,
				pdg->dg_fip[0] &0377,
				pdg->dg_fip[1] &0377,
				pdg->dg_fip[2] &0377,
				pdg->dg_fip[3] &0377);
        xinu_write(STDOUT, str, xinu_strlen(str));
	}
	return(OK);
}
