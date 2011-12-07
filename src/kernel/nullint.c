/* nullint.c - nullint */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 *  nullint  --  null interrupt service routine
 *------------------------------------------------------------------------
 */
INTPROC nullint(mdevno)
int mdevno;				/* minor device number		*/
{
	return;
}
