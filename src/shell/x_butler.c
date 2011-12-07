/* x_bulter.c  -  x_butler */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <dos.h>
#include <q.h>
LOCAL  pprint();
LOCAL qprint(int i, char* qnam, int state, int keyprint);
static char *sttab[8] = {
	"???",
	"current",
	"free",
	"ready",
	"receiving",
	"sleeping ",
	"suspended",
	"waiting"
};


static struct psnap {
	int	state;
	int	prio;
	char	name[10];
	char	*pbase;
	int	plen;
	char	*pregs;
} stab[NPROC];

static int pmark[NPROC];
static char qname[4];


#define CTRLC		3			/* ASCII code for control-C*/
#define STATE(s)	sttab[((s)>0&&(s)<8)?(s):0]
#define next(i)		q[(i)].qnext
#define prev(i) 	q[(i)].qprev
#define key(i)		q[(i)].qkey


/*------------------------------------------------------------------------
 *  x_butler  --  (command butler) print process information
 *------------------------------------------------------------------------
 */
COMMAND  x_butler(nargs,args)
int  nargs;
char *args[];
{
	int	i,j,s;

	if ( nargs > 1 ) {
		xinu_fprintf(STDERR,"Usage: butler\n");
		return(SYSERR);
	}
	pprint();
	xinu_fprintf(STDOUT, "Press any key to continue . . .");
	if ( kgetc(0) == CTRLC )
		return(OK);
	xinu_printf("\n");
	for ( i=0; i<NPROC; i++ )
		pmark[i] = 0;
	/* print the semaphore queues */
	i = NPROC;
	for ( j=0; j<NSEM; j++ ) {
		qname[0] = 'S';
		qname[1] = '0'+(j/10);
		qname[2] = '0'+(j%10);
		qname[3] = 0;
		qprint(i,qname,PRWAIT,0);
		i += 2;
	}
	qprint(i,"RDY",PRREADY,1);
	i += 2;
	qprint(i,"SLP",PRSLEEP,1);
	for ( i=0 ; i<NPROC; i++ )
		if ( pmark[i] == 0 ) {
			s = stab[i].state;
			if ( s == PRWAIT || s == PRREADY || s == PRSLEEP )
				xinu_printf("?%02d: %s %s\n",i,
				    stab[i].name,STATE(s));
		}
}


LOCAL  pprint()
{
	int	i,j;
	int	s;
	int	*addr;
	struct	pentry *pptr;
	struct	psnap *sptr;

	xinu_printf("\n id state      prio name        sp   ss  top");	
	xinu_printf("   di   si flag   bp\n");

	for ( i=0; i<NPROC; i++ ) {
		pptr = &proctab[i];
		sptr = &stab[i];
		sptr->state = pptr->pstate;
		sptr->prio = pptr->pprio;
		xinu_strcpy(sptr->name,pptr->pname);
		sptr->pbase = pptr->pbase;
		sptr->plen = pptr->plen;
		sptr->pregs = pptr->pregs;
	}
	for ( i=0; i<NPROC; i++ ) {
		sptr = &stab[i];	/* pointer to process entry	*/
		if ( (s=sptr->state) != PRFREE ) {
			xinu_printf("%3d %-9s %5d %s",
				i,STATE(s),
				sptr->prio,sptr->name);
			xinu_printf(" %04x %04x %04x",FP_OFF(sptr->pregs),
				FP_SEG(sptr->pregs),sptr->plen);
			if ( s != PRCURR ) {
				addr = (int *) sptr->pregs;
				for (j=0; j<4; j++ )
					xinu_printf(" %04x",*addr++);
			}
			xinu_printf("\n");
		}
	}
}


LOCAL qprint(i,qnam,state,keyprint)
int i;
char *qnam;
int state;
int keyprint;
{
	int	ni,pi,s;

	if ( prev(i) != EMPTY )
		xinu_printf("%s: nonempty qprev header\n",qnam);
	if ( next(i+1) != EMPTY )
		xinu_printf("%s: nonempty qnext header\n",qnam);
	for ( ni=next(i), pi=i ; ni < NPROC ; pi=ni, ni=next(ni) ) {
		xinu_printf("%s: pid=%02d",qnam,ni);
		if ( ni < 0 ) {
			xinu_printf(" ?queue index corrupted!\n");
			break;
		}
		xinu_printf(" %s",stab[ni].name);
		if ( keyprint )
			xinu_printf(" key=%d",key(ni));
		if ( pmark[ni]++ != 0 ) {
			xinu_printf(" ?process on another queue!\n");
			break;
		}
		s = stab[ni].state;
		if ( s != state )
			xinu_printf(" ?illegal state (%d=%s)",s,STATE(s));
		if ( prev(ni) != pi )
			xinu_printf(" ?invalid qprev index (%d)",prev(ni));
		xinu_printf("\n");
	}
	if ( prev(ni) != pi ) {
		xinu_printf("%s: ?invalid qprev index (%d)",qnam,prev(ni));
	}
	if ( ni != i+1 )
		xinu_printf("%s: ?queue terminates incorrectly (%d)",qnam,ni);
	if ( pi != i )
		xinu_printf("----\n");
	return(OK);
}
