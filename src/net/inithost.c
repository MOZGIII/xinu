/* inithost.c - inithost */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

extern int  efaceinit();
extern int  ofaceinit();

#if Noth > 0
static  int getsim();
static  int itod[] = { NI_LOCAL, EC0, OTHER1, OTHER2 };
static  IPaddr  iftoip[] = { {0, 0, 0, 0}, {0, 0, 0, 0},
			{130, 10, 59, 0}, {209, 6, 36, 0} };
#else	/* !(Noth > 0) */
static  int itod[] = { -1, EC0 };
#endif	/* Noth > 0 */

/*------------------------------------------------------------------------
 * inithost  --  initialize a gateway's interface structures
 *------------------------------------------------------------------------
 */
int inithost()
{
	int	i, host;

    Net.netpool = mkpool(MAXNETBUF, NETBUFS);
    Net.lrgpool = mkpool(MAXLRGBUF, LRGBUFS);
    Net.sema = screate(1);
	Net.nif = NIF;

#if Noth > 0
	host = getsim();
#endif	/* Noth > 0 */

	for (i=0; i<Net.nif; ++i) {
		/* start with everything 0 */
		bzero(&nif[i], sizeof(nif[i]));
        nif[i].ni_num = i;              /* index of this nif */
        nif[i].ni_state = NIS_DOWN;
		nif[i].ni_ivalid = nif[i].ni_nvalid = FALSE;
		nif[i].ni_svalid = FALSE;
		nif[i].ni_dev = itod[i];
		if (i == NI_LOCAL) {
            /* maxlrgbuf - (extended) ether header */
            nif[i].ni_mtu = MAXLRGBUF-EP_HLEN;
            blkcopy(nif[i].ni_ip, ip_anyaddr, IP_ALEN);
			continue;
		}
		switch(nif[i].ni_dev) {
#ifdef OTHER1
		case OTHER1:
		case OTHER2:
				if (nif[i].ni_dev != host) {
					nif[i].ni_state = NIS_DOWN;
					nif[i].ni_admstate = NIS_DOWN;
					continue;
				}
				ofaceinit(i);
				break;
#endif /* OTHER1 */
        case EC0: efaceinit(i);   /* for the simulation */
				nif[i].ni_state = NIS_DOWN;
				nif[i].ni_admstate = NIS_DOWN;
				break;
		};
	}

	return OK;
}

#if Noth > 0
static int
getsim()
{
	char *prompt = "Othernet number? ";
	char *badval = "Choose '1' or '2'.\n";
	char answer[2];

start:
	write(CONSOLE, prompt, xinu_strlen(prompt));
	read(CONSOLE, answer, sizeof(answer));
	if (answer[0] == '1' && answer[1] == '\n')
		return OTHER1;
	if (answer[0] == '2' && answer[1] == '\n')
		return OTHER2;
	write(CONSOLE, badval, xinu_strlen(badval));
	goto start;
}
#endif /* Noth > 0 */
