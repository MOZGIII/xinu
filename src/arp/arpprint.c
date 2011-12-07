/* arpprint - arpprint */

#include <conf.h>
#include <kernel.h>
#include <network.h>

static	printone();

/*------------------------------------------------------------------------
 * arpprint - print the ARP table on descriptor fd
 *------------------------------------------------------------------------
 */
arpprint(fd)
int	fd;
{
	int	i;

    for (i = 0; i<ARP_TSIZE; ++i)
     if (arptable[i].ae_state != AS_FREE)
			printone(fd, &arptable[i]);
	return OK;
}

char	*index();

/*------------------------------------------------------------------------
 * printone - print one entry in the ARP table
 *------------------------------------------------------------------------
 */
static
printone(fd, pae)
int	fd;
struct	arpentry	*pae;
{
	char	line[128], *p;
	int	i, inum;

	if (pae->ae_state == AS_RESOLVED)
		xinu_strcpy(line, "RESOLVED ");
	else
		xinu_strcpy(line, "PENDING  ");
	p = index(line, '\0');

	for (i=0; i<pae->ae_prlen-1; ++i) {
		xinu_sprintf(p, "%u.", pae->ae_pra[i] & 0xff);
		p = index(p, '\0');
	}
	xinu_sprintf(p, "%u ", pae->ae_pra[i] & 0xff);
	p = index(p, '\0');
    inum = pae->ae_pni - &nif[0];
	for (i=0; i<pae->ae_hwlen-1; ++i) {
		xinu_sprintf(p, "%02x:", pae->ae_hwa[i] & 0xff);
		p = index(p, '\0');
	}
	xinu_sprintf(p, "%x  hw %d pr %x inum %d ttl %d s\n",
		pae->ae_hwa[i]&0xff, pae->ae_hwtype, pae->ae_prtype,
		inum, pae->ae_ttl);
	p = index(p, '\0');
	return xinu_write(fd, line, xinu_strlen(line));
}
