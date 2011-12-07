/* mem.c  -  mem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <dos.h>

#define STATUS getdev("status")

/*------------------------------------------------------------------------
 *  mem  --  check memory 
 *------------------------------------------------------------------------
 */
LSYSCALL mem()
{
	char	*pp;
	int	bytes;
	word 	total,lp;
	unsigned long	ltotal, h, minh, maxh, avgh;
	word	nholes;
	int	ps;

	disable(ps);
	total = 0;
	nholes = 0;
	minh = maxh = 0L;
	bytes = (int)memlist.mnext;
	while ( bytes != 0 ) {
		nholes++;
		pp = (char*)(bytes);
		lp = memlen(pp);
		total += lp;
		bytes = (int)memnext(pp);
	}
	restore(ps);
	if ( total != memlist.mlen )
    	    xinu_fprintf(STATUS,"\n ** inconsistent memory allocation **");
    return(total);
}
