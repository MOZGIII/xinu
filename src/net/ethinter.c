/* ethinter.c - ethinter */

#include <kernel.h>
#include <conf.h>
#include <proc.h>
#include <network.h>
#include <pc.h>
#include <ec.h>

/*------------------------------------------------------------------------
 *  ethinter  --  Ethernet device ISR (3C503 Version)
 *------------------------------------------------------------------------
 */
INTPROC ethinter(mdev)
int mdev;
{
    unsigned char isr;
    unsigned char stat;
    unsigned char txp;
    unsigned char bnry;
    unsigned char curr;
    int oldprio, i;
    int resend;
    struct etblk *etptr;
    struct netif *pni;

#define STRPG STRTPG-STTPG
#define STPPG STOPPG-STTPG

    outportb(IC_PORT,IC_EOI);   /* 8259 non-specific end of interrupt */
    
    resend = 0;
    etptr = &eth[mdev];
    pni = &nif[etptr->etintf];  /* set up pointer to nif */

    while ((isr=inportb(ISR)) & IMASK) {    /* we don't want all ints */ 
        etptr->ints++;                      /* count interrupts */
        if (isr & MSK_OVW) {                /* rcv buffer overrun */
            stat = inportb(CMDR) & 0x04;    /* was an xmit pending */
            outportb(CMDR,MSK_PG0 | MSK_STP);      /* stop the 8390 */
            /* wait for stop to complete */
            /* poll the ISR for the RST bit */
            while ((inportb(ISR) & MSK_RST) == 0 ); 
            outportb(TCR, 0x02);                /* loopback mode on */
            /* clear remote byte count registers */
            outportb(RBCR0, 0);                 /* not essential *
            outportb(RBCR1, 0);
            if (stat == 0) resend = FALSE;
            else {
                isr = inportb(ISR) & IMASK & (MSK_PTX | MSK_TXE);
                if (isr != 0) resend = FALSE;
                else resend = TRUE;
            }
            /* empty the buffer ring */
            
            while (ethdemux(etptr) == OK) ;     /* read packets */
            outportb(CMDR,MSK_PG0 | MSK_STA);   /* start again */
            outportb(ISR, MSK_OVW);             /* clear ISR OVW          */
            outportb(TCR,0);                    /* loopback mode off */
            /* restart the xmitter if necessary */
            if (resend) outportb(CMDR, MSK_TXP); 
            etptr->over++;
			etptr->drop++;
            pni->ni_idiscard++;
            pni->ni_ierrors++;
            continue; 
            /* above code violates point 3 page 1-139 of NS8390 data book */ 
		}
        if (isr & MSK_RXE) {                    /* receive error */
            outportb(ISR,MSK_RXE);              /* clear RXE */
			etptr->fcs++;
			etptr->drop++;
            pni->ni_ierrors++;
            pni->ni_idiscard++;
			continue;
		}
        if (isr & MSK_PRX) {                    /* good packet */
            outportb(ISR, MSK_PRX);             /* clear PRX */
/*            oldprio = getprio(currpid);
            if (oldprio < NETIPRI)
                oldprio = chprio(currpid, NETIPRI); */
            while (ethdemux(etptr)==OK) ;       /* get them all */
/*            chprio(currpid, oldprio);                 */
            continue;
		}
        if (isr & (MSK_TXE | MSK_PTX)) {        /* transmit has completed */
            stat = inportb(TSR);        /* get tx status bits */
            /* clear TXE and PTX */
            outportb(ISR, MSK_TXE | MSK_PTX);
			if (stat & SMK_COL || isr & MSK_TXE)
				etptr->jams++;
			if (stat & SMK_ABT)
				etptr->timeout++;
            etptr->out++;
            xinu_signal(etptr->etwsem);
            continue;
        }
        etptr->spur++;             /* all other ints are of no interest */
        outportb(ISR, (MSK_CNT | MSK_RDC)); /* clear them anyway */
    }
}
