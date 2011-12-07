/* x_tcp.c - x_tcp */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <mem.h>
#include <tty.h>

int control(int, int, long);

/*------------------------------------------------------------------------
 *  x_tcp  -  (command tcp) send & receive data over a TCP connection
 *------------------------------------------------------------------------
 */
COMMAND x_tcp(nargs, args)
int     nargs;
char	*args[];
{
    char    *buf;
    IPaddr  ip;
    int     dd;
    int     cc;
    int     len;
    long    tcpbuf = TCP_BUFFER;
    
#define BSIZE 512
    
    if ((nargs == 1) || (nargs >3)) {
        xinu_fprintf(STDERR,"usage: tcp hostname [port]\n");
        return OK;
    }
    if ((buf=xmalloc(BSIZE)) == NULLPTR) {
        xinu_fprintf(STDERR,"tcp: out of memory\n");
        return SYSERR;
    }

/* convert name to IPaddr format */

    if (name2ip(ip, args[1]) == SYSERR) {
        xinu_printf("tcp: unknown host %s \n",args[1]);
        return(OK);
    }

/* convert IPaddr to string format */

    ip2str(buf, ip);
    strcat(buf, ":");               /* attach port number to string */
    if (nargs == 3)
        strcat(buf, args[2]);           
    else
        strcat(buf,"7");
    xinu_fprintf(STDERR, "[%s", buf);

    dd = xinu_open(TCP, buf, ANYLPORT);
    if (dd == SYSERR)
		return SYSERR;
    if (dd < 0) {
        xinu_sprintf(buf, "open: %s\n", tcperror[-dd]);
        xinu_write(STDERR, buf, xinu_strlen(buf));
		return SYSERR;
	}
/*    if (control(dd, TCPC_SOPT, tcpbuf) == SYSERR) {
		xinu_sprintf(buf, "TCPC_SOPT failed\n");
        xinu_write(STDERR, buf, xinu_strlen(buf));
		return SYSERR;
	}
*/    xinu_write(STDOUT, "]\n", 2);
    while (TRUE) {
        len = xinu_read(STDIN, buf, BSIZE);
        if (len == SYSERR) break;
        if (len == EOF ) {
            buf[0]=EOFC;
            xinu_write(dd,buf,1);
            break;
        }
        else {
            xinu_write(dd, buf, len);
            cc = xinu_read(dd, buf, BSIZE);
            xinu_write(STDOUT, buf, cc);
        }
	}
    xinu_close(dd);
    return OK;
}
