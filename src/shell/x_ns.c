/* x_ns.c - x_ns */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  x_ns  -  (command ns) interact with the host name server cache
 *------------------------------------------------------------------------
 */
COMMAND x_ns(nargs, args)
int nargs;
char	*args[];
{
	char	str[128], ipstr[64];
	IPaddr	ip;
	int	ac;

	for (ac=1; ac < nargs; ++ac) {
		if (name2ip(ip, args[ac]) == SYSERR)
			xinu_strcpy(ipstr, "not resolved");
		else
			xinu_sprintf(ipstr, "%u.%u.%u.%u", BYTE(ip, 0), BYTE(ip, 1),
				BYTE(ip, 2), BYTE(ip, 3));
		xinu_sprintf(str, "\"%s\": %s\n", args[ac], ipstr);
        xinu_write(STDOUT, str, xinu_strlen(str));
	}
	return OK;
}
