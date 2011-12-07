/* ripout.c - ripout */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  ripout  -  do the RIP route exchange protocol, output side
 *------------------------------------------------------------------------
 */
PROCESS ripout()
{
	int	rnd;

	rippid = xinu_getpid();
	dorip = TRUE;
	/* advertise our routes */

	rnd = 0;	/* "randomizer" */
	while (TRUE) {
		sleep10(RIPOUTMIN);	/* minimum time between each	*/
		if (++rnd > RIPDELTA)
			rnd = -RIPDELTA;
		recvtim(RIPINT-RIPOUTMIN+rnd);
		wait(riplock);
        ripsend(ip_maskall, UP_RIP);
		xinu_signal(riplock);
	}
}
