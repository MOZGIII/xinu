/* x_udp.c - x_udp */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <network.h>
#include <mem.h>

#define BSIZE 520

/*------------------------------------------------------------------------
 *  x_udp  --  ( command udp hostname port)
 *------------------------------------------------------------------------
 */

COMMAND x_udp(nargs,args)
int     nargs;
char    *args[];
{
        int     dgmode = (DG_DMODE | DG_TMODE);
        char    *buf;                         /* datagram buffer */
        IPaddr  ip;                           
        int     dev, i, len;

    if (nargs==1 || nargs > 3) {
        xinu_fprintf(STDERR,"usage: udp hostname [port]\n");
        return(OK);
	}

    if ((buf=xmalloc(BSIZE)) == NULLPTR)
		return (SYSERR);

/* convert name to IPaddr format */

    if (name2ip(ip, args[1]) == SYSERR) {
        xinu_fprintf(STDERR,"udp: unknown host %s \n",args[1]);
        return(OK);
    }

/* convert IPaddr to string format */

    ip2str(buf, ip);
    strcat(buf, ":");               /* attach port number to string */
    if (nargs == 2)
        strcat(buf,"7");
    else  
            if (udp2str(&buf[20], args[2]) == SYSERR) {
            xinu_fprintf(STDERR,"udp: bad port name %s\n",args[2]);
            return(OK);
            }
            else strcat(buf, &buf[20]);
    dev = xinu_open(UDP, buf, ANYLPORT);
    if (isbaddev(dev)) {
        xinu_fprintf(STDERR,"udp: open failed\n");
		return(SYSERR);
	}

/* set UDP data mode with timeout */

    control(dev, DG_SETMODE, &dgmode);

/* send a datagram */

    strcat(buf," chrisc@isepc29.canberra.edu.au");
    xinu_printf("udp: sent %s\n", buf);
    write (dev, buf, xinu_strlen(buf));
    if ((len=read(dev, buf, BSIZE)) == SYSERR) {
        xinu_close(dev);
        xinu_fprintf(STDERR,"udp: read failed\n");
        return(SYSERR);
    }
    if (len == TIMEOUT) xinu_fprintf(STDERR,"udp: no response\n");
    else xinu_printf("udp: rcvd %s\n",buf);
    xinu_close(dev);
    return(OK);
}
