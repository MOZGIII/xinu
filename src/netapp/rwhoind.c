/* rwhoind.c - rwhoind */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <rwho.h>

extern long net2hl(long);

/*------------------------------------------------------------------------
 *  rwhoind  -  rwho daemon to record info from incoming rwho packets
 *------------------------------------------------------------------------
 */
PROCESS	rwhoind()
{
    int ps;
	int	dev;
	int	len;
	int	i;
    int dgmode = DG_DMODE;
	long	now;
	struct	rwhopac	*rpacptr;
	struct	rwent	*rwptr;

	if ( (dev=xinu_open(UDP, ANYFPORT, UP_RWHO)) == SYSERR ||
          control(dev, DG_SETMODE, &dgmode) == SYSERR)
		panic("rwho_in: cannot open rwho port");
	while (TRUE) {
		if ( (len = xinu_read(dev,Rwho.rbuf,RWMAXP)) == SYSERR )
			continue;
		rpacptr = (struct rwhopac *) Rwho.rbuf;
		rpacptr->rw_host[RMACLEN-1] = '\00';
		for (i=0 ; i<Rwho.rwnent ; i++) {
			rwptr = &Rwho.rwcache[i];
			if (strncmp(rpacptr->rw_host, rwptr->rwmach,
			    RMACLEN) == 0)
				break;
		}
		if (i >= Rwho.rwnent) {
			disable(ps);
			if (Rwho.rwnent >= RWCSIZ) {
				restore(ps);
				continue;
			}
			rwptr = &Rwho.rwcache[Rwho.rwnent++];
			strncpy(rwptr->rwmach, rpacptr->rw_host, RMACLEN);
			restore(ps);
		}
		rwptr->rwboot = net2hl(rpacptr->rw_btim);
		gettime(&now);
		rwptr->rwlast = now;
		rwptr->rwslast = net2hl(rpacptr->rw_sndtim);
		for (i=0 ; i<RWNLOAD ; i++)
			rwptr->rwload[i] = net2hl(rpacptr->rw_load[i]);
		rwptr->rwusers = (len-RWMINP)/sizeof(struct rw_who);
	}
}
