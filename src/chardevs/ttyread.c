/* ttyread.c - ttyread */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  ttyread  --  read one or more characters from a tty device
 *------------------------------------------------------------------------
 */
ttyread(devptr, buff, count)
struct devsw *devptr;
char *buff;
int count;
{
	register struct	tty *ttyp;
	int	ps;
	int	avail, nread;
	char	ch, eofch;
	int	donow, dolater;

	if ( count<0 )
		return(SYSERR);
	disable(ps);
	ttyp = &tty[devptr->dvminor];
	avail = scount( ttyp->isem );
	if ( avail<0 )
		avail=0;
	if (count==0) {
		if (avail == 0) {
			restore(ps);
			return(0);
		}
		count = avail;
	}
	if (count < avail) {
		donow = count;
		dolater = 0;	
	} else {
		donow = avail;
		dolater = count - avail;
	}
	nread = 0;
	if (donow >0) {
		ch = ttyp->ibuff[ttyp->itail++];
		ttyp->icnt--;
		if (ttyp->itail >= IBUFLEN)
			ttyp->itail = 0;
		if ( ((eofch=ttyp->ieofc) == ch) && ttyp->ieof) {
			sreset(ttyp->isem, avail-1);
			restore(ps);
			return(EOF);
		}
		*buff++ = ch;
		for(nread=1; nread < donow; ) {
			ch = ttyp->ibuff[ttyp->itail];
			if ( (ch==eofch) && ttyp->ieof) {
				sreset(ttyp->isem, avail - nread);
				restore(ps);
				return(nread);
			}
			*buff++ = ch;
			ttyp->icnt--;
			if (++ttyp->itail >= IBUFLEN)
				ttyp->itail = 0;
			nread++;
			if (ch == NEWLINE || ch == RETURN) {
				sreset(ttyp->isem, avail - nread);
				restore(ps);
				return(nread);
			}
		}
		sreset(ttyp->isem, avail - nread);
	}
	donow = nread;
	for (nread=0; nread < dolater; ) {
		wait(ttyp->isem);
		ch = ttyp->ibuff[ttyp->itail];
		if (ch == ttyp->ieofc && ttyp->ieof) {
			if (nread == 0 && donow == 0) {
				ttyp->icnt--;
				if (++ttyp->itail == IBUFLEN)
					ttyp->itail = 0;
				restore(ps);
				return(EOF);
			}
			xinu_signal(ttyp->isem);
			break;
		}
		*buff++ = ch;
		nread++;
		ttyp->icnt--;
		if (++ttyp->itail >= IBUFLEN)
			ttyp->itail = 0;
		if (ch == NEWLINE || ch == RETURN)
			break;
	}
	restore(ps);
	return(donow + nread);
}
