/* ethdemux.c - ethdemux */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <ec.h>
#include <bufpool.h>
LOCAL slowcopy(unsigned char *buf, int src, int len);
/*------------------------------------------------------------------------
 *  ethdemux - get a packet for ethinter (3C503 Version)
 *------------------------------------------------------------------------
 */
int ethdemux(etptr)
struct  etblk   *etptr;
{
    int readlen;                /* no. of bytes read */
	int	copylen;
    int ifnum, oldprio;
    Bool pool;                  /* TRUE if buff is from pool */
    unsigned char spare[4];     /* four spare bytes for emergencies */
    struct  ep      *pep;
    unsigned char   *buff;      /* we put the packet here */
	unsigned char	stat;		/* receive status */
    unsigned char   curr;       /* CURR pointer */
	unsigned char	bnry;		/* BNRY pointer */
	unsigned char	readpg;		/* shared memory read block page */
	unsigned char	endpg;		/* end page of shared memory buffer */
	unsigned char	nextpg;		/* next input page (linked) */

#define STRPG STRTPG-STTPG      /* NIC sees Packet RAM at 0x2000 */
#define STPPG STOPPG-STTPG

    int slowcopy(unsigned char *, int, int);

    ifnum = etptr->etintf;
    pep = (struct ep *)getbufi(Net.netpool);    /* musn't wait */
    if (pep == NULL) {     /* none left but we must still handle the NIC */
        nif[ifnum].ni_idiscard++;
        pool = FALSE;   
        buff = spare;
    } else {
       pool = TRUE;
       buff = (unsigned char*)&pep->ep_eh;           /* point past next hop IP and length */
    }
    
/* transfer the packet from Packet RAM */
    
    outportb(CMDR,MSK_PG1);
    curr = inportb(CURR)-STTPG;
    outportb(CMDR,MSK_PG0);
    bnry = inportb(BNRY)-STTPG;
    readpg = bnry + 1;
    if (readpg >= STPPG)
        readpg = STRPG;
    if (readpg == curr) {           /* nothing to do */
        if (pool) freebuf(pep);       
        return SYSERR;
        }

    readlen = 0;    
    slowcopy(buff, (int)(readpg << 8), 4);  /* get first four bytes */
    nextpg = buff[1]-STTPG;                 /* page of next packet */
    stat = buff[0];                         /* status byte */
    if ((stat & SMK_PRX) && pool) {
        readlen = buff[2] + (int)(buff[3] << 8);
        readlen -= 4;               /* forget the FCS */
        if ((readlen < EP_MINLEN) || (readlen > EP_MAXLEN)) {
            nif[ifnum].ni_idiscard++;
            freebuf(pep);
            readlen = 0;
         } else {    /* good packet -- snatch it */
              pep->ep_len = readlen;
              etptr->any++;
              endpg = readpg + buff[3];
              if (endpg >= STPPG) {
                   copylen = (int)((STPPG-readpg) << 8);
                   copylen -= 4;             /* no header to copy */
              } else
                   copylen = readlen;
              if (copylen < readlen) {      /* must do a two-part copy */
                    /* first copy high memory pages */
                   slowcopy(buff, (int)(readpg << 8)+4, copylen);
                   buff += copylen;
               /* now copy rest of low memory pages */
                   copylen = readlen - copylen;
                   slowcopy(buff, (STRPG<<8), copylen);
               } else
               /* no need to wrap around */
                   slowcopy(buff, (int)(readpg << 8)+4, readlen);
         }
    } else {
        nif[ifnum].ni_idiscard++;
        if (pool) freebuf(pep);
        readlen = 0;
        }
    bnry = nextpg - 1;         /* move BNRY forward */
    if (bnry < STRPG)
        bnry = STPPG - 1;
    outportb(BNRY,bnry+STTPG);
    
    if (nif[ifnum].ni_state != NIS_UP) {      /* it was all for nothing */
		nif[ifnum].ni_idiscard++;
        return ((pool) ? freebuf(pep): OK);
	}
    if (readlen != 0) {
        pep->ep_type = net2hs(pep->ep_type);
/*        oldprio = getprio(currpid);
        if (oldprio < NETIPRI)      
            oldprio = chprio(currpid, NETIPRI);
*/        ni_in(&nif[ifnum], pep, readlen);
/*        chprio(currpid, oldprio);   
*/
    }
	return OK;
}
 
LOCAL slowcopy(buf, src, len)   /* copy from Packet RAM the slow way */
unsigned char    *buf;                   /* put the bytes here */
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
        buf[i]  = inportb(RFLSB);
    }
    outportb(CTRL, XSEL);                   /* stop the GA DMA */
    return OK;
}
