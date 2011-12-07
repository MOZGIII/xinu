/* x_conf.c - x_conf */

#include <conf.h>
#include <kernel.h>
#include <network.h>

extern	char	vers[];

/*------------------------------------------------------------------------
 *  x_conf  -  Print info about the system configuration
 *------------------------------------------------------------------------
 */
COMMAND x_conf(nargs, args)
int     nargs;
char	*args[];
{
	char	str[128];
	char	name[80];
	char	*FindName();
	int	i;

	xinu_sprintf(str,"Identification\n");
	xinu_write(STDOUT, str, xinu_strlen(str));

	xinu_sprintf(str,"    Xinu version:     %s\n",vers);
	xinu_write(STDOUT, str, xinu_strlen(str));

	xinu_sprintf(str,"    Ethernet address: %02x:%02x:%02x:%02x:%02x:%02x\n",
		eth[0].etpaddr[0]&0xff,
		eth[0].etpaddr[1]&0xff,
		eth[0].etpaddr[2]&0xff,
		eth[0].etpaddr[3]&0xff,
		eth[0].etpaddr[4]&0xff,
		eth[0].etpaddr[5]&0xff);
	xinu_write(STDOUT, str, xinu_strlen(str));
	
	for (i=0; i<Net.nif; ++i) {
		if (nif[i].ni_state == NIS_DOWN)
			continue;
		if (i == NI_LOCAL)
			continue;
		xinu_sprintf(str,"    IP address:       %d.%d.%d.%d\n",
			(unsigned short) nif[i].ni_ip[0]&0xff,
			(unsigned short) nif[i].ni_ip[1]&0xff,
			(unsigned short) nif[i].ni_ip[2]&0xff,
			(unsigned short) nif[i].ni_ip[3]&0xff);
        xinu_write(STDOUT, str, xinu_strlen(str));
	xinu_sprintf(str,"    Domain name:      %s\n",
        ip2name(nif[i].ni_ip,name) == SYSERR ?  "UNKNOWN" : name );
        xinu_write(STDOUT, str, xinu_strlen(str));
	}


	xinu_sprintf(str,"\nTable sizes\n");
	xinu_write(STDOUT, str, xinu_strlen(str));

	xinu_sprintf(str,"    Number of processes:   %d\n",NPROC);
	xinu_write(STDOUT, str, xinu_strlen(str));

	xinu_sprintf(str,"    Number of semaphores:  %d\n",NSEM);
	xinu_write(STDOUT, str, xinu_strlen(str));

	xinu_sprintf(str,"    Number of devices:     %d\n",NDEVS);
	xinu_write(STDOUT, str, xinu_strlen(str));

	xinu_sprintf(str,"\nNetwork information\n");
	xinu_write(STDOUT, str, xinu_strlen(str));

#ifdef TSERVER
	xinu_sprintf(str,"    Time server:        %s (%s)\n",
		TSERVER, FindName(TSERVER));
	xinu_write(STDOUT, str, xinu_strlen(str));
#endif	

#ifdef RSERVER
	xinu_sprintf(str,"    Remote file server: %s (%s)\n",
		RSERVER, FindName(RSERVER));
        xinu_write(STDOUT, str, xinu_strlen(str));
#endif	

#ifdef NSERVER
        xinu_sprintf(str,"    Domain name server: %s (%s)\n",
		NSERVER, FindName(NSERVER));
	xinu_write(STDOUT, str, xinu_strlen(str));
#endif	

	return(OK);
}

char *FindName(strIP)
     char *strIP;
{
    static char name[80];
    IPaddr addr;

    dot2ip(addr, strIP);
    
    if (ip2name(addr, name) == SYSERR)
	return("UNKNOWN");
    else
    return(name);
}
