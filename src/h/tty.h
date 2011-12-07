/* tty.h */

#include <window.h>			/* window definitions		*/

#define	OBMINSP		20		/* min space in buffer before	*/
#define	EBUFLEN		32		/* size of echo queue		*/
#define	TTYOPRIO	100		/* priority of tty output	*/
#define TTYIPRIO	(TTYOPRIO+1)	/* priority of tty input	*/

/* size constants */

#ifndef	Ntty
#define	Ntty		1		/* number of serial tty lines	*/
#endif
#ifndef	IBUFLEN
#define	IBUFLEN		128		/* num.	chars in input queue	*/
#endif
#ifndef	OBUFLEN
#define	OBUFLEN		256		/* num.	chars in output	queue	*/
#endif

/* mode constants */

#define	IMRAW		'R'		/* raw mode => nothing done	*/
#define	IMCOOKED	'C'		/* cooked mode => line editing	*/
#define	IMCBREAK	'K'		/* honor echo, etc, no line edit*/
#define	OMRAW		'R'		/* raw mode => normal processing*/

struct	tty	{			/* tty line control block	*/
	int	ihead;			/* head of input queue		*/
	int	itail;			/* tail of input queue		*/
	int	ibuff[IBUFLEN];		/* input buffer for this line	*/
	int	icnt;
	int	isem;			/* input semaphore		*/
	int	ohead;			/* head of output queue		*/
	int	otail;			/* tail of output queue		*/
	char	obuff[OBUFLEN];		/* output buffer for this line	*/
	int	ocnt;
	int	osem;			/* output semaphore		*/
	int	odsend;			/* sends delayed for space	*/
	int	ehead;			/* head of echo queue		*/
	int	etail;			/* tail of echo queue		*/
	int	ebuff[EBUFLEN];		/* echo queue			*/
	int	ecnt;
	char	imode;			/* IMRAW, IMCBREAK, IMCOOKED	*/
	Bool	iecho;			/* is input echoed?		*/
	Bool	ieback;			/* do erasing backspace on echo?*/
	Bool	evis;			/* echo control chars as ^X ?	*/
	Bool	ecrlf;			/* echo CR-LF for newline?	*/
	Bool	icrlf;			/* map '\r' to '\n' on input?	*/
	Bool	ierase;			/* honor erase character?	*/
	char	ierasec;		/* erase character (backspace)	*/
	Bool	ikill;			/* honor line kill character?	*/
	char	ikillc;			/* line kill character		*/
	Bool	iintr;			/* is interrupt char honored?	*/
	char	iintrc;			/* interrupt character		*/
	int	iintpid;		/* interrupt process id		*/
	char	iintkc;			/* process kill character	*/
	Bool	ieof;			/* honor end-of-file char?	*/
	char	ieofc;			/* end-of-file character	*/
	int	icursor;		/* current cursor position	*/
	Bool	oflow;			/* honor ostop/ostart?		*/
	Bool	oheld;			/* output currently being held?	*/
	char	ostop;			/* character that stops output	*/
	char	ostart;			/* character that starts output	*/
	Bool	ocrlf;			/* output CR/LF for LF ?	*/
	char	ifullc;			/* char to send when input full	*/
	int	dnum;			/* device number of this window	*/
	int	oprocnum;		/* output server process id 	*/
	int	wstate;			/* window state (window)	*/
					/* input server process id (tty)*/
	int	ocount;			/* # of active opens on this dev*/
	int	seq;			/* sequence changed at creation	*/
	int	colsiz;			/* logical window column size	*/
	int	rowsiz;			/* logical window row size	*/
	char	attr;			/* character attributes		*/
	CURSOR	curcur;			/* current cursor pos. in win	*/
	CURSOR	topleft;		/* top left corner of window	*/
	CURSOR	botright;		/* bottom right corner of window*/
	Bool	hasborder;		/* does window have a border	*/
};

extern	struct	tty tty[];

#define	BACKSP	'\b'
#define DELKEY	0x7f
#define	EOFC	'\004'
#define	BELL	'\07'
#define	ATSIGN	'@'
#define	BLANK	' '
#define ESC     '\033' 
#define NEWLINE '\n'
#define	RETURN	'\r'
#define TAB	'\t'
#define TABSTOP	8
#define	TFULLC		BELL		/* char to echo when buffer full*/
#define	STOPCH	'\023'			/* control-S stops output	*/
#define	STRTCH	'\021'			/* control-Q restarts output	*/
#define	INTRCH	'\002'			/* control-B is interrupt	*/
#define KILLCH	'\003'			/* control-C is kill		*/
#define SUSPCH	'\032'			/* control-Z is suspend		*/
#define	UPARROW	'^'			/* usually for visuals like ^X	*/
#define	IOCHERR	0200			/* bit set on when an error	*/
					/* occurred reading the char.	*/

/* special function keys */

#define	SPECKEY	0x100			/* special function key offset	*/
#define FKEY	0x13b			/* F1				*/
#define	CFKEY	0x15e			/* control-F1			*/
#define PSNAPK	0			/* offset for process snapshot	*/
#define TSNAPK	1			/* offset for tty snapshot	*/
#define DSNAPK  2			/* offset for disk snapshot	*/


/* ttycontrol function codes */

#define	TCSETBRK	1		/* turn on BREAK in transmitter	*/
#define	TCRSTBRK	2		/* turn off BREAK "       "	*/
#define	TCNEXTC		3		/* look ahead 1 character	*/
#define	TCMODER		4		/* set input mode to raw	*/
#define	TCMODEC		5		/* set input mode to cooked	*/
#define	TCMODEK		6		/* set input mode to cbreak	*/
#define	TCICHARS	8		/* return number of input chars	*/
#define	TCECHO		9		/* turn on echo			*/
#define	TCNOECHO	10		/* turn off echo		*/
#define	TCINT		11		/* set input interrupt pid	*/
#define	TCINTCH		12		/* set input interrupt char	*/
#define	TCNOINT		13		/* turn off input interrupt	*/
#define	TCCLEAR		14		/* clear the current window	*/
#define	TCCURPOS	15		/* put cursor at position (x,y)	*/
#define	TCATTR		16		/* change the window's attribute*/
#define TCFLUSH		17		/* flush the buffer - cooked md	*/
#define TCNEXTCI	18		/* look ahead immediate 1 char	*/
#define TCMODEQ		19		/* query the input mode		*/
#define TCMODES		20		/* set the input mode		*/
#define TCECHOQ		21		/* query the echo mode		*/
#define TCECHOS		22		/* set the echo mode		*/
#define TCINTQ		23		/* query interrupt pid		*/
#define TCINTS		24		/* set interrupt pid		*/
#define TCFASTP     25      /* put char on screen fast (raw only) */

/* messages passed to output process */
#define	TMSGOK		0		/* all OK			*/
#define	TMSGEFUL	1		/* echo buffer overflow		*/


extern	int	kprintf();		/* formatted console print	*/
extern	int	xinu_printf();		/* XON/XOFF console print	*/
extern	int	wputcsr();		/* put cursor routine		*/

extern	int	winofcur;		/* cur window of cursor		*/
