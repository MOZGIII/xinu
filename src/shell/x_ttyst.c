/* x_ttyst.c - x_ttyst */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <sem.h>

static struct	tsnap {
	int	ihead;			/* input queue head		*/
	int	itail;			/* input queue tail		*/
	int	isem;			/* input semaphore value	*/
	int	icnt;			/* input count			*/
	int	ohead;			/* output queue head		*/
	int	otail;			/* output queue tail		*/
	int	osem;			/* output semaphore value	*/
	int	ocnt;			/* output count			*/
	int	odsend;			/* output defer count		*/
	int	ehead;			/* echo queue head		*/
	int	etail;			/* echo queue tail		*/
	int	ecnt;			/* echo count			*/
	int	wstate;
	int	iintr;
	int	iintrc;
	int	iintpid;
	int	ocount;
} ttab[Ntty];


/*------------------------------------------------------------------------
 *  x_ttyst  --  (command ttystat) print tty statistics
 *------------------------------------------------------------------------
 */
COMMAND	x_ttyst(nargs, args)
int	nargs;
char	*args[];
{	
 	int	i,ps;
	struct	tsnap	*tabp;
	struct	tty	*ttyp;

	if ( nargs > 1 ) {
		xinu_fprintf	(STDERR,"Usage: ttyst\n");
		return(SYSERR);
	}
	xinu_printf("\ncurrent window = %d\n",winofcur);
 	xinu_printf("%2s %5s %5s %4s %4s %5s %5s %4s %4s %5s %5s %5s %5s\n",
		"wn","ihead","itail","isem","icnt","ohead","otail","osem",
		"ocnt","iintr","iintrc","iintpid","ocount");

	disable(ps);
	for (i=0 ; i<Ntty ; i++) {
		ttyp = &tty[i];
		tabp = &ttab[i];
		if ( (tabp->wstate=ttyp->wstate) > 0 ) {
			tabp->ihead   = ttyp->ihead;
			tabp->itail   = ttyp->itail;
			tabp->isem    = semaph[ttyp->isem].semcnt;
			tabp->icnt    = ttyp->icnt;
			tabp->ohead   = ttyp->ohead;
			tabp->otail   = ttyp->otail;
			tabp->osem    = semaph[ttyp->osem].semcnt;
			tabp->ocnt    = ttyp->ocnt;
			tabp->odsend  = ttyp->odsend;
			tabp->ehead   = ttyp->ehead; 
			tabp->etail   = ttyp->etail;  
			tabp->ecnt    = ttyp->ecnt;
			tabp->iintr   = ttyp->iintr;
			tabp->iintrc  = ttyp->iintrc;
			tabp->iintpid =ttyp->iintpid;
			tabp->ocount  = ttyp->ocount;
		}
	}
	restore(ps);
	for (i=0 ; i<Ntty ; i++) {
		tabp = &ttab[i];
		if ( tabp->wstate > 0 )
		xinu_printf("%2d %5d %5d %4d %4d %5d %5d %4d %4d %5d  %5d   %5d  %5d\n",
			i,tabp->ihead,tabp->itail,tabp->isem,tabp->icnt,
			tabp->ohead,tabp->otail,tabp->osem,tabp->ocnt,
			tabp->iintr,tabp->iintrc,tabp->iintpid,tabp->ocount);
	}		
	return(OK);
}
