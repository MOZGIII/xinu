/* udp2str.c - udp2str */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  udp2str  --  convert a UDP Port name into decimal string rep.
 *------------------------------------------------------------------------
 */
SYSCALL udp2str(cp, sp)
char	*cp;
char    *sp;
{
    int i;
    struct  port    {
            char    portnam[10];
            char    portnum[6];
    };
    struct  port    ptab[12]={
        "echo",     "7",   /* echo server              */
        "discard",  "9",   /* discard packet           */
        "users",    "11",  /* users server             */
        "daytime",  "13",  /* day and time server          */
        "qotd",     "17",  /* quote of the day server      */
        "chargen",  "19",  /* character generator          */
        "time",     "37",  /* time server              */
        "whois",    "43",  /* who is server (user information) */
        "dname",    "53",  /* darpa domain name server     */
        "tftp",     "69",  /* trivial file transfer protocol server*/
        "talk",     "517", /* PC-Xinu talk service         */
        "rwho",     "513"  /* remote who server (ruptime)      */
    };

    if (isdigit(*sp)) {
        xinu_strcpy(cp,sp);
        return (OK);
    }

    for (i=0 ; i < 12 ; i++) {
        if ( strcmp(sp,ptab[i].portnam) != 0 ) ;
        else {
                xinu_strcpy(cp,ptab[i].portnum);
                return (OK);
        }
    }
	return(SYSERR);
}
