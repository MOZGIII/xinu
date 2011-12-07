/* userret.c - userret */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 *  userret  --  entered when a process exits by return
 *------------------------------------------------------------------------
 */
userret()
{
	int	pid = xinu_getpid();

    die(pid);
	kprintf("Unable to kill process %d",pid);
	panic("Fatal system error");
}
