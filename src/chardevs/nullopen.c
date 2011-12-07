/* nullopen.c - nullopen */

#include <conf.h>
#include <kernel.h>




/*------------------------------------------------------------------------
 *  nullopen  --  
 *------------------------------------------------------------------------
 */
int nullopen(devptr)
struct devsw *devptr;
{
	return(devptr->dvnum);
}
