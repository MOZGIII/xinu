/* x_slstat.c  -  x_slstat */

#include <conf.h>
#include <kernel.h>
#include <sl.h>

/*------------------------------------------------------------------------
 *  x_slstat  --  (command slstat) print SLIP statistics
 *------------------------------------------------------------------------
 */
COMMAND  x_slstat(nargs,args)
int  nargs;
char *args[];
{
	int	x;
	struct	slblk	*slp;
	
	if ( nargs != 1 ) {
		xinu_fprintf(STDERR,"Usage: slstat\n");
		return(SYSERR);
	}
	
	for(x=0; x<Nsl; x++) {
		slp = &sltab[x];
		xinu_printf("Serial device #%d, major device #%d\n",x+1,
			slp->sl_dnum);
		xinu_printf("              Send     Receive\n");
		xinu_printf("Interupts   %6ld      %6ld (bad=%ld)\n",
			slp->sl_tint,slp->sl_rint,slp->sl_iir);
		xinu_printf("Ghost Int   %6ld      %6ld\n",
			slp->sl_notx,slp->sl_nord);
		xinu_printf("Timeouts    %6ld      %6ld\n",
			slp->sl_sndtmo,slp->sl_reqtmo);
		xinu_printf("Packets     %6ld      %6ld (drop=%ld)\n",
			slp->sl_snd,slp->sl_rcv,slp->sl_drop);
		xinu_printf("Escapes     %6ld      %6ld (bad=%ld)\n",
			slp->sl_wescct,slp->sl_rescct,slp->sl_badesc);
		xinu_printf("Phantom frame ends = %ld\n",slp->sl_badfend);
	}
	
	return(OK);
}
