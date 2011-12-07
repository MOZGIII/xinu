/* window.h - window definitions */

#define LWFREE		-1		/* window is free		*/
#define LWLIMBO		0		/* window is in limbo		*/
#define LWALLOC		1		/* window is allocated		*/

#define BW		56		/* white on black attributes	*/
#define WB      	7        	/* black on white attributes */

#define TLBORDER	0xc9
#define TRBORDER	0xbb
#define BLBORDER	0xc8
#define BRBORDER	0xbc
#define HBORDER		0xcd
#define VBORDER		0xba

#define	BORDER		0x100		/* attribute bit for border	*/

/* character display screen - 25 rows x 80 columns */
#define	G_ROWS	25
#define	G_COLS	80

/* cursor position */
typedef	struct	{
	unsigned  char	col;
	unsigned  char	row;
} CURSOR;
