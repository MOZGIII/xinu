
/* rfgetnam.c - rfget_nam */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ctype.h>

/*------------------------------------------------------------------------
 *  rfget_nam  -  parse foreign address specification; get name and port #
 *		  from a name:port format
 *------------------------------------------------------------------------
 */
rfget_nam(cp, nm, portptr)
char 	*cp;
char	*nm;
int	*portptr;
{
	int	i;
	char	ch;

	/* parse forms like xinu.cs.purdue.edu:2001 into (name,udp-port) */

	while ((*cp != ':')  &&  (*cp != '\0')) 
		*nm++ = *cp++;
	*nm = '\0';
	if (*cp == '\0') {
		/* use default port */
		*portptr = RFPORT;
		return(OK);
	}
	if (*cp != ':')
		return(SYSERR);
	i = 0;
	while (isdigit(ch = *cp++))
		i = 10*i + (ch - '0');
	if (i==0 || ch!='\0')
		return(SYSERR);
	*portptr = i;
	return(OK);
}
