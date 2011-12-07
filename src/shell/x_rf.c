/* x_rf.c - x_rf */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  x_rf  -  (command rf) format and print remote file status
 *------------------------------------------------------------------------
 */
COMMAND x_rf(nargs, args)
int     nargs;
char	*args[];
{
	struct	rfblk	*rfptr;
	char	str[80];
	int	i;

	xinu_sprintf(str, "Remote files: server on dev=%d, server mutex=%d\n",
            Rf[0].rdev, Rf[0].rmutex);
    xinu_write(STDOUT, str, xinu_strlen(str));
	for (i=0 ; i<Nrf; i++) {
        if ((rfptr = &rftab[i])->rf_state == RFFREE)
			continue;
		xinu_sprintf(str,
			" %2d. name=%-20s, pos=%6D, mode=%03o, sem=%d\n",
				rfptr->rf_dnum, rfptr->rf_name,
				rfptr->rf_pos,  rfptr->rf_mode,
				rfptr->rf_mutex);
        xinu_write(STDOUT, str, xinu_strlen(str));
	}
	return(OK);
}
