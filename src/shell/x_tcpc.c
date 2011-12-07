/* x_tcpc.c - x_tcpc */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  x_tcpc  -  (command tcpc) close a TCP connection given its dev number
 *------------------------------------------------------------------------
 */
COMMAND x_tcpc(nargs, args)
int	nargs;
char	*args[];
{
    int fd;
    struct devsw    *devptr;
    struct  tcb     *ptcb;
    
    if (nargs != 2)
        xinu_fprintf(STDERR,"usage: tcpc devnum\n");
    
    fd = atoi(args[1]);
           
    devptr = &devtab[fd];
    ptcb = (struct tcb *)devptr->dvioblk;
/* close the connection if necessary */    
    if (ptcb->tcb_state != TCPS_CLOSEWAIT)
        xinu_close(fd);
/* clear the deletion event prematurely */
    tmclear(tcps_oport, (long) MKEVENT(DELETE,(ptcb-&tcbtab[0])));
    tcbdealloc(ptcb);
    return OK;
}
