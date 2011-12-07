/* pc.h - I/O definitions specific to the IBM PC */

#define DB0VEC		0x00		/* divide by zero exception vec	*/
#define SSTEPVEC	0x01		/* single step exception vector	*/
#define BKPTVEC		0x03		/* breakpoint exception vector	*/
#define OFLOWVEC	0x04		/* overflow exception vector	*/

#define BASEVEC	0x08			/* Base of hardware interrupts	*/
#define NVECS	0x08			/* Number of hardware ints	*/

/* device specific hardware interrupts */
#define	CLKVEC	0x08			/* Clock */
#define	KBDVEC	0x09			/* Keyboard */
#define	COM2VEC	0x0b			/* COM2 */
#define	COM1VEC	0x0c			/* COM1 */
#define	FLOPVEC	0x0e			/* Floppy */
#define	PRLLVEC	0x0f			/* Parallel port */

/* device specific port assignments */
#define COM1PORT	0x3f8
#define COM2PORT    0x2f8

/* pseudo-device interrupt */
#define CBRKVEC	0x1b			/* Ctrl-Break interrupt vector	*/

#define	BIOSFLG	0x100			/* BIOS flag for intmap		*/

/* 8259 interrupt controller definitions */
#define	IC_MASK	0x21		/* 8259 int. controller mask	*/
#define	IC_PORT	0x20		/* 8259 port address		*/
#define	IC_EOI	0x20		/* end-of-interrupt command	*/

/* 8250 definitions */
/* Control/status register offsets from base address */
#define	ASY_THR		0	/* Transmitter holding register */
#define	ASY_RBR		0	/* Receiver buffer register */
#define	ASY_DLL		0	/* Divisor latch LSB */
#define	ASY_DLM		1	/* Divisor latch MSB */
#define	ASY_IER		1	/* Interrupt enable register */
#define	ASY_IIR		2	/* Interrupt ident register */
#define	ASY_LCR		3	/* Line control register */
#define	ASY_MCR		4	/* Modem control register */
#define	ASY_LSR		5	/* Line status register */
#define	ASY_MSR		6	/* Modem status register */

/* 8250 Line Control Register */
#define	LCR_5BITS	0	/* 5 bit words */
#define	LCR_6BITS	1	/* 6 bit words */
#define	LCR_7BITS	2	/* 7 bit words */
#define	LCR_8BITS	3	/* 8 bit words */
#define	LCR_NSB		4	/* Number of stop bits */
#define	LCR_PEN		8	/* Parity enable */
#define	LCR_EPS		0x10	/* Even parity select */
#define	LCR_SP		0x20	/* Stick parity */
#define	LCR_SB		0x40	/* Set break */
#define	LCR_DLAB	0x80	/* Divisor Latch Access Bit */

/* 8250 Line Status Register */
#define	LSR_DR		1	/* Data ready */
#define	LSR_OE		2	/* Overrun error */
#define	LSR_PE		4	/* Parity error */
#define	LSR_FE		8	/* Framing error */
#define	LSR_BI		0x10	/* Break interrupt */
#define	LSR_THRE	0x20	/* Transmitter line holding register empty */
#define	LSR_TSRE	0x40	/* Transmitter shift register empty */

/* 8250 Interrupt Identification Register */
#define	IIR_IP		1	/* 0 if interrupt pending */
#define	IIR_ID		6	/* Mask for interrupt ID */
#define	IIR_RLS		6	/* Receiver Line Status interrupt */
#define	IIR_RDA		4	/* Receiver data available interrupt */
#define	IIR_THRE	2	/* Transmitter holding register empty int */
#define	IIR_MSTAT	0	/* Modem status interrupt */

/* 8250 interrupt enable register bits */
#define	IER_DAV		1	/* Data available interrupt */
#define	IER_TxE		2	/* Tx buffer empty interrupt */
#define	IER_RLS		4	/* Receive line status interrupt */
#define	IER_MS		8	/* Modem status interrupt */

/* 8250 Modem control register */
#define	MCR_DTR		1	/* Data Terminal Ready */
#define	MCR_RTS		2	/* Request to Send */
#define	MCR_OUT1	4	/* Out 1 (not used) */
#define	MCR_OUT2	8	/* Master interrupt enable (actually OUT 2) */
#define	MCR_LOOP	0x10	/* Loopback test mode */

/* 8250 Modem Status Register */
#define	MSR_DCTS	1	/* Delta Clear-to-Send */
#define	MSR_DDSR	2	/* Delta Data Set Ready */
#define	MSR_TERI	4	/* Trailing edge ring indicator */
#define	MSR_DRLSD	8	/* Delta Rx Line Signal Detect */
#define	MSR_CTS		0x10	/* Clear to send */
#define	MSR_DSR		0x20	/* Data set ready */
#define	MSR_RI		0x40	/* Ring indicator */
#define	MSR_RLSD	0x80	/* Received line signal detect */

extern int _panic();			/* exception handler		*/

extern int cbrkint();			/* ctrl-break handler		*/
extern int clkint();			/* clock interrupt handler	*/
extern int ethinter();			/* Ethernet interrupt handler	*/
extern int ttyiin();			/* keyboard interrupt handler	*/
extern int slinter();			/* COM interrupt handler	*/
extern int nullint();			/* null interrupt handler	*/
