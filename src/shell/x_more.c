/* x_more.c - x_more */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  x_more  -  (command more) echo arg to STDOUT rowsize lines at a time
 *------------------------------------------------------------------------
 */
COMMAND x_more(nargs, args)
int     nargs;
char	**args[];
{
    char    ch;
    int     from, i , dev, count;
    struct  tty *iptr;

    if (nargs > 2) {
        xinu_fprintf(STDERR,"usage: more [filename]\n");
        return(SYSERR);
    }

    from = STDIN;
    if (nargs == 2) from=xinu_fopen(args[1],"ro");
    if (from == SYSERR) {
        xinu_fprintf(STDERR,"more: %s not found\n",args[1]);
        return(OK);
    }
    if (isatty(dev=getstdo())) {
        iptr = &tty[dev];
        count=iptr->rowsiz;
    }
    else count=0;
    i=1;
    while ((ch = getc(from)) != EOF) {
        putc(STDOUT,ch);
        if (count == 0) continue;
        if (ch=='\n') i++;
        if ((i % count) == 0) {
        xinu_fprintf(STDOUT,"--more--");
        switch (ch=getc(STDOUT)) {
            case EOF:       putc(STDOUT,'\n');
            case 'q':
            case 'Q':       return OK;
        }
        i=1;
        }

    }
	return(OK);
}
