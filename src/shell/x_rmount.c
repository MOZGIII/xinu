/* x_rmount.c - x_rmount, rmprint */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <io.h>
#include <name.h>
LOCAL	rmprint();
/*------------------------------------------------------------------------
 *  x_rmount  --  (command rmount) change or display namespace table
 *		 remote entries only		
 *------------------------------------------------------------------------
 */
COMMAND	x_rmount(nargs, args)
int	nargs;
char	*args[];
{
	int	dev;
	char	*cp;

	if (nargs == 1)
		return(rmprint());
	if (nargs<3 || nargs>4) {
		xinu_fprintf(STDERR,"Usage: rmount\n");
		xinu_fprintf(STDERR,"       rmount prefix host [new_prefix]\n");
		return(SYSERR);
	}
	cp = "";
	if (nargs == 4)
		cp = args[3];
	if (rmount(args[1], args[2], cp) == SYSERR) {
		xinu_fprintf(STDERR, "rmount: rmount failed\n");
		return(SYSERR);
	}
	return(OK);
}



/*------------------------------------------------------------------------
 * rmprint  -  print the current contents of the namespace prefix table
 *	       remote entries only		
 *------------------------------------------------------------------------
 */
LOCAL	rmprint()
{
	struct	nament	*nptr;
	int	i, len, dev, count;
	char	str[80];
	char	*p;
	struct	rfmastr	*rfmp;

	for (i=0 ; i<Nam.nnames ; i++) {
		nptr = & Nam.nametab[i];
		if (nptr->ntyp != REMOTE)
			continue;
		xinu_printf("\"%-24s\"", nptr->npre);
		dev = nptr->ndev;
		if (isbaddev(dev)) {
			count = 0;
			p = "SYSERR";
		} else {
			rfmp = (struct rfmastr *)devtab[dev].dvioblk;
			count = rfmp->rcount;
			p = rfmp->rname;
		}
		xinu_printf(" -> (%s=%d) \"%s\"\n", p, count, nptr->nrepl);
	}
	return(OK);
}
