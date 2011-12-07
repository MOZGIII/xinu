/* pipclose.c - pipclose */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <pipe.h>

/*-------------------------------------------------------------------
 *  pipclose  --  close a pipe device
 *-------------------------------------------------------------------
 */
pipclose(devptr)
struct  devsw   *devptr;
{
    struct  pipe    *pptr;
    int     ps;
    disable(ps);
    pptr = &pipe[devptr->dvminor];
    if (pptr->wpid == xinu_getpid()) {
        pptr->wpid = BADPID;
        xinu_signal(pptr->asem);
    }
    if (pptr->rpid == xinu_getpid())
        pptr->rpid = BADPID;
        
    if ( /* (pptr->wpid == BADPID) && */ (pptr->rpid == BADPID)) { 
        pptr->pstate = PCLOSED;
        sreset(pptr->ssem,1);
        sreset(pptr->asem,0);
    }
    restore(ps);
    return(OK);
}
