/* sif_match.c - sif_match */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <snmp.h>
#include <mib.h>

/*------------------------------------------------------------------------
 * sif_match - check if a variable exists in the Interfaces Table.
 *------------------------------------------------------------------------
 */
int sif_match(bindl, iface, field, numifaces)
struct snbentry	*bindl;
int		*iface;
int		*field;
int		numifaces;
{
	int 	oidi;

	oidi = SIF_OIDLEN;
	if ((*field = bindl->sb_oid.id[oidi++]) > SNUMF_IFTAB)
		return SYSERR;
	if ((*iface = bindl->sb_oid.id[oidi++]) > numifaces)
		return SYSERR;
	if (oidi != bindl->sb_oid.len)
		return SYSERR;	/* oidi is not at end of objid */
	return OK;
}
