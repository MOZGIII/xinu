/* tcpiss.c - tcpiss */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	TCPINCR		904

/*------------------------------------------------------------------------
 *  tcpiss -  set the ISS for a new connection
 *------------------------------------------------------------------------
 */
long tcpiss()
{
    static  long    seq = 0;
	extern	long	clktime;		/* the system ticker	*/

	if (seq == 0)
		seq = clktime;
	seq += TCPINCR;
	return seq;
}
