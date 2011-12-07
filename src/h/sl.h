/* sl.h */

/*  The various SL command/status registers */
#define	SL_DLL(num)		((num)+0)	/* Divisor Latch Low	*/
#define SL_DATAREG(num) ((num)+0)   /* SL Data Register */
#define	SL_DLH(num)		((num)+1)	/* Divisor Latch High	*/
#define	SL_LCR(num)		((num)+3)	/* Line Control Register*/
#define	SL_MCR(num)		((num)+4)	/* Modem Control Register*/
#define	SL_LSR(num)		((num)+5)	/* Line Status Register	*/
#define	SL_MSR(num)		((num)+6)	/* Modem Status Register*/
#define SL_IER(num)     ((num)+1)   /* Interrupt Enable Reg */
#define SL_IIR(num)     ((num)+2)   /* Interrupt Ident Reg  */

#define	SL_DTR		0x0b		/* Bits to set DTR Line		*/
#define	SL_DLA		0x80		/* Divisor latch access		*/
#define	SL_RXINT	0x01		/* Enable Data Available Intr	*/
#define	SL_TXINT	0x02		/* Enable TX Holding Register	*/
                                /* Empty Interrupt      */
#define SL_TCHECK	0x20		/* Mask for checking TX Reg	*/
#define	SL_RCHECK	0x01		/* Mask for checking RX Reg	*/
#define	SL_IMR		0x21		/* Interrupt Mask Register	*/
#define	SL_INT_PEND	0x01		/* Interrupt pending		*/
#define	SL_LSTAT	0x06		/* Line Status Interrupt	*/
#define	SL_RD		0x04		/* Receive Data Interrupt	*/
#define SL_WR		0x02		/* Ready to X-mit data		*/
#define	SL_MSTAT	0x00		/* Modem Status Interrupt	*/
#define SL_OCW2     0x20        /* 8259 Operatnl Cntl Word  */
#define SL_EOI      0x20        /* Non-specific end of int  */

#define	SL_RAWMEM	(2*1024)	/* Size of raw read memory	*/

#define SL_RETRIES  4           /* Xmit retries         */
#define	SL_SNDTMOUT	(4*18)		/* 1/18th secs to wait for xmit	*/
#define	SL_XMITDONE	0x64		/* Xmit done message		*/

#define	BITS8NOP	0x03		/* 8 bits, no parity		*/
#define	BIT8SEVENP	0x1b		/* 8 bits, even parity		*/

#define	SL_DEMSTK	0x0400		/* Size of SL daemon stack	*/
#define	SL_DEMPRIO	0x7fe0		/* Priority of SL daemon	*/
#define	SL_DEMNAME	"slwstrt"	/* Name of SL daemon		*/
#define	SL_DAEMON	slwstrt

extern	PROCESS	slwstrt();		/* Daemon process		*/


/* Control call constants		*/

#define	SL_PRNTRAW	1


/* Serial Line device control block */

struct	slblk	{			/* Datagram device control block*/
    int       sl_dnum;      /* device number of this device */
	unsigned  sl_baud;		/* baud rate			*/
    int       sl_mode;      /* word size/parity setting */
    int       sl_vect;      /* vector number        */
    int       sl_port;      /* MS-DOS port number       */
    int       sl_oldmcr;    /* Original value of MCR    */

/* Serial line Read variables						*/
	int	  sl_rsem;		/* read access semaphore	*/
    char  *sl_rbuf;     /* read buffer address      */
    char  *sl_rptr;     /* pointer into sl_rbuf     */
	int	  sl_rpid;		/* pid of reading process	*/
	int	  sl_rlen;		/* length of data read		*/
	int	  sl_rct;		/* current # of chars read 	*/
	int	  sl_resc;		/* Have we read an escape?	*/
	int	  sl_rstat;		/* status of read upon return	*/
    char  sl_raw[SL_RAWMEM];    /* raw read data        */
    char  *sl_rawptr;   /* pointer into sl_raw      */
	int	  sl_rawct;		/* count of chars in sl_raw	*/

/* Serial line Write variables						*/
	int	  sl_wsem;		/* write access semaphore    	*/
    char  *sl_wbuf;     /* write buffer address     */
    char  *sl_wptr;     /* pointer into sl_wbuf     */
    int   sl_wstsem;    /* start up low level write */
	int	  sl_wpid;		/* pid of slwstrt process	*/
	int	  sl_wlen;		/* len of packet being written	*/
	int	  sl_wct;		/* current # of chars written	*/
	int	  sl_wesc;		/* pending escape xmit?		*/
    int   sl_wechar;    /* escaped character        */
	int	  sl_wstat;		/* status of write upon return	*/

 /* Serial Line Device Statistics */
 	long	  sl_int;		/* count of SL dev interrupts	*/
 	long	  sl_tint;		/* transmit interrupts		*/
 	long	  sl_rint;		/* receive interrupts		*/
 	long	  sl_lstat;		/* line status interrupts	*/
 	long	  sl_mstat;		/* modem status interrupts	*/
 	long	  sl_iir;		/* Bad interrupts		*/
    long      sl_badesc;    /* # of badly escaped chars */
 	long	  sl_snd;		/* packets transmitted		*/
    long      sl_wescct;    /* # of llp ESC's written   */
 	long	  sl_notx;		/* # of tx ghost interrupts	*/
 	long	  sl_nord;		/* # of read ghost interrupts	*/
    long      sl_badfend;   /* # of ghost frame ends    */
    long      sl_rescct;    /* # of llp ESCs received   */
 	long	  sl_drop;		/* # of packets dropped 	*/
 	long	  sl_rcv;		/* # of packets received 	*/
    long      sl_reqtmo;    /* # of Read packet timeouts    */
    long      sl_sndtmo;    /* # of Send packet timeouts    */
};


/* Character constants to trap during SLIP transmissions	 	*/

#define SL_END          0300        /* End of packet        */
#define SL_ESC          0333        /* Frame Escape character    */
#define	SL_TRANS_END	0334		/* Translated Frame End char	*/
#define	SL_TRANS_ESC	0335		/* Translated ESC character	*/


/* Baud rate masks.  For more information on these masks, please refer to
 * the IBM-BIOS section on the IBM Asynchronous Communications Adaptr .
 */

#define BAUD_19200	0x0006	/* divisors for the different baud rates */
#define BAUD_9600	0x000c
#define	BAUD_4800	0x0018
#define	BAUD_2400	0x0030
#define	BAUD_1200	0x0060
#define	BAUD_600	0x00c0
#define	BAUD_300	0x0180
#define	BAUD_110	0x0417

#define	INIT_BAUD	BAUD_9600

#ifdef Nsl
extern  struct  slblk   sltab[Nsl];
#endif

