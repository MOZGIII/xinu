#ifdef CMU_VERSION

/* send_ack - send_ack */

#include <kernel.h>
#include <conf.h>
#include <sl.h>

/*------------------------------------------------------------------------
 *  send_ack  --  Send an ACK by setting the ACK flag and kicking the 
 *                transmitter
 *------------------------------------------------------------------------
 */
send_ack(slptr) 
register struct slblk *slptr;
{
	slptr->TACK = TRUE;
	wake_serial(slptr);
}
#else
sned_ack(slptr)
{
    return -1;
}
#endif
