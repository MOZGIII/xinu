/* echod.c - echod */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

int control(int, int, long);
#define ECHOLOG   getdev("status")

/*------------------------------------------------------------------------
 *  echod  -  the TCP echo server
 *------------------------------------------------------------------------
 */
PROCESS echod()
{
	int	fd;
    long ql = 10;                  /* queue for control */
    
	fd = xinu_open(TCP, ANYFPORT, 7);	/* a stub LISTEN state TCP fd */
    control(fd, TCPC_LISTENQ, ql);  /* set the queue length */

	while (TRUE) {
		int	cfd;
		struct	tcpstat tcpstat;
		IPaddr	from;
        char    str[64];

		/* cfd is a CONNECTED TCP stream when control() returns */
        cfd = control(fd, TCPC_ACCEPT, 0L);
        control(cfd, TCPC_STATUS, (long) &tcpstat);
		blkcopy(from, tcpstat.ts_faddr, IP_ALEN);
        xinu_sprintf(str, "\n echod: from %u.%u.%u.%u",
			BYTE(from, 0), BYTE(from, 1),
			BYTE(from, 2), BYTE(from, 3));
        xinu_write(ECHOLOG, str, xinu_strlen(str));

		if (cfd == SYSERR) {
			break;
		}

		resume(create(ECHOP, ECHOSTK, ECHOPRI, ECHOPNAM, 1, cfd));
		/* closed by child */
	}

}

/*------------------------------------------------------------------------
 *  echop  -  handle a particular TCP echo request
 *------------------------------------------------------------------------
 */
PROCESS echop(cfd)
int	cfd;
{
	int	cc, wr;
	char	buf[512];

	while (TRUE) {
		cc = xinu_read(cfd, buf, sizeof(buf));
		if (cc == TCPE_URGENTMODE || cc == TCPE_NORMALMODE)
			continue;
		if (cc <= 0 || cc == SYSERR)
			break;
		wr = xinu_write(cfd, buf, cc);
		if (wr <= 0 || wr == SYSERR)
			break;
	}
	xinu_close(cfd);
}
