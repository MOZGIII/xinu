/* ethinter.c - ethinter */

#include <kernel.h>
#include <conf.h>
#include <proc.h>
#include <network.h>
#include <pc.h>
#include <ec.h>

/*------------------------------------------------------------------------
 *  ethinter  --  Ethernet device ISR for WD8003E controller
 *------------------------------------------------------------------------
 */
INTPROC ethinter(mdev)
int mdev;
{
	register unsigned ieport;
    unsigned char isr;
    unsigned char stat;
    int pid;
    int sem;
    struct etblk *etptr;

    outportb(IC_PORT,IC_EOI);   /* 8259 non-specific end of interrupt */
	etptr = &eth[mdev];
	ieport = etptr->etport;
    etptr->ints++;              /* count interrupts */
    pid = BADPID;               /* assume nobody is waiting to read */
    sem = SYSERR;               /* assume nobody is waiting to write */

    while ((isr=inportb(ISR(ieport))) & IMASK) { /* we don't want all ints */ 
        if (isr & MSK_OVW) {                /* rcv buffer ring overwritten */
            outportb(CMDR(ieport),MSK_STP);             /* stop the 8390 */
			/* clear remote byte count registers */
			outportb(RBCR0(ieport),0);
			outportb(RBCR1(ieport),0);
            /* poll the ISR for the RST bit */
			while ( (inportb(ISR(ieport)) & MSK_RST) == 0 )
				;
			outportb(TCR(ieport),0x02);	/* loopback mode */
			outportb(CMDR(ieport),MSK_STA);	/* start mode */
			outportb(TCR(ieport),0);	/* out of loopback */
			outportb(ISR(ieport),MSK_OVW);	/* clear OVW */
			etptr->over++;
			etptr->drop++;
            continue; 
        /* the above code violates page 1-139 of the NS8390 data book */ 
		}
        if (isr & MSK_RXE) {                            /* receive error */
            outportb(ISR(ieport),MSK_RXE);              /* clear RXE */
			etptr->fcs++;
			etptr->drop++;
			continue;
		}
        if (isr & MSK_PRX) {                            /* good packet */
            outportb(ISR(ieport), MSK_PRX);             /* clear PRX */
            if ((pid=etptr->etrpid) != BADPID) /* set pid and clear etrpid */
				etptr->etrpid = BADPID;
            continue;
        /* alter the above code to get the packet and call ni_in */    
		}
        if (isr & (MSK_TXE | MSK_PTX)) {        /* transmit has completed */
            stat = inportb(TSR(ieport));        /* get tx status bits */
			/* clear TXE and PTX */
			outportb(ISR(ieport), MSK_TXE | MSK_PTX);
			if (stat & SMK_COL || isr & MSK_TXE)
				etptr->jams++;
			if (stat & SMK_ABT)
				etptr->timeout++;
			sem = etptr->etwsem;
            etptr->out++;
            continue;
		}
        etptr->spur++;             /* all other ints are of no interest */
	}
    if (pid != BADPID)
        resume(pid);               /* resume the reading process */
	if (sem != SYSERR)
        xinu_signal(sem);               /* signal the relevant semaphore */
}
