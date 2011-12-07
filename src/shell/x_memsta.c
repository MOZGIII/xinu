/* x_memsta.c  -  x_memstat */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <dos.h>

/*------------------------------------------------------------------------
 *  x_memstat  --  (command memstat) print memory statistics
 *------------------------------------------------------------------------
 */
COMMAND  x_memstat(nargs,args)
int  nargs;
char *args[];
{
	char	*pp;
	int	bytes;
	word 	total,lp;
	unsigned long	ltotal, h, minh, maxh, avgh;
	word	nholes;
	int	ps;

	if ( nargs > 1 ) {
		xinu_fprintf(STDERR,"Usage: memstat\n");
		return(SYSERR);
	}
	disable(ps);
	total = 0;
	nholes = 0;
	minh = maxh = 0L;
	bytes = (int)memlist.mnext;
	while ( bytes != 0 ) {
		nholes++;
		pp = (char*)bytes;
		lp = memlen(pp);
		h = ((unsigned long)lp);
		if ( maxh < h )
			maxh = h;
		if ( minh > h || minh == 0L )
			minh = h;
		total += lp;
		bytes = (int)memnext(pp);
	}
	restore(ps);
	if ( total != memlist.mlen )
		xinu_printf("*inconsistent memory allocation*\n");
	ltotal = ((unsigned long)total);
	xinu_printf("%9lu total memory\n",ltotal);
	if ( nholes > 0 ) {
		xinu_printf("%9u hole%s\n",nholes,(nholes==1)?"":"s");
		if ( nholes > 1 ) {
			avgh = ltotal/nholes;
			xinu_printf("%9lu min hole size\n",minh);
			xinu_printf("%9lu max hole size\n",maxh);
			xinu_printf("%9lu avg hole size\n",avgh);
		}
	}
	return(OK);
}
