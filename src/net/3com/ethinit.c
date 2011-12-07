/* ethinit.c -- Ethernet initialization */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <pc.h>
#include <ec.h>
#include <q.h>

struct etblk eth[Neth];		/* device control blocks */

/*------------------------------------------------------------------------
 *  ethinit  --  Ethernet device initializaion (3C503 Version)
 *------------------------------------------------------------------------
 */	
ethinit(devptr)
struct devsw	*devptr;
{
	register int	i;
	struct etblk	*etptr;
    int     icvect;                         /* interrupt controller slot */
	char	*cp;
	long	strtol();

    icvect = devptr->dvivec - 8;            /* adjust int number */
	if ( icvect < 0 || icvect > 7 )
		return(SYSERR);

    etptr = &eth[devptr->dvminor];          /* get the device cntl block */
    memset(etptr,0,sizeof(struct etblk));   /* clear the lot */

    etptr->etbase = SMBASE;             /* shared mem is at 0xd800 */
    devptr->dvioblk = (char *) etptr;   /* dvioblk is *etptr */
    etptr->etdnum = devptr->dvnum;      /* cross-ref to device number */
    etptr->etport = devptr->dvport;     /* device i/o port address */
    etptr->etvect = devptr->dvivec;     /* interrupt vector */

    blkcopy(etptr->etbcast, EP_BRC, EP_ALEN);
        
    etptr->etrsem = screate(1);         /* synchronizes read access */
    etptr->etwsem = screate(1);         /* synchronizes write access */
    etptr->etrpid = BADPID;             /* pid of reading process */

    /* set up global information */

    etptr->etdescr = "3C503";       /* identifies the board */
    etptr->etdev = devptr;          /* handy for later */
    etptr->etintf = -1;             /* invalid for now */

    /* 3C503 reset and EA prom read */
    
    outportb(CTRL, RST | XSEL);         /* reset ON */
    outportb(CTRL, XSEL);               /* reset OFF */
    outportb(CTRL, EALO | XSEL);        /* EA prom ON */
	cp = etptr->etpaddr;
    for (i=0 ; i < EP_ALEN ; i++)       /* copy the ethernet address */
        *cp++ = inportb(ADDROM+i);
    outportb(CTRL, XSEL);               /* EA prom OFF */
        
    /* Configure 3C503 */

    outportb(GACFR, NORM);      /* shared memory, 8K RAM */
    outportb(PSTR, STRTPG);     /* set start page */
    outportb(PSPR, STOPPG);     /* set stop page */
    outportb(IDCFR, 0x80);      /* INT 5 active */
    outportb(DQTR, 0x08);       /* 8-byte transfers */
    outportb(DAMSB, 0x20);
    outportb(DALSB, 0);                                           

    /* initialize page 0 controller registers */

    outportb(CMDR, MSK_PG0);            /* select Page 0 */
    outportb(DCR, MSK_BMS | MSK_FT10);  /* loopback FIFO threshold=8*/
    outportb(RBCR0, 0);                 /* clear remote byte count  */
    outportb(RBCR1, 0);
    outportb(RCR, MSK_MON);             /* disable receiver for now */
    outportb(TCR, 0x04);                /* loopback mode */
    outportb(BNRY, STOPPG-1);           /* ring buffer write bndry */
    outportb(PSTART, STRTPG);           /* bottom of ring buffer */
    outportb(PSTOP, STOPPG);            /* top of ring buffer     */
    outportb(ISR, 0xff);                /* clear ISR            */
    outportb(IMR, IMASK);               /* respond to all interrupts*/

	/* initialze page 1 controller registers */

    outportb(CMDR, MSK_PG1);        /* select Page 1 */
	cp = etptr->etpaddr;
    for (i=0 ; i<EP_ALEN ; i++)     /* initialize physical address  */
        outportb(PAR0+i, *cp++);
    for (i=0 ; i<8 ; i++ )          /* clear multicast address mask */
        outportb(MAR0,'\0');

    outportb(CURR,STRTPG);         /* first page of shared mem */

	/* now start the controller */

    outportb(CMDR, MSK_STA);        /* start the controller */
    outportb(CMDR, MSK_PG0);        /* select Page 0 */
    outportb(RCR, MSK_AB);          /* accept broadcasts */

    /* Clear mask (enable interrupt) in 8259 controller */

	outportb(IC_MASK,(char)(inportb(IC_MASK) & ~(1<<icvect)));

    return(OK);
}
