/* tcpswitch.c */

#include <conf.h>
#include <kernel.h>
#include <network.h>

char	*tcperror[] = {
	"no error",
	"connection reset",		/* TCPE_RESET		*/
	"connection refused",		/* TCPE_REFUSED		*/
	"not enough buffer space",	/* TCPE_TOOBIG		*/
	"connection timed out",		/* TCPE_TIMEDOUT	*/
	"urgent data pending",		/* TCPE_URGENTMODE	*/
	"end of urgent data",		/* TCPE_NORMALMODE	*/
	};
/* SEGMENT ARRIVES state processing */

int	tcpclosed(), tcplisten(),tcpsynsent(), tcpsynrcvd(),
	tcpestablished(), tcpfin1(), tcpfin2(), tcpclosewait(),
	tcpclosing(), tcplastack(), tcptimewait();

int	(*tcpswitch[NTCPSTATES])() = {
	ioerr,				/* TCPS_FREE		*/
	tcpclosed,			/* TCPS_CLOSED 		*/
	tcplisten,			/* TCPS_LISTEN 		*/
	tcpsynsent,			/* TCPS_SYNSENT 	*/
	tcpsynrcvd,			/* TCPS_SYNRCVD 	*/
	tcpestablished,			/* TCPS_ESTABLISHED 	*/
	tcpfin1,			/* TCPS_FINWAIT1 	*/
	tcpfin2,			/* TCPS_FINWAIT2 	*/
	tcpclosewait,			/* TCPS_CLOSEWAIT 	*/
	tcplastack,			/* TCPS_LASTACK 	*/
	tcpclosing,			/* TCPS_CLOSING 	*/
	tcptimewait,			/* TCPS_TIMEWAIT 	*/
};
/* Output event processing */

int	tcpidle(), tcppersist(), tcpxmit(), tcprexmt();

int	(*tcposwitch[NTCPOSTATES])() = {
	tcpidle,			/* TCPO_IDLE		*/
	tcppersist,			/* TCPO_PERSIST		*/
	tcpxmit,			/* TCPO_XMIT		*/
	tcprexmt,			/* TCPO_REXMT		*/
};
