/* fingerd.c - fingerd */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

int control(int, int, long);
#define FINGERLOG   getdev("status")

/*------------------------------------------------------------------------
 *  fingerd  -  the finger server
 *------------------------------------------------------------------------
 */
PROCESS fingerd()
{
    int i, fd;
    long ql = 5;                   /* queue length for control */
	fd = xinu_open(TCP, ANYFPORT, 79);	/* a stub LISTEN state TCP fd	*/
    i = control(fd, TCPC_LISTENQ, ql);   /* set the queue length     */
    while (TRUE) {
		struct	tcpstat tcpstat;
		char	user[128], str[80];
		IPaddr	from;
		int	cfd, cc;
		/* cfd is a CONNECTED TCP stream when control() returns */
        cfd = control(fd, TCPC_ACCEPT, 0L);
		if (cfd == SYSERR) {
            break;
		}

		/* just handle it directly */

		cc = xinu_read(cfd, user, sizeof(user));
        if (cc < 2) {
			xinu_close(cfd);
			continue;
		}
		user[cc-2] = '\0';	/* format is "[user]\r\n"	*/
        control(cfd, TCPC_STATUS, (long) &tcpstat);
		blkcopy(from, tcpstat.ts_faddr, IP_ALEN);
        xinu_sprintf(str, "\n fingerd: %-10s from %u.%u.%u.%u", user,
			BYTE(from, 0), BYTE(from, 1),
			BYTE(from, 2), BYTE(from, 3));
		write(FINGERLOG, str, xinu_strlen(str));

		lfing(user, cfd);
		xinu_close(cfd);
	}
}
