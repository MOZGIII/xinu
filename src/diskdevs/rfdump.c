/* rfdump.c - rfdump */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rfdump  --  dump the contents of the remote file device blocks
 *------------------------------------------------------------------------
 */
rfdump()
{
	struct	rfblk	*rfptr;
	int	i;

	for (i=0 ; i<Nrf; i++) {
		rfptr = &rftab[i];
		if (rfptr->rf_state == RFFREE)
			continue;
		kprintf(" %2d. ", rfptr->rf_dnum);
		kprintf("mode=%03o, sem=%2d, pos=%6D, file name=%s\n",
				rfptr->rf_mode,	rfptr->rf_mutex,
				rfptr->rf_pos, rfptr->rf_name);
	}
}
