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
 *  ethwrite - write a single packet to the ethernet
 *------------------------------------------------------------------------
 */

ethwrite(devptr, pep, len)
struct	devsw	*devptr;
struct  ep      *pep;
int             len;
{
    struct  etblk   *etptr;         /* Ethernet control block ptr */
    char    *smp;                  /* shared memory pointer */
    int     ps;
    int     ieport;
    int     size;
    
    if (len > EP_MAXLEN || len <= 0) {      /* check packet length */
        freebuf(pep);
		return(SYSERR);
	}
    if (len < EP_MINLEN)           /* enforce minimum length */
        len = EP_MINLEN;
        
    etptr = (struct etblk *) devptr->dvioblk;  /* ptr to  Eth Cntl Block */
    wait(etptr->etwsem);            /* ethwrite requires mutual exclusion */
    blkcopy(pep->ep_src, etptr->etpaddr, EP_ALEN); /* copy src address */
    pep->ep_type = hs2net(pep->ep_type); /* network byte order needed */

    ieport = etptr->etport;
    size = len;
    size = (size+1) & ~1;          /* round size up to next even number */
    smp = MK_FP(etptr->etbase, 0); /* xmit buffer is at base of shared mem */
    disable(ps);                   /* we don't want interrupts for now */
    blkcopy(smp,&pep->ep_eh,size); /* copy packet into shared mem */
    freebuf(pep);                  /* return buffer to the pool */

/* assume the controller is set to I/O page zero */

    outportb(TPSR(ieport), 0);      /* transmit page starts at 0 */
    outportb(TBCR0(ieport), size & 0xff);      /* LSB of xmit count */
    outportb(TBCR1(ieport), (size>>8) & 0xff); /* MSB of xmit count */
    outportb(CMDR(ieport),MSK_TXP); /* initiate the transfer */
    restore(ps);                    /* restore interrupts */
    return(len);                    /* the int routine will signal etwsem */
}
/* Note: We assume that the packet will be sent and that a TXE interrupt will
         NOT occur. TXE can occur if the FIFO underruns (unlikely) or if the
         max collision count is exceeded (could happen on loaded networks).
         If a packet can't be sent, there's nothing we can do about it at
         this point (the buffer is gone) and higher level protocols will 
         have to recover.
*/          
