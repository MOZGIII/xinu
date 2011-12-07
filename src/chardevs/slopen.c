/* slopen.c - slopen */

#include <conf.h>
#include <kernel.h>


/*------------------------------------------------------------------------
 *  slopen  --  open the serial line
 *------------------------------------------------------------------------
 */
int slopen(devptr)
struct devsw *devptr;
{
	return(devptr->dvnum);
}
