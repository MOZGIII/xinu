/* x_route.c - x_route */

#include <conf.h>
#include <kernel.h>
#include <network.h>

static	int	xrtusage();
struct	route	*rtget();

/*------------------------------------------------------------------------
 *  x_route  -  add/delete routes
 *------------------------------------------------------------------------
 */
COMMAND x_route(nargs, args)
int     nargs;
char	*args[];
{
	Bool	isadd;
	char	str[80];
	IPaddr	dest, mask, gw;
	int	metric, ttl;
	int	i;

	if (nargs < 4)
        return xrtusage();

	if (strcmp(args[1], "add") == 0)
		isadd = TRUE;
	else if (strcmp(args[1], "delete") == 0)
		isadd = FALSE;
	else
        return xrtusage();
	if ((isadd && nargs != 7) || (!isadd && nargs != 4))
        return xrtusage();
	dot2ip(dest, args[2]);
	dot2ip(mask, args[3]);

	if (isadd) {
		struct route *prt;

		dot2ip(gw, args[4]);

		metric = atoi(args[5]);
		ttl = atoi(args[6]);

        prt = rtget(gw, 1);
		if (prt == NULL) {
			xinu_sprintf(str, "route: no route to gateway\n");
            xinu_write(STDERR, str, xinu_strlen(str));
		}
		if(rtadd(dest,mask,gw,metric,prt->rt_ifnum,ttl) == SYSERR){
			xinu_sprintf(str, "route: add failed\n");
            xinu_write(STDERR, str, xinu_strlen(str));
		}
		rtfree(prt);
	} else {
		if (rtdel(dest, mask) == SYSERR) {
			xinu_sprintf(str, "route: delete failed\n");
            xinu_write(STDERR, str, xinu_strlen(str));
		}
	}

	return(OK);
}

static
xrtusage()
{
    char *str = "usage: route add <dest> <mask> <gateway> <metric> <ttl>\n       route delete <dest> <mask>\n";

    xinu_write(STDERR, str, xinu_strlen(str));
	return OK;
}

