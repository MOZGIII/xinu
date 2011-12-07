/* lwgoto.c  -  lwgoto */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <ctype.h>
LOCAL getcoord(char *cstr, CURSOR *w);
/*------------------------------------------------------------------------
 *  lwgoto  --  system call to set a new cursor position
 *------------------------------------------------------------------------
 */
int lwgoto(ttyp,cstr)
struct	tty  *ttyp;
char	*cstr;
{
	CURSOR  c;
	
	if ( getcoord(cstr,&c) == SYSERR )
		return(SYSERR);
	if ( c.row < 0 || c.row >= ttyp->rowsiz ||
	     c.col < 0 || c.col >= ttyp->colsiz )
	     	return(SYSERR);
	ttyp->curcur = c;
	wputcsr(ttyp,c);
	return(OK);	
}

/*------------------------------------------------------------------------
 *  getcoord  --  parse screen coordinates from coordinate string
 *------------------------------------------------------------------------
 */
LOCAL getcoord(cstr,w)
char	*cstr;
CURSOR	*w;
{
	int	j;
	int	coord;
	char	*cstrp=cstr;
	
	for(j=0; j<2; j++) {
		if ( isdigit(*cstrp) == 0 )
			return(SYSERR);
		coord = 0;
		while ( isdigit(*cstrp) ) {
			coord *= 10;
			coord += (*cstrp++) - '0';
		}
		switch(j) {
			case 0:
				w->col  = coord;
				break;
			case 1:
				w->row  = coord;
				break;
		}
		if ( *cstrp ==',' && j == 0 )
			cstrp++;
	}
	if ( *cstrp == NULLCH )
		return(OK);
	return(SYSERR);
}
