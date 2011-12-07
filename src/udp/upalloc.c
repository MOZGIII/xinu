/* upalloc.c - upalloc */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  upalloc  -  allocate a UDP port demultiplexing queue
 *------------------------------------------------------------------------
 */
int upalloc()
{
	struct	upq	*pup;
	int		i;

	wait(udpmutex);
	for (i=0 ; i<UPPS ; i++) {
		pup = &upqs[i];
		if (!pup->up_valid) {
			pup->up_valid = TRUE;
			pup->up_port = -1;
			pup->up_pid = BADPID;
			pup->up_xport = pcreate(UPPLEN);
			xinu_signal(udpmutex);
			return i;
		}
	}
	xinu_signal(udpmutex);
    return SYSERR;
}

struct	upq	upqs[UPPS];
