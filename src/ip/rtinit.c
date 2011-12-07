/* rtinit.c - rtinit */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <network.h>
#include <bufpool.h>

struct  rtinfo  Route = {(struct route*)NULLPTR,0,0,0};
struct	route	*rttable[RT_TSIZE];

/*------------------------------------------------------------------------
 *  rtinit  -  initialize the routing table
 *------------------------------------------------------------------------
 */
void rtinit()
{
	int i;

	for (i=0; i<RT_TSIZE; ++i)
        rttable[i] = NULL;
	Route.ri_bpool = mkpool(sizeof(struct route), RT_BPSIZE);
	Route.ri_valid = TRUE;
	Route.ri_mutex = screate(1);
	Route.ri_default = NULL;
}
