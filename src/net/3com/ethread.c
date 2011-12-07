/* ethread.c - ethread */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <pc.h>
#include <ec.h>
#include <dos.h>

/*------------------------------------------------------------------------
 *  ethread - read a single packet from the ethernet (3C503 Version)
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
    struct  netif   *pni;
    char    *buff;
	unsigned char	stat;		/* receive status */
    unsigned char   curr;       /* CURR pointer */
	unsigned char	bnry;		/* BNRY pointer */
	unsigned char	readpg;		/* shared memory read block page */
	unsigned char	endpg;		/* end page of shared memory buffer */
	unsigned char	nextpg;		/* next input page (linked) */
    unsigned char   *smp;       /* 3C503 shared memory pointer */

#define STRPG STRTPG-STTPG
#define STPPG STOPPG-STTPG

/* above defines required because NIC sees packet RAM at 0x2000 */

    if (len <= 0 || len > EP_MAXLEN) /* check length */
		return(SYSERR);

    etptr = (struct etblk *) devptr->dvioblk;
    pni = &nif[etptr->etintf];
    buff=&pep->ep_eh;           /* point past next hop IP and length */
	wait(etptr->etrsem);
	disable(ps);
	readlen = 0;
	while (readlen == 0) {
        outportb(CMDR,MSK_PG1);
        curr = inportb(CURR)-STTPG;
        outportb(CMDR,MSK_PG0);
        bnry = inportb(BNRY)-STTPG;
		readpg = bnry + 1;
        if (readpg >= STPPG)
            readpg = STRPG;
		if (readpg == curr) {	/* nothing in buffer -- wait around */
			etptr->etrpid = currpid;
            etptr->etrbuf = pep;
			etptr->etrsize = SYSERR;
			suspend(currpid);
			continue;
		}
        smp = MK_FP(etptr->etbase, (readpg << 8));
        slowcopy(buff, (readpg << 8), 4);   /* get first four bytes */
        nextpg = *(buff+1)-STTPG; /* next page of the packet */
/*        nextpg = *(smp+1)-STTPG;*/  /* beginning page of next packet */
        stat = *buff;                   /* status byte */
/*        stat = *smp; */
		if (stat & SMK_PRX) {
            readlen = (*(buff+2) & 0xff) + ((*(buff+3) << 8) & 0xff00);
/*            readlen = *(smp+2) + (*(smp+3) << 8); */
			readlen -= 4;	/* avoid copying CRC */
            if (readlen < EP_MINLEN) {
				etptr->drop++;
                pni->ni_idiscard++;
                etptr->shorts++;
				readlen = 0;
            } else if (readlen > EP_MAXLEN || readlen > len) {
				etptr->drop++;
                pni->ni_idiscard++;
				etptr->badsize++;
				readlen = 0;
            } else {    /* good packet -- snatch it */
                pep->ep_len = readlen;
                etptr->any++;
                endpg = readpg + *(buff+3)-STTPG;
/*                endpg = readpg + *(smp+3)-STTPG; */
/*                smp += 4; */                    /* ignore header */
                if (endpg >= STPPG) {
                    copylen = (int)(STPPG-readpg) << 8;
                    copylen -= 4;             /* no header to copy */
				} else
					copylen = readlen;
				if (copylen < readlen) {
					/* must do a two-part copy */
					/* first copy high memory pages */
                    slowcopy(buff, (readpg << 8)+4, copylen);
/*                    blkcopy(buff,smp,copylen); */
					buff += copylen;
/*                    smp = MK_FP(etptr->etbase,(STRPG<<8)); */
					/* now copy rest of low memory pages */
					copylen = readlen - copylen;
                    slowcopy(buff, (STRPG<<8), copylen);
/*                    blkcopy(buff,smp,copylen); */
				} else
					/* no need to wrap around */
                    slowcopy(buff, (readpg << 8)+4, readlen);                    
/*                    blkcopy(buff,smp,readlen); */
			}
        } else {
			etptr->drop++;
            pni->ni_idiscard++;
        }
		/* now forward the BNRY register */
		bnry = nextpg - 1;
        if (bnry < STRPG)
            bnry = STPPG - 1;
        outportb(BNRY,bnry+STTPG);
	}
    pep->ep_type = net2hs(pep->ep_type);
    
    xinu_signal(etptr->etrsem);
    restore(ps);
	return(readlen);
}
 
LOCAL slowcopy(buf, src, len)   /* get bytes from the NS8390 the slow way */
char    *buf;                   /* put the bytes here */
int     src;                    /* get the bytes from here in adapter RAM */
int     len;                    /* get this many bytes */
{
    int     i;
   
    outportb(DALSB, src & 0xff);            /* DMA LSB */
    outportb(DAMSB, ((src >> 8) & 0xff)+STTPG);  /* DMA MSB = rxbuf page */
    outportb(CTRL, START | XSEL);           /* start the GA DMA */
    
    for(i = 0; i < len; i++) {
        if ((i % 8) == 0)
            while ((inportb(STREG) & DPRDY) == 0) ; /* wait for FIFO ready */
        buf[i] = inportb(RFMSB);
    }
    outportb(CTRL, XSEL);                   /* stop the GA DMA */
    return OK;
}
