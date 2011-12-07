/* rfgetip.c - rfget_ip */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ctype.h>

/*------------------------------------------------------------------------
 *  rfget_ip  -  parse foreign address specification; get IP and port #s
 *------------------------------------------------------------------------
 */
rfget_ip(cp, ip, portptr)
char 	*cp;
char	*ip;
int	*portptr;
{
	int	i, byte;
	char	ch;
	char	*ipptr;


	/* parse forms like 128.10.3.30:2001 into (ip-address,udp-port) */

	for (i=0 ; i<4 ; i++) {
		byte = 0;
		while ( isdigit((ch=*cp++)) )
			byte = 10*byte + (ch - '0');
		if (byte > 256 || (i<3 && ch!='.') )
			return(SYSERR);
		*ip++ = (char) byte;
	}
	if (ch == '\0') {
		/* use default port */
		*portptr = RFPORT;
		return(OK);
	}
	if (ch != ':')
		return(SYSERR);
	i = 0;
	while (isdigit(ch = *cp++))
		i = 10*i + (ch - '0');
	if (i==0 || ch!='\0')
		return(SYSERR);
	*portptr = i;
	return(OK);
}
