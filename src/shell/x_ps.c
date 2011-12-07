/* x_ps.c  -  x_ps */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

static char *sttab[9] = {
	"???",
	"current",
	"free",
	"ready",
	"receiving",
	"sleeping ",
	"suspended",
    "waiting",
    "recvtime"
};


static struct psnap {
	int	state;
	int	prio;
	char	name[10];
    word pmalloc;
    long time;
} stab[NPROC];

#define STATE(s)    sttab[((s)>0&&(s)<9)?(s):0]

/*------------------------------------------------------------------------
 *  x_ps  --  (command ps) print process information
 *------------------------------------------------------------------------
 */
COMMAND  x_ps(nargs,args)
int  nargs;
char *args[];
{
    int ps,i,s;
	struct	pentry *pptr;
    struct  psnap  *sptr;
    char    buf[128];
    
	if ( nargs > 1 ) {
        xinu_fprintf(STDERR,"Usage: ps\n");
		return(SYSERR);
	}

    xinu_sprintf(buf, " id state      prio name     memory   time\n");
    xinu_write(STDOUT, buf, xinu_strlen(buf));
    disable(ps);
	for ( i=0; i<NPROC; i++ ) {
		pptr = &proctab[i];
		sptr = &stab[i];
		sptr->state = pptr->pstate;
		sptr->prio = pptr->pprio;
		xinu_strcpy(sptr->name,pptr->pname);
        sptr->pmalloc = pptr->pmalloc;
        sptr->time = pptr->time;
	}
    restore(ps);
	for ( i=0; i<NPROC; i++ ) {
		sptr = &stab[i];	/* pointer to process entry	*/
        if ( (s=sptr->state) != PRFREE ) {
            xinu_sprintf(buf,"%3d %-9s %5d %s %5d %6D\n",
            i,STATE(s),sptr->prio,sptr->name,(sptr->pmalloc * 16),sptr->time);
            xinu_write(STDOUT, buf, xinu_strlen(buf));
        }
	}
}
