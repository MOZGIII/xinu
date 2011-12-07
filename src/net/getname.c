/* getname.c - getname, getiname */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  getname  -  get name of this host and place it where specified
 *------------------------------------------------------------------------
 */
SYSCALL getname(nam)
char	*nam;
{
	return getiname(NI_PRIMARY, nam);
}

/*------------------------------------------------------------------------
 *  getiname  -  get name of this host and place it where specified
 *------------------------------------------------------------------------
 */
int getiname(inum, nam)
int	inum;
char	*nam;
{
	struct	netif *intf;
	IPaddr	myaddr;
	char	*p;

	if (inum < 0 || inum >= Net.nif)
		return SYSERR;
	intf = &nif[inum];
	*nam = NULLCH;
    if (!intf->ni_nvalid) {
		getiaddr(inum, myaddr);
        intf->ni_nvalid = TRUE;
        if (ip2name(myaddr, intf->ni_name) == SYSERR) {
            xinu_strcpy(intf->ni_name, "XINU");
            return SYSERR;
        }
	}
	for (p=intf->ni_name ; p && *p != NULLCH; )
		*nam++ = *p++;
	*nam = NULLCH;
	return OK;
}
