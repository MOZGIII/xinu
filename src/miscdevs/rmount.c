/* rmount.c - rmount */

#include <conf.h>
#include <kernel.h>
#include <name.h>
#include <network.h>
#include <ctype.h>

/*------------------------------------------------------------------------
 *  rmount  -  create a connection between a remote file server and an 
 *	       abstract name space
 *------------------------------------------------------------------------
 */
SYSCALL	rmount(prefix, host, replace)
char	*prefix;
char	*host;
char	*replace;
{
	struct	rfmastr	*rfmptr;
	int	port, dev;
	IPaddr	ip;
	char	*cp;
	char	name[MAXRNAME];
	int	ps;
#ifdef RFILESYS
	if ( isdigit(*host) ) {
 		if (rfget_ip(host, ip, &port) == SYSERR)
 			return(SYSERR);
		if (ip2name(name, ip) == SYSERR) {
			xinu_strcpy(name,host);
			for (cp=name ; *cp ; cp++)
				if (*cp == ':') {
					*cp = 0;
					break;
				}
		}
	} else {
		if (rfget_nam(host, name, &port) == SYSERR
			|| name2ip(ip, name) == SYSERR) {
			return(SYSERR);
		}
	}
	disable(ps);
	if ((dev = nopen(RFILESYS, ip, &port)) == SYSERR) {
		restore(ps);
		kputc(0, '\7');
		return(SYSERR);
	}
	rfmptr = (struct rfmastr *) devtab[dev].dvioblk;
	strncpy(rfmptr->rname, name, MAXRNAME);
	rfmptr->rname[MAXRNAME-1] = '\0';

	if (mount(prefix, dev, replace, REMOTE) == SYSERR) {
		close(dev);
		restore(ps);
		return(SYSERR);
	}
	restore(ps);
#endif    
	return(OK);
}
