/* udpqd.c - udpqd */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define MAXQUOTE 600     /* maximum size of quote datagram  */
static  char    buff[MAXQUOTE];  /* here because the stack may be small  */

/*------------------------------------------------------------------------
 *  udpqd  -  UDP quote of the day server (runs forever in background)
 *------------------------------------------------------------------------
 */
PROCESS udpqd()
{
	int	dev, len;
    struct  udp *udpbuf;
    
    if ( (dev=xinu_open(UDP, ANYFPORT, UP_QOTD)) == SYSERR) {
        xinu_printf("udpqd: open fails\n");
		return(SYSERR);
	}
    udpbuf = (struct udp*)buff;
	while ( TRUE ) {
        len = xinu_read(dev, buff, MAXQUOTE);
        xinu_printf("%s\n", udpbuf->u_data);
		write(dev, buff, len);
	}
}
