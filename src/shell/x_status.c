/* x_status.c - x_status */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

extern	char	vers[];
LSYSCALL mem();

/*------------------------------------------------------------------------
 *  x_status  -  print system status at given intervals
 *------------------------------------------------------------------------
 */
COMMAND x_status(nargs, args)
int     nargs;
char	*args[];
{
	char	str[128];
    char    temp[32];
    int     i, t;
    long    now;
    
    if (nargs==2) 
        t=atoi(args[1]);
    else t=2;                           /* default is 2/10 seconds */
        
    while (TRUE) {
        xinu_strcpy(str,nif[1].ni_name);
        strcat(str,"            ");
        for (i=0; i<12; i++) 
            if (str[i] == '.') break;
        xinu_strcpy(&str[i],"            ");
        str[12]='\0';        
        xinu_sprintf(temp,"%3d Procs ", numproc);
        strcat(str, temp);
        xinu_sprintf(temp,"%6D Pkts Sent ", nif[1].ni_oucast+nif[1].ni_onucast);
        strcat(str, temp);
        xinu_sprintf(temp,"%6D Pkts Rcvd ", nif[1].ni_iucast+nif[1].ni_inucast);
        strcat(str, temp);
        xinu_sprintf(temp," %6D [%2d] ", mem(), xinu_getpid());
        strcat(str, temp);
        gettime(&now);
        ascdate(now, temp);
        strcat(str, &temp[12]);
        if (isatty(getstdo()) ) {
            strcat(str,"  ");
            show(str,0,1);
        } else xinu_printf(" %s\n",str);
        sleept(t);
    }
}
