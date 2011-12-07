/* slinit.c - slinit */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sl.h>

/*------------------------------------------------------------------------
 *  slinit  --  Serial Line Device initialization
 *------------------------------------------------------------------------
 */
slinit(devptr, flag)
struct	devsw	*devptr;
int flag;
{
	struct	slblk	*slp;
	int	pid;
#ifdef Nsl
    if (!flag) return OK;
    
	devptr->dvioblk = (char *)(slp=&sltab[devptr->dvminor]);
	slp->sl_dnum = devptr->dvnum;
	slp->sl_vect = devptr->dvivec;
	slp->sl_port = devptr->dvport;
	slp->sl_oldmcr = inportb(SL_MCR(slp->sl_port));
	slp->sl_baud = BAUD_2400;       
	slp->sl_mode = BITS8NOP;

	slp->sl_rsem = screate(1);
	slp->sl_wsem = screate(1);
	slp->sl_wstsem = screate(0);
	slp->sl_wpid = BADPID;
	
	slp->sl_rawptr = slp->sl_raw;
	slp->sl_rawct = 0;

	pid = create(SL_DAEMON,SL_DEMSTK,SL_DEMPRIO,SL_DEMNAME,2,slp);
	if ( pid == SYSERR ) {
        xinu_printf("slinit: Can't create serial line driver process\n");
		return(SYSERR);
	}
	ready(pid);

/* Inititalize Serial Line statistics	*/
 	slp->sl_int = 0;
 	slp->sl_tint = 0;
 	slp->sl_rint = 0;
 	slp->sl_lstat = 0;
 	slp->sl_mstat = 0;
 	slp->sl_iir = 0;
 	slp->sl_badesc = 0;
 	slp->sl_snd = 0;
 	slp->sl_wescct = 0;
 	slp->sl_notx = 0;
 	slp->sl_nord = 0;
 	slp->sl_badfend = 0;
 	slp->sl_rescct = 0;
	slp->sl_drop = 0;
 	slp->sl_rcv = 0;
 	slp->sl_reqtmo = 0;
 	slp->sl_sndtmo = 0;
	
	/* Initialize this serial port */
	init_aux(slp);
#endif    
	return(OK);
}
#ifdef Nsl
struct slblk sltab[Nsl];
#endif

