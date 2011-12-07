/* getstdio.c - getstdi, getstdo */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  getstdi  --  return the device currently assigned as stdin
 *------------------------------------------------------------------------
 */
SYSCALL getstdi()
{
	return(proctab[currpid].pdevs[0]);
}

/*------------------------------------------------------------------------
 *  getstdo  --  return the device currently assigned as stdout
 *------------------------------------------------------------------------
 */
SYSCALL getstdo()
{
	return(proctab[currpid].pdevs[1]);
}
