/* x_cp.c - x_cp */

#include <conf.h>
#include <kernel.h>
#include <io.h>

#define BUFSIZ	512

static usage();

/*------------------------------------------------------------------------
 *  x_cp  -  (copy command) copy one file to another
 *------------------------------------------------------------------------
 */
COMMAND	x_cp(nargs, args)
int	nargs;
char	*args[];
{
	char	*buf;
    int from, to, i;
	int	ret;
	int	len;
	char	*wmode = "w";
	char	*rmode = "ro";
	char	*errfmt = "cp: cannot open %s\n";

	while (args[1][0] == '-') {
		switch (args[1][1]) {
		case 'b':
			wmode = "wb";
			rmode = "rbo";
			break;
		default:
			usage();
			return(SYSERR);
		}
		--nargs;
		args++;
	}
	if (nargs != 3) {
		usage();
		return(SYSERR);
	}
	if ( (from=xinu_fopen(args[1], rmode)) == SYSERR) {
		xinu_fprintf(STDERR, errfmt, args[1]);
		return(SYSERR);
	}
	if ( (to=xinu_fopen(args[2], wmode)) == SYSERR) {
		xinu_fprintf(STDERR, errfmt, args[2]);
		return(SYSERR);
	}
	if ( (buf=xmalloc(BUFSIZ)) == NULL ) {
		xinu_fprintf(STDERR, "no memory\n");
		ret = SYSERR;
	} else {
        while ( (len = xinu_read(from, buf, BUFSIZ)) > 0 )
            if ( xinu_write(to, buf, len) == SYSERR ) {
				xinu_fprintf(STDERR,"cp: write failure\n");
				break;	
            }
		ret = OK;
	}
    return(ret);
}

LOCAL
usage()
{
	xinu_fprintf(STDERR,"usage: cp [-b] source dest\n");
}
