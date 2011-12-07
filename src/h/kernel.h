/* kernel.h - isodd */
/* 8088 PC-Xinu version - for IBM PC and Clones */

/* Symbolic constants used throughout Xinu */
#define NULL        0   /* Null pointer for linked lists*/

typedef int             Bool;       /* Boolean type         */
typedef unsigned int	word;		/* word type			*/

#define	FALSE		0		/* Boolean constants		*/
#define	TRUE		1
#define	EMPTY		(-1)		/* an illegal gpq		*/
#define LSYSCALL    long        /* Syscall returning a long */
#define SYSCALL     int     /* System call declaration  */
#define	LOCAL		static		/* Local procedure declaration	*/
#define	INTPROC		int		/* Interrupt procedure  	*/
#define	PROCESS		int		/* Process declaration		*/
#define WORD		word		/* 16-bit word			*/ 
#define	MININT		0100000		/* minimum integer (-32768)	*/
#define	MAXINT		0077777		/* maximum integer (+32767)	*/
#define	MINSTK		0x400		/* minimum process stack size	*/
#define	NULLSTK		0x400		/* process 0 stack size		*/
#define	OK		 1 		/* returned when system	call ok	*/
#define	SYSERR		-1		/* returned when sys. call fails*/

#define	EOF		-2		/* End-if-file (usu. from read)	*/
#define TIMEOUT     -3L      /* time out (usu. recvtime) */
#define TMSGINT     -4L      /* keyboard "intr" key pressed  */
					/* (usu. defined as ^B)		*/
#define TMSGKILL    -5L      /* keyboard "process killed" key*/
					/* pressed (usu. defined as ^C)	*/

#define	NULLCH		'\0'		/* The null character		*/
#define	NULLSTR		""		/* Pointer to empty string	*/
#define	COMMAND		int		/* Shell command declaration	*/
#define	BUILTIN		int		/* Shell builtin ""		*/

#define	LOWBYTE		0377		/* mask for low-order 8 bits	*/
#define	HIBYTE		0177400		/* mask for high 8 of 16 bits	*/
#define	LOW16		0177777		/* mask for low-order 16 bits	*/

#define	STKMAGIC	0125252		/* unusual value for top of stk	*/


/* initialization constants */

#define	INITARGC	1		/* initial process argc		*/
#define	INITSTK		0x800*4		/* initial process stack	*/
#define	INITPRIO	20		/* initial process priority	*/
#define	INITNAME	"xmain"		/* initial process name		*/
#define	INITRET		userret		/* processes return address	*/
#define	INITREG		0		/* initial register contents	*/
#define	QUANTUM		1		/* clock ticks until preemption	*/
#define	INITARGS	1,0		/* initial count/arguments	*/

/* misc. utility functions */

#define	isodd(x)	(01&(int)(x))
#define	disable(x)	(x)=sys_disabl() /* save interrupt status	*/
#define	restore(x)	sys_restor(x)	/* restore interrupt status	*/
#define	enable()	sys_enabl()	/* enable interrupts		*/
#define	halt()		sys_hlt()	/* halt PC-Xinu			*/
#define xdisable(x)	(x)=sys_xdisabl() /* save int & dosflag status	*/
#define xrestore(x)	sys_xrestor(x)	/* restore int & dosflag status	*/
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define max(a,b)    (((a) > (b)) ? (a) : (b))

/* system-specific functions and variables */

extern	int	sys_disabl();		/* return flags & disable ints	*/
extern	void	sys_restor();		/* restore the flag register	*/
extern	void	sys_enabl();		/* enable interrupts		*/
extern	void	sys_wait();		/* wait for an interrupt	*/
extern	void	sys_hlt();		/* halt the processor		*/
extern	int	sys_get_int_flag();	/* return virtual interrupt flag		*/

extern	int	sys_xdisabl();		/* Return interrupts to MS-DOS	*/
extern	void	sys_restor();		/* Interrupts back to Xinu	*/

extern	word	sys_cs;			/* code segment register value	*/

/* process management variables */

extern	int	rdyhead, rdytail;
extern	int	preempt;
extern  long receive();
extern  long recvclr();
extern  long recvtim();
extern  send(int, long);
extern  sendf(int, long);
extern  sendn(int, long);

#include <sys/io.h>
#define coreleft() (128*1024) 
#define outportb(port, byte) outb(port, byte)
#define inportb(port) inb(port)

#define TICK_FREQ 1000   /* Size of XINU timer tick in MICROseconds */
