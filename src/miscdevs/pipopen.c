/* pipopen.c - pipopen */

#include <conf.h>
#include <kernel.h>
#include <pipe.h>

/*-------------------------------------------------------------------
 *  pipopen  --  open a pipe device
 *-------------------------------------------------------------------
 */
pipopen(devptr, name, mode)
struct  devsw   *devptr;
char    *name;
char    *mode;
{
    struct  pipe    *pptr;
    int     ps, i;

    disable(ps);
    if ((i = pipalloc(mode)) != SYSERR) {
            pptr = &pipe[i];
            pptr->pstate = POPEN;
            restore(ps);
            return(pptr->dvnum);
    }
    restore(ps);
    return(SYSERR);
}
