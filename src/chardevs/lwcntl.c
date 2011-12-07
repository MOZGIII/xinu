/* lwcntl.c - lwcntl */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <tty.h>

/*------------------------------------------------------------------------
 *  lwcntl  --  do misc. tty control functions
 *------------------------------------------------------------------------
 */
int lwcntl(devptr, func, arg1)
struct devsw *devptr;
int func;
char *arg1;
{
	register struct	tty *ttyp;
	int	i;
	int	ps;
	int	c;
	int	attr;

	ttyp=&tty[devptr->dvminor];
	disable(ps);
	if ( ttyp->wstate != LWALLOC ) {/* can't access unopened window	*/
		restore(ps);
		return(SYSERR);
	}
again:
	c = OK;				/* assume the best		*/
	switch ( func )	{
	case TCNEXTC:
		wait(ttyp->isem);
		c = ttyp->ibuff[ttyp->itail];
		xinu_signal(ttyp->isem);
		break;
	case TCNEXTCI:
		if ( scount(ttyp->isem) > 0 ) {
			c = ttyp->ibuff[ttyp->itail];
		} else
			c = SYSERR;
		break;
	case TCMODER:
		if ( ttyp->imode == IMCOOKED )
			signaln(ttyp->isem,ttyp->icursor);
		ttyp->imode = IMRAW;
		break;
	case TCMODEC:
		if ( ttyp->imode != IMCOOKED )
			ttyp->icursor = 0;
		ttyp->imode = IMCOOKED;
		break;
	case TCMODEK:
		if ( ttyp->imode == IMCOOKED )
			signaln(ttyp->isem,ttyp->icursor);
		ttyp->imode = IMCBREAK;
		break;
	case TCMODEQ:
		*((int *)arg1) = ttyp->imode;
		break;
	case TCMODES:
		i = *((int *)arg1);
		switch (i) {
		case IMCOOKED:
			func = TCMODEC;
			goto again;
		case IMCBREAK:
			func = TCMODEK;
			goto again;
		case IMRAW:
			func = TCMODER;
			goto again;
		default:
			c = SYSERR;
		}
		break;
	case TCECHO:
		ttyp->iecho = TRUE;
		break;
	case TCNOECHO:
		ttyp->iecho = FALSE;
		break;
	case TCECHOQ:
		*((int *)arg1) = ttyp->iecho;
		break;
	case TCECHOS:
		ttyp->iecho = *((int *)arg1) ? TRUE : FALSE;
		break;
	case TCICHARS:
		c = scount(ttyp->isem);
		break;
	case TCFLUSH:
		if ( ttyp->imode == IMCOOKED ) {
			ttyp->ihead -= ttyp->icursor;
			ttyp->icnt -= ttyp->icursor;
			if ( ttyp->ihead < 0 )
				ttyp->ihead += IBUFLEN;
			ttyp->icursor =	0;
		}
		break;
	case TCINT:
		ttyp->iintr = TRUE;
		ttyp->iintpid = xinu_getpid();
		break;
	case TCINTQ:
		if (ttyp->iintr == TRUE)
			c = ttyp->iintpid;
		else
			c = SYSERR;
		break;
	case TCINTS:
		ttyp->iintr = TRUE;
		ttyp->iintpid = *((int *)arg1);
		break;
	case TCNOINT:
		ttyp->iintr = FALSE;
		break;
	case TCCLEAR:
		scrollup(ttyp->topleft,ttyp->botright,0,ttyp->attr);
		ttyp->curcur.row=0;
		ttyp->curcur.col=0;
		wputcsr(ttyp,ttyp->curcur);
		break;
	case TCCURPOS:
		c = lwgoto(ttyp,arg1);
		break;
    case TCFASTP:
		putchr(*arg1, 1, 0);
    		break;    
	case TCATTR:
		if ( (attr=lwattr(arg1,ttyp->attr)) != SYSERR )
			ttyp->attr=attr;
		else 
			c = SYSERR;
		break;
	default:
		c = SYSERR;
	}
	restore(ps);
	return(c);
}
