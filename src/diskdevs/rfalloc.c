/* rfalloc.c - rfalloc */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rfalloc  --  allocate pseudo device for a remote file; return id
 *------------------------------------------------------------------------
 */
rfalloc()
{
	int	i;
	int	ps;

	disable(ps);
	for (i=0 ; i<Nrf ; i++)
		if (rftab[i].rf_state == RFFREE) {
			rftab[i].rf_state = RFUSED;
			restore(ps);
			return(i);
		}
	restore(ps);
	return(SYSERR);
}
