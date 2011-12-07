/* rtfree.c - rtfree */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  rtfree  -  remove one reference to a route
 *------------------------------------------------------------------------
 */
int rtfree(prt)
struct	route	*prt;
{
	if (!Route.ri_valid)
		return SYSERR;
	wait(Route.ri_mutex);
	RTFREE(prt);
	xinu_signal(Route.ri_mutex);
	return OK;
}
