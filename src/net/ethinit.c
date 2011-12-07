/* ethinit.c -- Ethernet initialization */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <pc.h>
#include <ec.h>
#include <q.h>

extern SYSCALL pcreate(int count);

struct etblk eth[Neth];		/* device control blocks */

/*------------------------------------------------------------------------
 *  ethinit  --  Ethernet device initialization (3C503 Version)
 *------------------------------------------------------------------------
 */	
ethinit(devptr, flag)
struct devsw	*devptr;
int flag;
{
    
	register int	i;
	struct etblk	*etptr;
    int     icvect;                         /* interrupt controller slot */
    int     freebuf();
    unsigned char    *cp;
/* HACK - Remove this to restore network, but beware that this will crash XINU on Linux */
    return(OK);

        icvect = devptr->dvivec - 8;            /* adjust int number */
        if ( icvect < 0 || icvect > 7 )
            return(SYSERR);

        etptr = &eth[devptr->dvminor];          /* get the device cntl block */
    if (flag) {
        ethstate(devptr,1);                 /* save controller state */
        memset(etptr,0,sizeof(struct etblk));   /* clear the lot */
        devptr->dvioblk = (char *) etptr;   /* dvioblk is *etptr */
        etptr->etdnum = devptr->dvnum;      /* cross-ref to device number */
        etptr->etport = devptr->dvport;     /* device i/o port address */
        etptr->etvect = devptr->dvivec;     /* interrupt vector */

        blkcopy(etptr->etbcast, EP_BRC, EP_ALEN);
        
        /* set up global information */

        etptr->etdescr = "3C503";       /* identifies the board */
        etptr->etdev = devptr;          /* handy for later */
        etptr->etintf = -1;             /* invalid for now */
/* HACK! */
/*        etptr->etoutp = pcreate(NETBUFS);*/ /* was ETOUTQSZ */
        etptr->etwsem = screate(0);
        
        /* 3C503 reset and EA prom read (also resets 8390) */
    
        outportb(CTRL, RST | XSEL);         /* reset ON */
        outportb(CTRL, XSEL);               /* reset OFF */
        outportb(CTRL, EALO | XSEL);        /* EA prom ON */
        cp = (unsigned char*)etptr->etpaddr;
        for (i=0 ; i < EP_ALEN ; i++)       /* copy the ethernet address */
            *cp++ = inportb(ADDROM+i);
        outportb(CTRL, XSEL);               /* EA prom OFF */
        
        /* Configure 3C503 */

        outportb(GACFR, NORM);      /* shared memory, 8K RAM */
        outportb(PSTR, STRTPG);     /* set start page */
        outportb(PSPR, STOPPG);     /* set stop page */
        outportb(IDCFR, 0x10 << icvect-2);      /* select int */
        outportb(DQTR, 0x08);       /* 8-byte transfers */
        outportb(DAMSB, 0x20);
        outportb(DALSB, 0);                                           

        /* initialize page 0 controller registers */

        outportb(CMDR, MSK_PG0 | MSK_STP);  /* select Page 0 */
        outportb(DCR, MSK_BMS | MSK_FT10);  /* loopback FIFO threshold=8*/
        outportb(RBCR0, 0);                 /* not essential *
        outportb(RBCR1, 0);
        outportb(RCR, MSK_MON);             /* monitor mode */
        outportb(TCR, 0x02);                /* loopback mode */
        outportb(BNRY, STOPPG-1);           /* ring buffer write bndry */
        outportb(PSTART, STRTPG);           /* bottom of ring buffer */
        outportb(PSTOP, STOPPG);            /* top of ring buffer     */

        /* initialize page 1 controller registers */

        outportb(CMDR, MSK_PG1 | MSK_STP);  /* select Page 1 */
        outportb(CURR,STRTPG);              /* first page of shared mem */
        cp = (unsigned char*)etptr->etpaddr;
        for (i=0 ; i<EP_ALEN ; i++)         /* initialize physical address  */
            outportb(PAR0+i, *cp++);
        for (i=0 ; i<8 ; i++ )              /* clear multicast address mask */
            outportb(MAR0,'\0');

        outportb(CMDR, MSK_PG0 | MSK_STP);  /* select Page 0 */
        outportb(ISR, 0xff);                /* clear ISR            */
        outportb(IMR, IMASK);               /* respond to all interrupts*/
        outportb(CMDR, MSK_STA);            /* set start mode */
        outportb(RCR, MSK_AB);              /* accept broadcasts */

        /* Clear mask (enable interrupt) in 8259 controller */

        outportb(IC_MASK,(char)(inportb(IC_MASK) & ~(1<<icvect)));

        return(OK);         /* 8390 is still in internal loopback mode  */
                            /* we take it out of loopback in ethout.c */
                            /* Set RCR to 0 if no broadcasts wanted */
    } else {                        /* just stop the NIC */
/*            outportb(CMDR, MSK_PG0 | MSK_STP); */ /* select Page 0 and Stop */
/*            while ((inportb(ISR) & MSK_RST) == 0 ); */
            sdelete(etptr->etwsem);
/* HACK
            pdelete(etptr->etoutp, freebuf); */
            ethstate(devptr, 0);            /* restore controller state */
            return OK;
        }  
}
