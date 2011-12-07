/* snfreebl.c - snfreebl */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <snmp.h>

/*------------------------------------------------------------------------
 * snfreebl - free memory used for ASN.1 strings and snbentry nodes
 *------------------------------------------------------------------------
 */
snfreebl(bl)
struct snbentry	*bl;
{
	register struct snbentry	*pbl;

    if (bl == NULL)
		return;
	for (pbl = bl, bl = bl->sb_next; bl; pbl = bl, bl = bl->sb_next) {
        freemem(pbl->sb_a1str, pbl->sb_a1slen);
		freemem(pbl, sizeof(struct snbentry));
	}
    freemem(pbl->sb_a1str, pbl->sb_a1slen);
	freemem(pbl, sizeof(struct snbentry));
}
