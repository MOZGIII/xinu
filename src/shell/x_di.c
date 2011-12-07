/* x_di.c - x_di */

#include <conf.h>
#include <kernel.h>
#include <ec.h>

/*------------------------------------------------------------------------
 *  x_di  -  (command di) reinitialize a device
 *------------------------------------------------------------------------
 */
COMMAND x_di(nargs, args)
int	nargs;
char	**args[];
{
    int ps, dev;

    if (nargs != 2) {
        xinu_fprintf(STDERR,"usage: di dev\n");
        return(SYSERR);
    }
    args++;
    if ((dev=getdev(*args)) == SYSERR) {
        xinu_fprintf(STDERR,"di: %s is not a device\n",*args);
        return(SYSERR);
    }
    init(dev,0);                    /* uninitialize first */
    init(dev,1);                    /* now initialize it  */
    if (dev == EC0) {               /* extra stuff for Ethernet */
        efaceinit(1);
        disable(ps);                        /* turn off ints */
        outportb(TCR, 0);                   /* out of loopback */
        restore(ps);
        }  
    xinu_fprintf(STDERR,"\n");           /* skip over the error message */
    return(OK);
}
