/* stc_set.c - stc_set */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <snmp.h>
#include <mib.h>
#include <asn1.h>

/*------------------------------------------------------------------------
 * stc_set - return error: the TCP Connections Table is read-only
 *------------------------------------------------------------------------
 */
stc_set(bindl, mip)
struct snbentry	*bindl;
struct mib_info	*mip;
{
	return SERR_NO_SUCH;
}
