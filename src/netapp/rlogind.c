/* rlogind.c - remote login daemon */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <shell.h>

#define RLOGINPORT 513

static void Service();
static void rlog();
static void BeShell();
static long MungeClose();
static void RestoreClose(int, long);
extern shell();


/*------------------------------------------------------------------------
 *  rlogind - Xinu rlogin daemon -- uses PCXinu 7.9 shell
 *------------------------------------------------------------------------
 */
PROCESS rlogind()
{
    int	fd;

    fd = xinu_open(TCP, ANYFPORT, RLOGINPORT);   /* a stub LISTEN state TCP fd */
    control(fd, TCPC_LISTENQ, 5);           /* set the queue length */

    while (TRUE) {
        int fdConnect;

        /* fdConnect is a CONNECTED TCP stream when control() returns */
    
        if ((fdConnect = control(fd, TCPC_ACCEPT)) == SYSERR) {
            break;
        }
        if (control(fdConnect, TCPC_SOPT, (long) TCP_BUFFER) == SYSERR) {
            xinu_fprintf(STDERR, "TCPC_SOPT failed\n");
            break;
        }

        resume(create(Service, 4000, 20,"rlgsvr",1,fdConnect));
    }
}



static void
Service(fd)
    int fd;
{
    struct tcpstat tcpstat;
    char   fromString[128];


    /* see who the connection is from */
    control(fd, TCPC_STATUS, &tcpstat);
    ip2name(tcpstat.ts_faddr,fromString); 
    rlog("Serving connection from %s:%d on dev:%d\n",
	fromString, tcpstat.ts_fport, fd);

    BeShell(fd);

    rlog("logging out of dev:%d\n",fd);
    
    xinu_close(fd);
}



static void BeShell(dev)
int	dev;
{
    long close_routine;
    int pid;

	close_routine = MungeClose(dev);

    pid = create(shell, SHSTK, SHPRIO, "rsh", 3, 0, NULLPTR);
    if (pid == SYSERR) {
        rlog("Could not create remote shell\n");
		return;
	}
    setpdev(pid, STDIN, dev);
    setpdev(pid, STDOUT, dev);
    setpdev(pid, STDERR, dev);
    setnok(xinu_getpid(), pid);
    resume(pid);
	
	/* wait for the message that says the child is done */
	recvclr();
	receive();

	RestoreClose(dev,close_routine);
}




static long MungeClose(dev)
     int dev;
{
    long save;

    save = (long) devtab[dev].dvclose;
	devtab[dev].dvclose = ionull;
	return(save);
}


static void RestoreClose(dev,routine)
     int dev;
     long routine;
{
    devtab[dev].dvclose = (int (*)()) routine;
}


static void rlog(m,a1,a2,a3,a4)
     char *m;
     int a1,a2,a3,a4;
{
    xinu_fprintf(STDOUT,"rlogin: ");
    xinu_fprintf(STDOUT,m,a1,a2,a3,a4);
}
