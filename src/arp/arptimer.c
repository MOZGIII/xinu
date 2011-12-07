/* arptimer.c - arptimer */

#include <conf.h>
#include <kernel.h>
#include <network.h>

extern void arpdq();

/*------------------------------------------------------------------------
 *
 * arptimer - Iterate through ARP cache, aging (possibly removing) entries
 *------------------------------------------------------------------------
 */
void arptimer(gran)
int	gran;				/* time since last iteration	*/
{
	struct arpentry *pae;
    int     ps;
	int		i;

    disable(ps);    /* mutex */

	for (i=0; i<ARP_TSIZE; ++i) {
		if ((pae = &arptable[i])->ae_state == AS_FREE)
			continue;
		if ((pae->ae_ttl -= gran) <= 0)
			if (pae->ae_state == AS_RESOLVED)
				pae->ae_state = AS_FREE;
			else if (++pae->ae_attempts > ARP_MAXRETRY) {
				pae->ae_state = AS_FREE;
				arpdq(pae);
			} else {
				pae->ae_ttl = ARP_RESEND;
                arpsend(pae);
			}
	}
	restore(ps);
}
