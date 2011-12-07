/* x_ifstat.c - x_ifstat */

#include <conf.h>
#include <kernel.h>
#include <network.h>

static	char *sn[] = { "DOWN", "UP", "UNUSED" };

/*------------------------------------------------------------------------
 *  x_ifstat  -  print interface status information
 *------------------------------------------------------------------------
 */
COMMAND x_ifstat(nargs, args)
int     nargs;
char	*args[];
{
	char	str[80];
	char	*dn;
	struct	netif	*pni;
    int     iface;

	if (nargs != 2) {
        xinu_sprintf(str, "usage: ifstat <interface number>\n");
        xinu_write(STDERR, str, xinu_strlen(str));
		return OK;
	}
	iface = atoi(args[1]);
	if (iface < 0 || iface >= Net.nif) {
		xinu_sprintf(str, "ifstat: illegal interface (0-%d)\n", Net.nif-1);
        xinu_write(STDERR, str, xinu_strlen(str));
		return OK;
	}
	if (iface == NI_LOCAL) {
		xinu_sprintf(str, "Local interface state: %s\n",
			sn[nif[NI_LOCAL].ni_state]);
        xinu_write(STDOUT, str, xinu_strlen(str));
		return OK;
	}
	pni = &nif[iface];
	switch(pni->ni_dev) {
    case EC0: dn = "ec0";
			break;
#if	Noth > 0
	case OTHER1:	dn = "on0";
			break;
	case OTHER2:	dn = "on1";
			break;
#endif	/* Noth > 0 */
	default:
		dn = "??";
	}
	xinu_sprintf(str,"%s: state=%x<%s>\n",dn,pni->ni_state,sn[pni->ni_state]);
    xinu_write(STDOUT, str, xinu_strlen(str));
	if (pni->ni_ivalid)
		xinu_sprintf(str, "IP %u.%u.%u.%u NAME ", pni->ni_ip[0]&0xff,
			pni->ni_ip[1]&0xff, pni->ni_ip[2]&0xff,
			pni->ni_ip[3]&0xff);
	else
		xinu_sprintf(str, "IP <invalid> NAME ");
    xinu_write(STDOUT, str, xinu_strlen(str));
	if (pni->ni_nvalid)
		xinu_sprintf(str, "\"%s\"\n", pni->ni_name);
	else
		xinu_sprintf(str, "<invalid>\n");
    xinu_write(STDOUT, str, xinu_strlen(str));
	if (pni->ni_svalid)
        xinu_sprintf(str, "MASK %02x%02x%02x%02x BROADCAST %u.%u.%u.%u\n",
        pni->ni_mask[0], pni->ni_mask[1], pni->ni_mask[2], pni->ni_mask[3],
        pni->ni_brc[0]&0xff, pni->ni_brc[1]&0xff, pni->ni_brc[2]&0xff,
        pni->ni_brc[3]&0xff);
	else
		xinu_sprintf(str, "MASK <invalid> BROADCAST <invalid>\n");
    xinu_write(STDOUT, str, xinu_strlen(str));
	xinu_sprintf(str,"MTU %d HADDR %x:%x:%x:%x:%x:%x HBCAST %x:%x:%x:%x:%x:%x\n",
		pni->ni_mtu, pni->ni_hwa.ha_addr[0]&0xff,
		pni->ni_hwa.ha_addr[1]&0xff, pni->ni_hwa.ha_addr[2]&0xff,
		pni->ni_hwa.ha_addr[3]&0xff, pni->ni_hwa.ha_addr[4]&0xff,
		pni->ni_hwa.ha_addr[5]&0xff, pni->ni_hwb.ha_addr[0]&0xff,
		pni->ni_hwb.ha_addr[1]&0xff, pni->ni_hwb.ha_addr[2]&0xff,
		pni->ni_hwb.ha_addr[3]&0xff, pni->ni_hwb.ha_addr[4]&0xff,
		pni->ni_hwb.ha_addr[5]&0xff);
    xinu_write(STDOUT, str, xinu_strlen(str));
	xinu_sprintf(str, "inq %d\n", pni->ni_ipinq);
    xinu_write(STDOUT, str, xinu_strlen(str));
	return(OK);
}
