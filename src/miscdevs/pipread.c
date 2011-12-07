/* pipread.c - pipread */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <pipe.h>

/*-------------------------------------------------------------------
 *  pipread  --  read one or more characters from a pipe device
 *-------------------------------------------------------------------
 */
pipread(devptr, buf, count)
struct  devsw   *devptr;
        char    *buf;
        int     count;
{
    struct  pipe    *pptr;
    int     ps;
    int     avail, n;
    char    ch;

    if (count < 0) return(SYSERR);

    pptr = &pipe[devptr->dvminor];
    pptr->rpid = xinu_getpid();

    disable(ps);

/* is data available ? */
    
    if ((avail = scount(pptr->asem)) < 0) avail = 0;
    if (count == 0 && (count=avail) == 0) {
        restore(ps);
        return(0);
    }
    
/* get requested number of characters up to \n */

    ch = '\0';
    for (n=0; n<count && ch!='\n'; n++){
        if ((ch=pipgetc(devptr)) < 0) {
            n = EOF;
            break;
        }
        *buf++ = ch;
    }
    restore(ps);
    return(n);
}
