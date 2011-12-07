/* wake_ser.c - wake_serial */

#include <kernel.h>
#include <conf.h>
#include <sl.h>

/*------------------------------------------------------------------------
 *  wake_serial  --  kick start the serial line device
 *------------------------------------------------------------------------
 */
wake_serial(slptr)
register struct slblk *slptr;
{
	int	base;
	char	junk;
	
	base = slptr->sl_port;
	junk = (inportb(SL_IER(base)) | SL_TXINT) & 0xff;
	outportb(SL_IER(base),junk);
}
