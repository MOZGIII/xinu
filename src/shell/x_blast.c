/* blast.c - x_blast */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  x_blast  --  blast out packets at top speed (about 8% on a PC)
 *------------------------------------------------------------------------
 */
COMMAND x_blast(argc,argv)
int	argc;
char    *argv[];
{
    struct ep *pep;
    int n, len;
    long time;

    n=1000;                         /* default values */
    len=60;
    
    switch (argc) {                 /* analyse arguments */
        case 1: break;
        case 3: len = atoi(argv[2]);
        case 2: n = atoi(argv[1]);
                break;
        default: xinu_fprintf(STDERR,"blast: too many arguments\n");
                return SYSERR;
    }
    time = SysUpTime;               /* note the time */
    
    while (n) {                     /* blast out packets */
        pep = (struct ep*)getbuf(Net.netpool);
        blkcopy(pep->ep_dst, nif[1].ni_hwb.ha_addr, EP_ALEN);
        pep->ep_type = EPT_ECHO;
        xinu_write(EC0, pep, len);
        n--;
    }
    xinu_printf("%D\n", SysUpTime - time);
    return OK; 
}
