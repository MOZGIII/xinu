/* x_arp.c - x_arp */

#include <conf.h>
#include <kernel.h>
#include <network.h>

static parp(), xarpusage();

/*------------------------------------------------------------------------
 *  x_arp  -  print ARP tables
 *------------------------------------------------------------------------
 */
COMMAND x_arp(nargs, args)
int     nargs;
char	*args[];
{
	if (nargs != 1)
        return xarpusage(STDERR);
    arpprint(STDOUT);
	return OK;
}

static int xarpusage(/*xinu_stderr*/)
{
	char *str="usage: arp\n";

    xinu_write(STDERR, str, xinu_strlen(str));
	return OK;
}
