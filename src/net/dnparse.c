/* dnparse.c - dnparse */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ctype.h>

/*------------------------------------------------------------------------
 *  dnparse  -  parse foreign address specification; get IP and port #s
 *------------------------------------------------------------------------
 */
int dnparse(fspec, paddr, pport)
char	*fspec;
IPaddr	paddr;
short	*pport;
{
	int	i, byte;
	char	ch;

	if (fspec == ANYFPORT) {
		*pport = 0;
		return OK;
	}
	/* parse forms like 192.5.48.30:3 into (ip-address,port)	*/

	for (i=0 ; i<IP_ALEN ; ++i) {
		byte = 0;
		while (isdigit(ch = *fspec++))
			byte = 10*byte + (ch - '0');
		if (byte > 256 || (i<3 && ch!='.') )
			return SYSERR;
		paddr[i] = byte;
	}
	if (ch != ':')
		return SYSERR;
	i = 0;
	while (isdigit(ch = *fspec++))
		i = 10*i + (ch - '0');
	if (i==0 || ch!='\0')
		return SYSERR;
	*pport = i;
	return OK;
}
