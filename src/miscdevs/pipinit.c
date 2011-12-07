/* pipinit.c - pipinit */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <pipe.h>

struct pipe pipe[Npip];

/*--------------------------------------------------------------------------
 *  pipinit  --  initialize pipe
 *--------------------------------------------------------------------------
 */
pipinit(devptr,flag)
struct  devsw *devptr;
int     flag;
{
    struct  pipe *pptr;

    if (flag == 0) return(OK);
    pptr = &pipe[devptr->dvminor];
    pptr->dvnum = devptr->dvnum;
    pptr->pstate = PCLOSED;
    pptr->wpid = BADPID;
    pptr->rpid = BADPID;
    pptr->ssem = screate(1);        /* data space available */
    pptr->asem = screate(0);        /* no data available yet */
    return(OK);
}
