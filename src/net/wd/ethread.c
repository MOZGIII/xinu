/* ethread.c - ethread */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <pc.h>
#include <ec.h>
#include <dos.h>

/*------------------------------------------------------------------------
 *  ethread - read a single packet from the ethernet
 *------------------------------------------------------------------------
 */
ethread(devptr, pep, len)
struct	devsw	*devptr;
struct  ep      *pep;
int             len;
{
	int	ps;
	int	readlen;		/* no. of bytes read */
	int	copylen;
	struct	etblk	*etptr;
    char    *buff;
	register unsigned	ieport;
	unsigned char	stat;		/* receive status */
    unsigned char   curr;       /* CURR pointer */
	unsigned char	bnry;		/* BNRY pointer */
	unsigned char	readpg;		/* shared memory read block page */
	unsigned char	endpg;		/* end page of shared memory buffer */
	unsigned char	nextpg;		/* next input page (linked) */
	unsigned char	*smp;		/* WD8003E shared memory pointer */

    if (len <= 0 || len > EP_MAXLEN) /* check length */
		return(SYSERR);

    etptr = (struct etblk *) devptr->dvioblk;
	ieport = etptr->etport;
    buff=&pep->ep_eh;           /* point past next hop IP and length */
	wait(etptr->etrsem);
	disable(ps);
	readlen = 0;
	while (readlen == 0) {
		outportb(CMDR(ieport),MSK_PG1);
		curr = inportb(CURR(ieport));
		outportb(CMDR(ieport),MSK_PG0);
		bnry = inportb(BNRY(ieport));
		readpg = bnry + 1;
		if (readpg >= STOP_PG)
			readpg = STRT_PG;
		if (readpg == curr) {	/* nothing in buffer -- wait around */
			etptr->etrpid = currpid;
            etptr->etrbuf = pep;
			etptr->etrsize = SYSERR;
			suspend(currpid);
			continue;
		}
		smp = MK_FP(etptr->etbase, (readpg << 8));
		nextpg = *(smp+1);	/* beginning page of next packet */
		stat = *smp;
		if (stat & SMK_PRX) {
			readlen = *(smp+2) + (*(smp+3) << 8);
			readlen -= 4;	/* avoid copying CRC */
            if (readlen < EP_MINLEN) {
				etptr->drop++;
				etptr->shorts++;
				readlen = 0;
            } else if (readlen > EP_MAXLEN || readlen > len) {
				etptr->drop++;
				etptr->badsize++;
				readlen = 0;
            } else {    /* good packet -- snatch it */
                pep->ep_len = readlen;
                etptr->any++;
				endpg = readpg + *(smp+3);
                smp += 4;                     /* ignore header */
				if (endpg >= STOP_PG) {
					copylen = (int)(STOP_PG-readpg) << 8;
                    copylen -= 4;             /* no header to copy */
				} else
					copylen = readlen;
				if (copylen < readlen) {
					/* must do a two-part copy */
					/* first copy high memory pages */
                    blkcopy(buff,smp,copylen);
					/* Thanks to Andy Marks for fixes */
					buff += copylen;
					smp = MK_FP(etptr->etbase,(STRT_PG<<8));
					/* now copy rest of low memory pages */
					copylen = readlen - copylen;
					blkcopy(buff,smp,copylen);
				} else
					/* no need to wrap around */
					blkcopy(buff,smp,readlen);
			}
		} else
			etptr->drop++;
		/* now forward the BNRY register */
		bnry = nextpg - 1;
		if (bnry < STRT_PG)
			bnry = STOP_PG - 1;
		outportb(BNRY(ieport),bnry);
	}
    pep->ep_type = net2hs(pep->ep_type);
    xinu_signal(etptr->etrsem);
    restore(ps);
	return(readlen);
}
