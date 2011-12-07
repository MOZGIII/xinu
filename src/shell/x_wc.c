/* x_wc.c - x_wc */

#include <conf.h>
#include <kernel.h>
#include <ctype.h>
#include <mffile.h>

/*------------------------------------------------------------------------
 *  x_wc  -  (command wc) count words read from STDIN
 *------------------------------------------------------------------------
 */
COMMAND x_wc(nargs, args)
int	nargs;
char	**args[];
{
     int dev, cc, wc, lc, flagged, chr;
     
     cc = 0;
     wc = 0;
     lc = 0;
     
     flagged = 0;
     chr = ' ';
     
     while( isspace( chr ) ) {
          chr = getc(STDIN);
          cc++;
        }
     
    while(chr >= 0)
    {
        if( !isspace( chr ) && !flagged ) {
            flagged = 1;
            wc++;
        }
        if( isspace( chr ) )
            flagged = 0;
        chr = getc(STDIN);
        if (chr == '\n') lc++;
        cc++;
    }
    xinu_printf("%d %d %d ", cc, wc, lc);
    if (isanmf(dev=getpdev(xinu_getpid(), STDIN)))
        xinu_printf("%s\n", mftab[devtab[dev].dvminor].mf_nam);
    else xinu_printf("\n");     
}
