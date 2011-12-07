/* recvclr.c - recvclr */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  recvclr  --  clear messages, returning waiting message (if any)
 *------------------------------------------------------------------------
 */
LSYSCALL recvclr()
{
    int  ps;
    long msg;

	disable(ps);
	if (proctab[currpid].phasmsg) {		/* existing message?	*/
		proctab[currpid].phasmsg = 0;
		msg = proctab[currpid].pmsg;
	} else
		msg = OK;
	restore(ps);
	return(msg);
}
