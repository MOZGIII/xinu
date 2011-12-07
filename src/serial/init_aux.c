/* init_aux.c - init_aux */

#include <kernel.h>
#include <conf.h>
#include <sl.h>

/*------------------------------------------------------------------------
 *  init_aux  --  initialize the Serial Line device pointed to by slp.
 *                Assumes the sl_base, sl_baud and sl_mode fields have 
 *                been set previsouly in slp.
 *------------------------------------------------------------------------
 */
init_aux(slp)
struct	slblk	*slp;
{
	int	k;
	int	base = slp->sl_port;
	int	mode = slp->sl_mode;
	int	baud = slp->sl_baud;
	
	outportb(SL_MCR(base),0);		/* reset the UART	*/
	inportb(SL_LSR(base));			/* reset line status	*/
	inportb(SL_DATAREG(base));		/* reset receive data	*/
	inportb(SL_MSR(base));			/* reset modem deltas	*/
	
	/* Set baud rate.  Must first open divisor latch for write 	*/

	outportb(SL_LCR(base),SL_DLA+mode); /* access divisor latch */
	outportb(SL_DLL(base),baud&0xff);	/* set low byte of baud	*/
	outportb(SL_DLH(base),(baud>>8)&0xff);	/* set high byte of baud*/
	
	outportb(SL_LCR(base),mode);		/* set mode & close latch*/
	
	/* Enable interrupts on 8259 and 8250				*/

        k = inportb(SL_IMR) & ~(1<<(slp->sl_vect-8));
	outportb(SL_IMR,k);
	outportb(SL_IER(base),SL_TXINT);
	outportb(SL_IER(base),SL_RXINT);
	outportb(SL_MCR(base),SL_DTR);
}
