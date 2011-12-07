/* wakeup.c - wakeup */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>

/*------------------------------------------------------------------------
 *  wakeup  --  called by clock interrupt dispatcher to awaken processes
 *------------------------------------------------------------------------
 */
wakeup()
{
	register int	makeup;		/* makeup for lost time		*/
	register int	k;		/* key value			*/

	makeup = 0;
	while ( nonempty(clockq) && (k=firstkey(clockq)) <= makeup ) {
		makeup -= k;
		ready(getfirst(clockq));
	}
	if ( slnempty = nonempty(clockq) ) {
		sltop = &firstkey(clockq);
		*sltop -= makeup;
	}
	resched();
}
