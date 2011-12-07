/* fs_init.c - fs_init */

#include <fs.h>

/*------------------------------------------------------------------------
 *  fs_init  -  initialize data structures for file server; return device
 *------------------------------------------------------------------------
 */
fs_init()
{
	int	i;
	int	dgd;
	int   	lport;

	freelst = 0;
	lport = RFPORT;
	dgd = xinu_open(UDP, ANYFPORT, &lport);
	if (isbaddev(dgd))
		return(SYSERR);

	/* Initialize open file information cache */
	for (i=0 ; i<MAXNFILES ; i++) {
		fcache[i].prev = NOFILE;
		fcache[i].next = i+1;
	}
	NEWEST = OLDEST = NOFILE;
	return(dgd);
}
