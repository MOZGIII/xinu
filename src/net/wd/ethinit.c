/* ethinit.c -- initialization for WD8003E Ethernet card */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <pc.h>
#include <ec.h>
#include <q.h>

struct etblk eth[Neth];		/* device control blocks */

/*------------------------------------------------------------------------
 *  ethinit  --  Ethernet device initializaion
 *------------------------------------------------------------------------
 */	
ethinit(devptr)
struct devsw	*devptr;
{
	register int	i;
	struct etblk	*etptr;
	register int	ieport;		/* controller port		*/
    int     icvect;             /* interrupt controller slot    */
    int     ck;                 /* ROM checksum         */
	char	*cp;
	long	strtol();

    icvect = devptr->dvivec - 8;            /* adjust int number */
	if ( icvect < 0 || icvect > 7 )
		return(SYSERR);

    ieport = devptr->dvport;                /* get the I/O base addr */
    etptr = &eth[devptr->dvminor];          /* get the device cntl block */
    memset(etptr,0,sizeof(struct etblk));   /* clear the lot */

	if (devptr->dvioblk == NULL)
        etptr->etbase = SM_BASE;           /* typically 0xd000 */
    else    /* Get base memory paragraph address from dvioblk string */
		etptr->etbase = (unsigned) strtol(devptr->dvioblk,NULL,0);
        
	devptr->dvioblk = (char *) etptr;
    etptr->etdnum = devptr->dvnum;      /* cross-ref to device number */
    etptr->etport = devptr->dvport;     /* device i/o port address */
    etptr->etvect = devptr->dvivec;     /* interrupt vector */

    blkcopy(etptr->etbcast, EP_BRC, EP_ALEN);

    ck = 0;                             /* check the ROM */
	cp = etptr->etpaddr;
    for (i=0 ; i<EP_ALEN ; i++) {
		*cp = inportb(ADDROM(ieport)+i);
		ck += *cp++;
	}
    ck += inportb(ADDROM(ieport)+i);    /* WD8003E identity byte * /
    ck += inportb(ADDROM(ieport)+i+1);  /* ROM checksum */
    if (ck & 0xff != 0xff )
        return(SYSERR);
        
    etptr->etrsem = screate(1);         /* synchronizes read access */
    etptr->etwsem = screate(1);         /* synchronizes write access */
    etptr->etrpid = BADPID;             /* pid of reading process */

	/* Clear mask (enable interrupt) in 8259 controller */

	outportb(IC_MASK,(char)(inportb(IC_MASK) & ~(1<<icvect)));

    /* reset board and initialize WD8003E memory select register */

    outportb(MSR(ieport), MSK_RST);     /* WD8003E cmd register: reset */
    outportb(MSR(ieport), 0);           /* toggle the reset bit */
	outportb(MSR(ieport), MSK_MENB | (etptr->etbase >> 9 & 0x3f));
                                        /* enable shared memory */
                                        
	/* initialize page 0 controller registers */

    outportb(CMDR(ieport), MSK_PG0);            /* select Page 0 */
    outportb(DCR(ieport), MSK_BMS | MSK_FT10);  /* loopback FIFO threshold=8*/
    outportb(RBCR0(ieport), 0);                 /* clear remote byte count  */
	outportb(RBCR1(ieport), 0);
    outportb(RCR(ieport), MSK_MON);             /* disable receiver for now */
    outportb(TCR(ieport), 0);                   /* normal operation     */
    outportb(PSTOP(ieport), STOP_PG);           /* top of ring buffer     */
    outportb(PSTART(ieport), STRT_PG);          /* bottom of ring buffer */
    outportb(BNRY(ieport), STOP_PG-1);          /* ring buffer write bndry */
    outportb(ISR(ieport), 0xff);                /* clear ISR            */
    outportb(IMR(ieport), IMASK);               /* respond to all interrupts*/

	/* initialze page 1 controller registers */

    outportb(CMDR(ieport), MSK_PG1);        /* select Page 1 */
	cp = etptr->etpaddr;
    for (i=0 ; i<EP_ALEN ; i++)             /* initialize physical address  */
		outportb(PAR0(ieport)+i, *cp++);
    for (i=0 ; i<8 ; i++ )                  /* clear multicast address mask */
		outportb(MAR0(ieport),'\0');
    outportb(CURR(ieport),STRT_PG);         /* first page of shared mem */

    /* set up global information */

    etptr->etdescr = "WD8003E";             /* identifies the board */
    etptr->etdev = devptr;                  /* handy for later */
    etptr->etintf = -1;                     /* invalid for now */

	/* now start the controller */

    outportb(CMDR(ieport), MSK_STA);       /* start the controller */
    outportb(CMDR(ieport), MSK_PG0);       /* select Page 0 */
    outportb(RCR(ieport), MSK_AB);         /* accept broadcasts */
	return(OK);
}
