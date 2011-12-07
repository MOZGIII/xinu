/* x_cat.c - x_cat */

#include <conf.h>
#include <kernel.h>
#include <io.h>

#define BUFSIZ	512

/*------------------------------------------------------------------------
 *  x_cat  -  (command cat) concatenate files and write on stdout
 *------------------------------------------------------------------------
 */
COMMAND	x_cat(nargs, args)
int nargs;
char *args[];
{
    int i, len;
	int	dev;
	char	*buf;

	args++;
	nargs--;
	if ( (buf=xmalloc(BUFSIZ)) == NULL ) {
		xinu_fprintf(STDERR,"cat: no memory\n");
		return(SYSERR);
	}
	if ( nargs == 0 ) {
        while ( (len=xinu_read(STDIN,buf,BUFSIZ)) > 0 )
			write(STDOUT,buf,len);
		return(OK);
	}
	while ( nargs > 0 ) {
        dev = xinu_fopen(*args, "ro");
		if ( isbaddev(dev) ) {
			xinu_fprintf(STDERR, "cat: cannot open %s\n",*args);
			return(SYSERR);
		}
		while ( (len=xinu_read(dev,buf,BUFSIZ)) > 0) {
            for (i=0 ; i<=len ; i++)
            buf[i]=buf[i] & 0x7f;
            xinu_write(STDOUT,buf,len);
		}
		xinu_close(dev);
		args++;
		nargs--;
	}
	return(OK);
}
