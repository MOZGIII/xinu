/* io.h -  fgetc, fputc, getchar, isbaddev, putchar */

#include <mem.h>

#define	INTVECI	inint		/* input interrupt dispatch routine	*/
#define	INTVECO	outint		/* output interrupt dispatch routine	*/
extern	int	INTVECI();
extern	int	INTVECO();

#define NMAPS	0x20		/* number of intmap entries		*/
struct	intmap	{		/* device-to-interrupt routine mapping	*/
	char	ivec;		/* interrupt number			*/
	char	callinst;	/* the call instruction			*/
	word	intcom;		/* common interrupt code		*/
	int	(*oldisr)();	/* pointer to the old int. ser. routine */
	int	(*newisr)();	/* pointer to the new int. ser. routine */
	word	mdevno;		/* minor device number			*/
	word	iflag;		/* if nonzero, call the old isr		*/
	};

/*
 * NOTE: The intmap structure takes a total of 8 words or 16 bytes
 * per record.
 */

extern struct intmap *sys_imp;	/* pointer to intmap table	*/
extern	int	nmaps;		/* number of active intmap entries	*/
extern	int	intmap();	/* code segment interrupt mapping table	*/

#define BADDEV		(-1)

#define	isbaddev(f)	( (f)<0 || (f)>=NDEVS )
#define xinu_fopen(n,m)	xinu_open(STDIO, (n), (m))
#define xinu_fclose(d)	xinu_close(d)

/* In-line I/O procedures */

#define	getchar()	getc(STDIN)
#define	putchar(ch)	putc(STDOUT,(ch))
#define	fgetc(unit)	getc((unit))
#define fputc(ch,unit)  putc((unit),(ch))

extern int	_doprnt();	/* output formatter			*/
