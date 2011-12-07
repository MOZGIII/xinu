/* x_debug.c    x_debug */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <tty.h>
#include <dos.h>

/*--------------------------------------------------------------------------
 * x_debug  -  a simple debugger
 *--------------------------------------------------------------------------
 */


static struct psnap {
	int     state;
	int     prio;
	char    name[10];
	char    *pbase;
	int     plen;
	char    *pregs;
	int     ctx[4];
} stab[1];

COMMAND x_debug(nargs,args)
int     nargs;
char    *args[];
{
	int     i,j,wd,ps,pid,len;
	char    ch;
	char    buf[80];
	struct  pentry  *pptr;
	struct  psnap   *sptr;
	int     *addr;

for ( ; TRUE ; ){

    xinu_fprintf(STDOUT,"PID > ");
    if ((len=read(STDIN,buf,80))==0)
        len=read(STDIN,buf,80);
	if (len == EOF) break;
	buf[len-1]=NULLCH;
	pid=atoi(buf);

	if (pid>=NPROC) break;

	disable(ps);

	/* copy the process table for process pid */

	pptr=&proctab[pid];
	sptr=&stab[0];
	sptr->state=pptr->pstate;
	sptr->prio=pptr->pprio;
	xinu_strcpy(sptr->name,pptr->pname);
	sptr->pbase=pptr->pbase;
	sptr->plen=pptr->plen;
	sptr->pregs=pptr->pregs;

	restore(ps);

    xinu_fprintf(STDOUT,"pid %3d %s sp= %04x ss= %04x\n",pid,sptr->name,
	FP_OFF(sptr->pregs),FP_SEG(sptr->pregs));

 }
	return(OK);
}

