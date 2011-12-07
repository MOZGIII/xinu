/* pipwrite.c - pipwrite */

#include <conf.h>
#include <kernel.h>
#include <pipe.h>

/*-------------------------------------------------------------------
 *  pipwrite  --  write a buffer to a pipe device
 *-------------------------------------------------------------------
 */
pipwrite(devptr, buf, count)
struct  devsw   *devptr;
        char    *buf;
        int     count;
{
    int     i;
    if (count < 0) return(SYSERR);
    for (i=count; i>0; --i) 
        if (pipputc(devptr, *buf++) == SYSERR) return(SYSERR);
    return(count);
}
