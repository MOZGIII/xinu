/* x_finger.c - x_finger */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

extern  char *rindex();
int control(int, int, long);

/*------------------------------------------------------------------------
 *  x_finger  -  (command finger) format and print logged on users
 *------------------------------------------------------------------------
 */
COMMAND x_finger(nargs, args)
int     nargs;
char	*args[];
{
	char *rhost, *user, *index();
	int	ac = 0;

	if (nargs == 1)
        return lfing(NULLPTR, STDOUT);
	while (++ac < nargs) {
        rhost = rindex(args[ac], '@');
		if (rhost != NULL)
			*rhost++ = '\0';
		if (*args[ac] != '\0')
			user = args[ac];
		else
			user = NULL;
		if (rhost != NULL) {
            rfing(user, rhost);
            *--rhost = '@'; 
		} else
            lfing(user, STDOUT);
	}
    return OK;
}

int
rfing(user, rhost)
char *user, *rhost;
{
	IPaddr	addr;
	char host[22];		/* allows "XXX.XXX.XXX.XXX:XXXXX\0" */
	int fd, cc, wrv;
    long tcpbuf = TCP_BUFFER;
    char buf[2048];

	if (name2ip(addr, rhost) == SYSERR) {
        xinu_write(STDOUT, "unknown host\n", 13);
		return SYSERR;
	}
    xinu_sprintf(host, "%u.%u.%u.%u:79", BYTE(addr, 0), BYTE(addr, 1),
		BYTE(addr, 2), BYTE(addr, 3));

	xinu_sprintf(buf, "[%s", rhost);
    xinu_write(STDOUT, buf, xinu_strlen(buf));

	fd = xinu_open(TCP, host, ANYLPORT);
    if (fd == SYSERR)
		return SYSERR;
	if (fd < 0) {
		xinu_sprintf(buf, "open: %s\n", tcperror[-fd]);
        xinu_write(STDERR, buf, xinu_strlen(buf));
		return SYSERR;
	}
    xinu_write(STDOUT, "]\n", 2);
    if (user != NULL) 
        xinu_write(fd, user, xinu_strlen(user));
	write(fd, "\r\n", 2);
    if (control(fd, TCPC_SOPT, tcpbuf) == SYSERR) {
		xinu_sprintf(buf, "TCPC_SOPT failed\n");
        xinu_write(STDERR, buf, xinu_strlen(buf));
		return SYSERR;
	}
	wrv = 0;
    while ((cc = xinu_read(fd, buf, sizeof(buf))) > 0) {
		if (cc == SYSERR) {
            xinu_write(STDERR, "read returned SYSERR\n", 21);
			break;
		}
        wrv = xinu_write(STDOUT, buf, cc);
		if (wrv < 0)
			break;
	}
	if (cc < 0) {
		xinu_sprintf(buf, "read: %s\n", tcperror[-cc]);
        xinu_write(STDERR, buf, xinu_strlen(buf));
	} else if (wrv < 0) {
		xinu_sprintf(buf, "write: %s\n", tcperror[-wrv]);
        xinu_write(STDERR, buf, xinu_strlen(buf));
	}
	xinu_close(fd);
	return OK;
}
