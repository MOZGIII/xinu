/* slinter.c - slinter */

#include <kernel.h>
#include <conf.h>
#include <proc.h>
#include <sl.h>

/*#define inportb(port) \
        _DX = port;   \
        asm in al,dx
#define outportb(port,ch) \
        _DX = port;       \
        _AL = ch;         \
        asm out dx,al*/
                
typedef unsigned char u_char;
typedef unsigned short u_short;

/*------------------------------------------------------------------------
 *  slinter  --  Serial Line Device ISR
 *------------------------------------------------------------------------
 */
INTPROC slinter(mdev)
int	mdev;
{
	struct	slblk	*sptr;
	register unsigned base;
	register int	ch;
	u_char	lsr;
	int	pid;
#ifdef Nsl
 	sptr = &sltab[mdev];
	sptr->sl_int++;
	base = sptr->sl_port;

	/* Determine the initial reason for SL interrupt */
    inportb(SL_IIR(base));
    switch ( _AL ) {
	case SL_RD:
        inportb(SL_IIR(base));
        if (_AL & (u_char)SL_RCHECK)
			goto sl_rchr;
		sptr->sl_nord++;
		break;

	case SL_WR:
        inportb(SL_LSR(base));
        if (_AL & (u_char)SL_TCHECK)
            goto sl_wchr;
		sptr->sl_notx++;
		break;

	case SL_LSTAT:
		sptr->sl_lstat++;
		inportb(SL_LSR(base));
		break;

	case SL_MSTAT:
		sptr->sl_mstat++;
		inportb(SL_MSR(base));
		break;

	default:
		sptr->sl_iir++;
		goto int_done;
	}

	/* Continue processing interrupt initiated above until finished */
	for (;;) {
        inportb(SL_LSR(base));
        lsr = _AL;
		if ( sptr->sl_rpid != BADPID && (lsr & SL_RCHECK) ) {
sl_rchr:
            sptr->sl_rint++;
            inportb(SL_DATAREG(base));
            sl_bin(_AL, sptr);
			continue;
		}
		if ( sptr->sl_wpid != BADPID && (lsr & SL_TCHECK) ) {
sl_wchr:
			sptr->sl_tint++;
			ch = sl_bout(sptr);
			if ( ch == SYSERR ) {
				outportb(SL_IER(base),SL_RXINT);
				continue;
			}
			outportb(SL_DATAREG(base),ch);
			pid = sptr->sl_wpid;
			if ( ch == SL_END ) {
				outportb(SL_IER(base),SL_RXINT);
				if ( pid != BADPID ) {
                    sendn(pid,SL_XMITDONE);
                    sptr->sl_wpid = BADPID;
				}
			}
			continue;
		}
		break;
	}
int_done:
	/* Tell the 8259 that we are done with this interrupt */
	outportb(SL_OCW2,SL_EOI);
#endif
}
