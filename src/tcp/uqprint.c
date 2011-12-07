/* uqprint.c - uqprint */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  uqprint -  print a queue of urgent pointers (for debugging)
 *------------------------------------------------------------------------
 */
int uqprint(q)
{
	struct	uqe	*puqe;

	kprintf("\nUQ %d ");
	while (puqe = (struct uqe *)seeq(q))
		kprintf("(s %X l %d) ", puqe->uq_seq, puqe->uq_len);
	kprintf("\n");
}
