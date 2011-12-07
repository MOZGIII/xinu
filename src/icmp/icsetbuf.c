/* icsetbuf.c - icsetbuf */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  icsetbuf -  set up a buffer for an ICMP message
 *------------------------------------------------------------------------
 */
struct ep *icsetbuf(type, pa1, pisresp, piserr)
int	type;
char	*pa1;			/* old packet, if any	*/
Bool	*pisresp,		/* packet is a response	*/
	*piserr;		/* packet is an error	*/
{
	struct	ep	*pep;

	*pisresp = *piserr = FALSE;

	switch (type) {
	case ICT_REDIRECT:
        pep = (struct ep *)getbufi(Net.netpool);
        if (pep == NULL)
            return(NULL);
		blkcopy(pep, pa1, MAXNETBUF);
		pa1 = (char *)pep;
		*piserr = TRUE;
		break;
	case ICT_DESTUR:
	case ICT_SRCQ:
	case ICT_TIMEX:
	case ICT_PARAMP:
		pep = (struct ep *)pa1;
		*piserr = TRUE;
		break;
	case ICT_ECHORP:
	case ICT_INFORP:
	case ICT_MASKRP:
		pep = (struct ep *)pa1;
		*pisresp = TRUE;
		break;
	case ICT_ECHORQ:
	case ICT_TIMERQ:
	case ICT_INFORQ:
	case ICT_MASKRQ:
        pep = (struct ep *)getbufi(Net.lrgpool);
        if (pep == NULL)
            return(NULL);
		break;
	case ICT_TIMERP:		/* Not Implemented */
		/* IcmpOutTimestampsReps++; */
		IcmpOutErrors--;	/* Kludge: we increment above */
		freebuf(pa1);
        return(NULL);
	}
	switch (type) {		/* Update MIB Statistics */
	case ICT_ECHORP:	IcmpOutEchos++;		break;
	case ICT_ECHORQ:	IcmpOutEchoReps++;	break;
	case ICT_DESTUR:	IcmpOutDestUnreachs++;	break;
	case ICT_SRCQ:		IcmpOutSrcQuenchs++;	break;
	case ICT_REDIRECT:	IcmpOutRedirects++;	break;
	case ICT_TIMEX:		IcmpOutTimeExcds++;	break;
	case ICT_PARAMP:	IcmpOutParmProbs++;	break;
	case ICT_TIMERQ:	IcmpOutTimestamps++;	break;
	case ICT_TIMERP:	IcmpOutTimestampReps++;	break;
	case ICT_MASKRQ:	IcmpOutAddrMasks++;	break;
	case ICT_MASKRP:	IcmpOutAddrMaskReps++;	break;
	}
	return pep;
}
