/* x_netstat.c - x_netstat */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <asn1.h>

static	int	noarg(), iarg(), parp();
static	char	*tcpfnames();

/*------------------------------------------------------------------------
 *  x_netstat  -  (command netstat) print network status information
 *------------------------------------------------------------------------
 */
COMMAND x_netstat(nargs, args)
int     nargs;
char	*args[];
{

	if (nargs == 1)
        return noarg(STDOUT, STDERR);
    if (nargs > 2 || *args[1] != 'i') {
        xinu_write(STDERR, "usage: netstat [i]\n", 21);
        return OK;
	}
    switch (args[1][0]) {
    case    'i':    return iarg(STDOUT, STDERR);
	default:
        xinu_write(STDERR, "usage: netstat [i]\n", 21);
        return OK;
	}
}

char	*tcpsnames[] = { "FREE", "CLOSED", "LISTEN", "SYNSENT", "SYNRCVD",
			"ESTABLISHED", "FINWAIT1", "FINWAIT2", "CLOSEWAIT",
			"LASTACK", "CLOSING", "TIMEWAIT" };

#define	NH1 \
"Proto  RQ   SQ  L. Port    Remote IP    R. Port    State    flags   dev\n"
#define NH2 \
"----- ---- ---- ------- --------------- ------- ----------- -----  ----\n"

static	int noarg(xinu_stdout, xinu_stderr)
int	xinu_stdout, xinu_stderr;
{
	char	str[120];
	char	ipa[16];	/* "XXX.XXX.XXX.XXX\0" */
	int	i;

	write(xinu_stdout, NH1, xinu_strlen(NH1));
	write(xinu_stdout, NH2, xinu_strlen(NH2));
	for (i=0; i<Ntcp; ++i) {
		struct tcb	*ptcb = &tcbtab[i];

		if (ptcb->tcb_state == TCPS_FREE)
			continue;
		ip2dot(ipa, ptcb->tcb_rip);
		xinu_sprintf(str, " %3s  %4d %4d  %5d  %15s  %5d %11s %6s %5d\n",
			"tcp", ptcb->tcb_rbcount, ptcb->tcb_sbcount,
			ptcb->tcb_lport, ipa, ptcb->tcb_rport,
			tcpsnames[ptcb->tcb_state], tcpfnames(ptcb->tcb_flags),
			ptcb->tcb_dvnum);
		write(xinu_stdout, str, xinu_strlen(str));
	}
	return(OK);
}
#define IH1 \
"NIF     In    Brc  Bytes Errors  Unpro   Disc    Out    Brc  Bytes   Disc\n"
#define EH1 \
" EC     In    Out   Ints   Over   Spur\n"                             

static	int
iarg(xinu_stdout, xinu_stderr)
int	xinu_stdout, xinu_stderr;
{
    char    str[128];
    int i;

    xinu_write(xinu_stdout, IH1, xinu_strlen(IH1));
    
    for (i=1; i<Net.nif; ++i) {
		struct	netif	*pni = &nif[i];

		if (pni->ni_state == NIS_DOWN &&
		    pni->ni_admstate == NIS_DOWN)
				continue;
        xinu_printf("%3d %6D %6D %6D %6D %6D %6D %6D %6D %6D %6D\n",i,
               pni->ni_inucast,pni->ni_iucast,pni->ni_ioctets,
               pni->ni_ierrors,pni->ni_iunkproto,pni->ni_idiscard,
               pni->ni_onucast,pni->ni_oucast,pni->ni_ooctets,
               pni->ni_odiscard);
	}
    xinu_write(xinu_stdout, EH1, xinu_strlen(EH1));

    for (i=0; i<Neth; ++i) {
        xinu_printf("%3d %6D %6D %6D %6D %6D\n",i,
        eth[i].any, eth[i].out, eth[i].ints, eth[i].over, eth[i].spur);
	}
	return(OK);
}

#define	addflag(f, name) \
	if (flag & f) { \
		*p++ = name; \
		*p = '\0'; \
	}

static
char *
tcpfnames(flag)
{
	static char string[5];
	char	*p;

	p = &string[0];
	*p = '\0';

	addflag(TCBF_RDONE, 'R');
	addflag(TCBF_SDONE, 'S');
	addflag(TCBF_DELACK, 'D');
	addflag(TCBF_BUFFER, 'B');
	return string;
}


struct {
	char		*name;
	unsigned	*value;
} tcpstuff[] =  {
	{ "MaxConn",		(unsigned*)&TcpMaxConn },
	{ "ActiveOpens",	(unsigned*)&TcpActiveOpens },
	{ "PassiveOpens",	(unsigned*)&TcpPassiveOpens },
	{ "AttemptFails",	(unsigned*)&TcpAttemptFails },
	{ "EstabResets",	(unsigned*)&TcpEstabResets },
	{ "CurrEstab",		(unsigned*)&TcpCurrEstab },
	{ "InSegs",         	(unsigned*)&TcpInSegs },
	{ "OutSegs",		(unsigned*)&TcpOutSegs },
	{ "RetransSegs",	(unsigned*)&TcpRetransSegs },
	{ 0, 0}
};
