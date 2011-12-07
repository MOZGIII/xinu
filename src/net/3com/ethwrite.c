/* ethwrite.c - ethwrite */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <pc.h>
#include <ec.h>
#include <dos.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  ethwrite - write a single packet to the ethernet (3C503 Version)
 *------------------------------------------------------------------------
 */

ethwrite(devptr, pep, len)
struct	devsw	*devptr;
struct  ep      *pep;
int             len;
{
    struct  etblk   *etptr;         /* Ethernet control block ptr */
    char    *smp;                   /* shared memory pointer */
    int     ps;
    int     size;
    struct netif *pni;
        
    if (len > EP_MAXLEN || len <= 0) {      /* check packet length */
        freebuf(pep);
		return(SYSERR);
	}
    if (len < EP_MINLEN)            /* enforce minimum length */
        len = EP_MINLEN;
        
    etptr = (struct etblk *) devptr->dvioblk;  /* ptr to ecntl block */
    pni = &nif[etptr->etintf];
    
    wait(etptr->etwsem);            /* ethwrite requires mutual exclusion */
    blkcopy(pep->ep_src, etptr->etpaddr, EP_ALEN); /* copy src address */
    pep->ep_type = hs2net(pep->ep_type); /* network byte order needed */
    pni->ni_ooctets += len;
    if (blkequ(pni->ni_hwb.ha_addr, pep->ep_dst, EP_ALEN))
        pni->ni_onucast++;
    else 
        pni->ni_oucast++;

    size = len;
    size = (size+1) & ~1;           /* round size up to next even number */
    smp = MK_FP(etptr->etbase, 0);  /* xmit buffer is at base of shared mem */
    disable(ps);                    /* we don't want interrupts for now */
/*    blkcopy(smp,&pep->ep_eh,size);*/  /* copy packet into shared mem */
    slowcopy(&pep->ep_eh, size);    /* copy packet using GA DMA */
    freebuf(pep);                   /* return buffer to the pool */

/* assume the controller is set to I/O page zero */

    outportb(TCR, 0x00);                /* normal operation */    
    outportb(TPSR, STTPG);              /* transmit page starts at 0x2000 */
    outportb(TBCR0, size & 0xff);       /* LSB of xmit count */
    outportb(TBCR1, (size>>8) & 0xff);  /* MSB of xmit count */
    outportb(CMDR, MSK_STA);            /* start the NS8390 */
    outportb(CMDR, 0x26);               /* initiate the transfer */
    restore(ps);                        /* restore interrupts */
    return(len);                        /* int routine signals etwsem */
}
/* Note: We assume that the packet will be sent and that a 
         TXE interrupt will NOT occur. TXE can occur if the
         FIFO underruns (unlikely) or if the max collision 
         count is exceeded (could happen on loaded networks). 
         If a packet can't be sent, there's nothing we can do
         about it at this point (the buffer is gone) and higher
         level protocols will  have to recover. 
*/

LOCAL slowcopy(ptr, len)    /* write bytes to the NS8390 the slow way */
char    *ptr;
int     len;
{
    int     i;
    
    outportb(DALSB, 0x00);                  /* DMA LSB */
    outportb(DAMSB, STTPG);                 /* DMA MSB = txbuf start */
    outportb(CTRL, START | DDIR | XSEL);    /* start the GA DMA */
    
    for(i = 0; i < len; i++) {
        if (i % 8 == 0)
            while ((inportb(STREG) & DPRDY) == 0) ; /* wait for FIFO ready */
        outportb(RFMSB, ptr[i]);
    }
    outportb(CTRL, XSEL);                   /* stop the GA DMA */
    return OK;
}
