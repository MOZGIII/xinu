/* pipgetc.c - pipgetc */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <pipe.h>

/*-------------------------------------------------------------------
 *  pipgetc  --  read one character from a pipe device
 *-------------------------------------------------------------------
 */
pipgetc(devptr)
struct  devsw   *devptr;
{
    struct  pipe    *pptr;
    int     ps;
    char    ch;
    pptr = &pipe[devptr->dvminor];
    pptr->rpid = xinu_getpid();
    wait(pptr->asem);                    /* wait for data */
    disable(ps);
    if ((pptr->wpid == BADPID) && (scount(pptr->asem) == 0)) {
        restore(ps);
        return EOF;
    }
    ch = pptr->data;                    /* get the character       */
    restore(ps);
    xinu_signal(pptr->ssem);                 /* signal space available */
    return(ch);
}
