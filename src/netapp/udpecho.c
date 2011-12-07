/* udpecho.c - udpecho */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <mem.h>

#define ECHOLOG getdev("status")

/*------------------------------------------------------------------------
 *  udpecho  -  UDP echo server process (runs forever in background)
 *------------------------------------------------------------------------
 */
PROCESS udpecho()
{
    int     dev, len;
    IPaddr  from;
    char    str[32];
    struct xgram *buf;  
    
    buf = (struct xgram *) xmalloc(sizeof (struct xgram));

	if ( (dev=xinu_open(UDP, ANYFPORT, UP_ECHO)) == SYSERR) {
		xinu_printf("udpecho: open fails\n");
		return(SYSERR);
	}
	while ( TRUE ) {
        len = xinu_read(dev, buf, sizeof(struct xgram));
        blkcopy(from, buf->xg_fip, IP_ALEN);
        xinu_sprintf(str, "\n udpecho: from %u.%u.%u.%u",
			BYTE(from, 0), BYTE(from, 1),
			BYTE(from, 2), BYTE(from, 3));
        xinu_write(ECHOLOG, str, xinu_strlen(str));
        xinu_write(dev, buf, len);
	}
}
