/* pipputc.c - pipputc */

#include <conf.h>
#include <kernel.h>
#include <pipe.h>

/*-------------------------------------------------------------------
 *  pipputc  --  write one character to a pipe device
 *-------------------------------------------------------------------
 */
pipputc(devptr, ch)
struct  devsw   *devptr;
        char    ch;
{
    struct  pipe    *pptr;
    int     ps;
    pptr = &pipe[devptr->dvminor];
    if (pptr->pstate == PCLOSED) return SYSERR;
    pptr->wpid = xinu_getpid();
    wait(pptr->ssem);               /* wait for space in buff */
    disable(ps);
    pptr->data = ch;                /* put the character       */
    xinu_signal(pptr->asem);
    restore(ps);
    return(OK);
}
