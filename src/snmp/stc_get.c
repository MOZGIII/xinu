/* stc_get.c - stc_get */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <snmp.h>
#include <mib.h>
#include <asn1.h>

/*------------------------------------------------------------------------
 * stc_get - perform a get on a variable in the TCP connections table
 *------------------------------------------------------------------------
 */
int stc_get(bindl)
struct snbentry	*bindl;
{
	int 	field, tcbn;

	if (stc_match(bindl, &field, &tcbn) == SYSERR)
		return SERR_NO_SUCH;
	switch (field) {
	case 1:		/* tcpConnState */
		SVTYPE(bindl) = ASN1_INT;
		SVINT(bindl) = tcbtab[tcbn].tcb_state; 
		break;
	case 2:		/* tcpConnLocalAddress */
		SVTYPE(bindl) = ASN1_IPADDR;
		blkcopy(SVIPADDR(bindl), tcbtab[tcbn].tcb_lip, IP_ALEN);
		break;
	case 3:		/* tcpConnLocalPort */
		SVTYPE(bindl) = ASN1_INT;
		SVINT(bindl) = tcbtab[tcbn].tcb_lport; 
		break;
	case 4:		/* tcpConnRemAddress */
		SVTYPE(bindl) = ASN1_IPADDR;
		blkcopy(SVIPADDR(bindl), tcbtab[tcbn].tcb_rip, IP_ALEN);
		break;
	case 5:		/* tcpConnRemPort */
		SVTYPE(bindl) = ASN1_INT;
		SVINT(bindl) = tcbtab[tcbn].tcb_rport;
		break;
	default:
		return SERR_NO_SUCH;
	}
	return SNMP_OK;
}
