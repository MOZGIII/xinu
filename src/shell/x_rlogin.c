/* x_rlogin.c - x_rlogin */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <mem.h>

int control(int, int, long);
int kbdread(int);

/*------------------------------------------------------------------------
 *  x_rlogin  -  (command rlogin) login to remote Xinu system
 *------------------------------------------------------------------------
 */
COMMAND x_rlogin(nargs, args)
int     nargs;
char	*args[];
{
	IPaddr	addr;
	char host[22];		/* allows "XXX.XXX.XXX.XXX:XXXXX\0" */
    int fd, cc, pid;
    long tcpbuf = TCP_BUFFER;
    char buf[256];

    if (nargs != 2) {
        xinu_fprintf(STDERR,"usage: rlogin host\n");
        return SYSERR;
    }
    if (name2ip(addr, args[1]) == SYSERR) {
        xinu_fprintf(STDERR, "rlogin: unknown host\n");
		return SYSERR;
	}

    xinu_sprintf(host, "%u.%u.%u.%u:513", BYTE(addr, 0), BYTE(addr, 1),
		BYTE(addr, 2), BYTE(addr, 3));

    xinu_sprintf(buf, "[%s", args[1]);
    xinu_write(STDOUT, buf, xinu_strlen(buf));
    fd = xinu_open(TCP, host, ANYLPORT);
    if (fd == SYSERR)
		return SYSERR;
	if (fd < 0) {
        xinu_sprintf(buf, "\nopen: %s\n", tcperror[-fd]);
        xinu_write(STDERR, buf, xinu_strlen(buf));
		return SYSERR;
	}
    xinu_write(STDOUT, "]\n", 2);
    if (control(fd, TCPC_SOPT, tcpbuf) == SYSERR) {
		xinu_sprintf(buf, "TCPC_SOPT failed\n");
        xinu_write(STDERR, buf, xinu_strlen(buf));
		return SYSERR;
	}
    pid = create(kbdread, INITSTK, INITPRIO, "kbdread",1,fd);
    if (pid == SYSERR) {
        xinu_fprintf(STDERR,"rlogin: create kbdread returned SYSERR\n");
        xinu_close(fd);
        return SYSERR;
    }
    resume(pid);
    while ((cc = xinu_read(fd, buf, sizeof(buf))) > 0) {
		if (cc == SYSERR) {
            xinu_write(STDERR, "read returned SYSERR\n", 21);
			break;
		}
        if (cc < 0) {
            xinu_printf("rlogin: connection closed\n");
            xinu_sprintf(buf, "read: %s\n", tcperror[-cc]);
            xinu_write(STDERR, buf, xinu_strlen(buf));
            break;
        } else {
            buf[cc]='\0';
            xinu_write(STDOUT, buf, xinu_strlen(buf));
/*            xinu_printf("%s",buf);               */
        }
    }
    xinu_close(fd);
    kill(pid);
    return OK;
}

PROCESS kbdread(fd)
    int fd;
{
    
    char    *buf;
    int     n;
    
    if ((buf=(char *) xmalloc(256)) == NULLPTR)
        return SYSERR;
    while (TRUE) {
        n = xinu_read(STDIN, buf, 256);
        xinu_write(fd, buf, n);
    }
}   
