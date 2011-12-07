/* ether.h */

/* Ethernet definitions and constants */

#define EP_MINLEN     60        /* minimum packet length        */
#define EP_DLEN     1500        /* length of data field ep      */
#define EP_HLEN       20        /* size of Ether header + length + next hop */
#define EP_MAXLEN EP_HLEN+EP_DLEN
#define EP_ALEN        6        /* number of octets in physical address */
typedef char Eaddr[EP_ALEN];    /* length of physical address (48 bits) */
#define EP_RETRY        3       /* number of times to retry xmit errors */
#define	EP_BRC	"\377\377\377\377\377\377"/* Ethernet broadcast address	*/
#define EP_RTO        300       /* time out in seconds for reads    */

#define	EPT_LOOP	0x0060		/* type: Loopback		*/
#define	EPT_ECHO	0x0200		/* type: Echo			*/
#define	EPT_PUP		0x0400		/* type: Xerox PUP		*/
#define	EPT_IP		0x0800		/* type: Internet Protocol	*/
#define	EPT_ARP		0x0806		/* type: ARP			*/
#define	EPT_RARP	0x8035		/* type: Reverse ARP		*/

struct  eh {                    /* ethernet header          */
    Eaddr   eh_dst;             /* destination host address     */
    Eaddr   eh_src;             /* source host address          */
    unsigned short  eh_type;    /* Ethernet packet type (see below) */
};

struct  ep  {                   /* complete structure of Ethernet packet*/
    IPaddr  ep_nexthop;         /* niput() uses this            */
    short   ep_len;             /* length of the packet         */
    struct  eh ep_eh;           /* the ethernet header          */
	char	ep_data[EP_DLEN];	/* data in the packet		*/
};

/* these allow us to pretend it's all one big happy structure */

#define	ep_dst	ep_eh.eh_dst
#define	ep_src	ep_eh.eh_src
#define	ep_type	ep_eh.eh_type

#if	Noth > 0
/* this is for Othernet simulation */

struct	otblk {
	Bool	ot_valid;	/* these entries are valid?		*/
	Eaddr	ot_paddr;	/* the Othernet physical address	*/
	Eaddr	ot_baddr;	/* the Othernet broadcast address	*/
    int ot_rpid;        /* id of process reading from othernet  */
    int ot_rsem;        /* mutex for reading from the othernet  */
    int ot_pdev;        /* Physical device devtab index     */
    int ot_intf;        /* the associate interface      */
    char *ot_descr;     /* text description of the device   */
};
#endif 

/* Ethernet control block descriptions */

struct	etblk	{
	int	etdnum;		/* device number of this device 	*/
    struct devsw *etdev; /* address of device switch table entry */
    Eaddr  etpaddr; /* Ethernet physical device address */
    Eaddr  etbcast; /* Ethernet broadcast address */
	int	etrpid;		/* id of process reading from ethernet	*/
    char *etrbuf;   /* read buffer address          */
	int	etrsize;	/* length of data read			*/
	int	etrsem;		/* mutex for reading from the ethernet	*/
	int	etwsem;		/* mutex for writing to the ethernet	*/
	int	etwlen;		/* lenght of packet being written or 0	*/
	int	etport;		/* port number				*/
	int	etvect;		/* vector number			*/
	int	etbase;		/* paragraph address for shared memory	*/
    int etoutp;     /* port for writing to the ethernet    */
	int	etxpending;	/* number of packets pending output	*/
	int	etrpending;	/* 1 => a receive is pending already	*/
	int	etwtry;		/* num. of times to retry xmit errors	*/
    char *etwbuf;   /* pointer to current transmit buffer   */
	int	etintf;		/* our interface number			*/
    short etnextbuf;  /* for checking buffers round robin */
    char *etdescr;  /* text description of the device   */
    long ints;      /* Interrupts */
	long spur;		/* Spurious interrupts */
    long shorts;    /* Short packets */
    long dribble;   /* Packets with extra bits */
	long fcs;		/* Packets with CRC errors */
	long over;		/* Overflow errors */
	long any;		/* Good packets received */
	long stale;		/* Stale status on interrupt */
    long badsize;   /* Packets longer than legal length */
	long out;		/* Total output packets */
    long timeout;   /* Transmitter timeouts */
    long badtype;   /* Packets with unknown type fields */
	long drop;		/* Dropped because RX queue too long */
	long jams;		/* Collisions */
#if Noth > 0
	struct	otblk	*etoth[Noth];	/* eaddr to oaddr translations	*/
#endif
};
#define ETOUTQSZ 10         /* put this in ether.h   */

extern  struct  etblk   eth[];
