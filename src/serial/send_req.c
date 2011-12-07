#ifdef CMU_VERSION

/* send_req.c - send_req */

#include <kernel.h>
#include <conf.h>
#include <sl.h>

/*------------------------------------------------------------------------
 *  send_reg  --  Send a REQ by setting the REQ flag and kicking the 
 *                transmitter
 *------------------------------------------------------------------------
 */
send_req(slptr) 
register struct slblk *slptr;
{
	slptr->TREQ = TRUE;
	wake_serial(slptr); 
}
#else
send_req(slptr) 
register struct slblk *slptr;
{
    return -1;
}

#endif
