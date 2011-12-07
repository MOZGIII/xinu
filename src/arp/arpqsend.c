/* arpqsend.c - arpqsend */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <q.h>

/*------------------------------------------------------------------------
 * arpqsend - write packets queued waiting for an ARP resolution
 *------------------------------------------------------------------------
 */
void arpqsend(pae)
struct	arpentry	*pae;
{
	struct	ep	*pep;
	struct	netif	*pni;

	if (pae->ae_queue == EMPTY)
		return;

	pni = pae->ae_pni;
	while (pep = (struct ep *)deq(pae->ae_queue))
		netwrite(pni, pep, pep->ep_len);
	freeq(pae->ae_queue);
	pae->ae_queue = EMPTY;
}
