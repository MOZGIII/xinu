/* xinu_getpid.c - xinu_getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  xinu_getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL xinu_getpid()
{
	return(currpid);
}
