/* lwattr.c - lwattr */

#include <ctype.h>
#include <conf.h>
#include <kernel.h>
#include <tty.h>

const unsigned int ATTR_BLINK          	= (1 << 7);
const unsigned int ATTR_INTENSE		= (1 << 8);
#define	ATTR_DEFAULT		0x07

#define	newcol(c,n,j)	( ((c) & ~( 7<<(j) )) | ((n) << (j)) )

static char *clrs[] = { "blk","blu","grn","cyn","red","mag","yel","wht" };

/*------------------------------------------------------------------------
 *  lwattr  --  parse window color attribute string
 *------------------------------------------------------------------------
 */
int  lwattr(ap,attr)
char	*ap;
int	attr;				/* default value		*/
{
	char	tmp[4];			/* used to compare with clrs	*/
	int	i,j;
	int	cnum;			/* value of selected attr	*/
	int	shift = 0;		/* shift count			*/
	char *cattr = (char*)&attr;
	char bg = 0;
	char fg = 7;
	
	if ( ap == NULL || *ap == 0 )
		return(attr);
		
	while (  isalnum(*ap) == 0 && *ap != '/' ) {
		if ( *ap == 0 )
			return(attr);
		switch ( *ap++ ) {
			case '*':
				attr &= ~ATTR_BLINK;
				break;
			case '?':
				attr |= ATTR_BLINK;
				break;
			case '-':
				attr &= ~ATTR_INTENSE;
				break;
			case '+':
			 	attr |= ATTR_INTENSE;
				break;
			default:
				return(SYSERR);
		}
	}
	
	
	for( i=0 ; i<3 ; i++ ) { 
		if ( *ap == 0 )
			return(attr);
		if ( i == 1 ) {
			if ( *ap != '/' ) 
				return(SYSERR);
			ap++;
			shift = 8;
			continue;
		}
		if ( isdigit(*ap) )
			cnum = (*ap++) - '0';
		else if ( isalpha(*ap) ) {
			for ( j=0 ; j<3 ; j++ ) {
				if ( isalpha(*ap) == 0 )
					return(SYSERR);
				tmp[j] = tolower(*ap);
				ap++;
			}
			tmp[j] = 0;
			for ( cnum=0 ; cnum<8 ; cnum++ ) {
				if ( strcmp(clrs[cnum],tmp) == 0 )
					break;
			}
		} else 
			continue;
		if ( cnum >= 8 )
			return(SYSERR);
		if (i == 0)
		    fg = cnum;
		else
		    bg = cnum;
	}
	
	*cattr = fg * 8 + bg;
	if ( *ap == 0 )
		return(attr);
	return(SYSERR);
}
