/* ethwrite.c - ethwrite */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  ethwrite - write a single packet to the ethernet (interruptable)
 *------------------------------------------------------------------------
 */

ethwrite(devptr, pep, len)
struct	devsw	*devptr;
struct  ep      *pep;
int             len;
{
    struct  etblk   *etptr;         /* Ethernet control block ptr */
    int     ps;
        
    if (len > EP_MAXLEN || len <= 0) {      /* check packet length */
        freebuf(pep);
        return(SYSERR);
	}
    if (len < EP_MINLEN)            /* enforce minimum length */
        len = EP_MINLEN;
        
    etptr = (struct etblk *) devptr->dvioblk;
    pep->ep_len = len;
    
    if (psendi(etptr->etoutp, pep) == SYSERR) {     /* non-blocking */
        disable(ps);
        nif[etptr->etintf].ni_odiscard++;
        restore(ps);
        freebuf(pep);
        return SYSERR;
        }
    return(len);            /* len bytes will probably be sent */            
}
