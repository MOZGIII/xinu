/* ethout.c - ethout */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ec.h>
#include <bufpool.h>
#include <ports.h>
LOCAL slowcopy(char *ptr, int len);
/*------------------------------------------------------------------------
 *  ethout - write packets to the Ethernet (3C503 Version)
 *------------------------------------------------------------------------
 */

PROCESS ethout(ifnum)
int  ifnum;
{
    struct  etblk   *etptr;         /* Ethernet control block ptr */
    int     ps;
    int     size;
    struct  netif   *pni;
    struct  devsw   *devptr;
    struct  ep      *pep;
    int i;
    char    *ptr;
        
    pni = &nif[ifnum];
    devptr = &devtab[pni->ni_dev];
    etptr = (struct etblk *) devptr->dvioblk;  /* ptr to ecntl block */
    xinu_signal(Net.sema);
    
    while (TRUE) {
        pep = (struct ep *)preceive(etptr->etoutp); /* get a packet */
        if (pep == NULL) {                  
            sleep(1);
            continue;
        }
        blkcopy(pep->ep_src, etptr->etpaddr, EP_ALEN); /* copy src address */
        pep->ep_type = hs2net(pep->ep_type); /* network byte order needed */
        pni->ni_ooctets += pep->ep_len;
        if (blkequ(pni->ni_hwb.ha_addr, pep->ep_dst, EP_ALEN))
            pni->ni_oucast++;
        else 
            pni->ni_onucast++;

/*ptr = pep->ep_dst;
xinu_printf("len = %d\n",pep->ep_len);
for (i=0; i<pep->ep_len; i++)
    xinu_printf("%02x ",(unsigned char) ptr[i]);
printf("\n");
*/
        size = pep->ep_len;
        size = (size+1) & ~1;       /* round size up to next even number */
        disable(ps); 
        slowcopy((char*)&pep->ep_eh, size);    /* copy packet using GA DMA */
        freebuf(pep);                   /* return buffer to the pool */

        /* assume the controller is set to I/O page zero */
        
        outportb(TCR, 0);                   /* out of loopback */
        outportb(TPSR, STTPG);              /* transmit page starts at 0x2000 */
        outportb(TBCR0, size & 0xff);       /* LSB of xmit count */
        outportb(TBCR1, (size>>8) & 0xff);  /* MSB of xmit count */
        outportb(CMDR, MSK_TXP);            /* start the transmission */
        wait(etptr->etwsem);                /* wait for completion */ 
        restore(ps); 
    }
}
/* We assume that the packet will be sent and that a TXE interrupt will NOT
   occur. TXE can occur if the FIFO underruns (unlikely) or if the max 
   collision count is exceeded (could happen on loaded networks). If a packet
   can't be sent, there's nothing we can do about it at this point (the 
   buffer is gone) and higher level protocols will  have to recover. We do 
   always need our signal to continue, even if the packet isn't sent. 
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
        outportb(RFLSB, ptr[i]);
    }
    outportb(CTRL, XSEL);                   /* stop the GA DMA */
    return OK;
}
