/* dgparse.c - dgparse */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ctype.h>

/*------------------------------------------------------------------------
 *  dgparse  -  parse foreign address specification; get IP and port #s
 *------------------------------------------------------------------------
 */
int dgparse(pdg, fspec)
struct	dgblk	*pdg;
char		*fspec;
{
	int	i, byte;
	char	ch, *pipa;

    if (fspec == ANYFPORT) {
		pdg->dg_fport = 0;
        for (i=0 ; i<IP_ALEN ; ++i) 
            pdg->dg_fip[i]= 0;
		return(OK);
	}

	/* parse forms like 192.5.48.30:3 into (ip-address,udp-port) */

	pipa = (char *) pdg->dg_fip;
	for (i=0 ; i<IP_ALEN ; ++i) {
		byte = 0;
		while (isdigit(ch = *fspec++))
			byte = 10*byte + (ch - '0');
		if (byte > 256 || (i<(IP_ALEN-1) && ch != '.'))
			return SYSERR;
		*pipa++ = (char) byte;
	}
	if (ch != ':')
		return SYSERR;
	i = 0;
	while (isdigit(ch = *fspec++) )
		i = 10*i + (ch - '0');
	if (i==0 || ch != '\0')
		return SYSERR;
	pdg->dg_fport = i;
	return OK;
}
