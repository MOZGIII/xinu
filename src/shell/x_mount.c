/* x_mount.c - x_mount, mprint */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <name.h>
#include <network.h>
LOCAL	mprint();
/*------------------------------------------------------------------------
 *  x_mount  --  (command mount) change or display namespace table
 *------------------------------------------------------------------------
 */
COMMAND	x_mount(nargs, args)
int	nargs;
char	*args[];
{
	int	dev;
	char	*cp;

	if (nargs == 1)
		return(mprint());
	if (nargs<3 || nargs>4) {
		xinu_fprintf(STDERR,"Usage: mount\n");
		xinu_fprintf(STDERR,"       mount prefix device [new_prefix]\n");
		return(SYSERR);
	}
	cp = "";
	if (nargs == 4)
		cp = args[3];
	for (dev=0 ; dev<NDEVS ; dev++)
		if (strcmp(args[2], devtab[dev].dvnam) == 0)
			break;
	if (dev >= NDEVS)
		if (strcmp(args[2],"SYSERR") == 0)
			dev = SYSERR;
		else {
			xinu_fprintf(STDERR, "mount: device %s not found\n", args[2]);
			return(SYSERR);
		}
	if (mount(args[1], dev, cp, RLOCAL) == SYSERR) {
		xinu_fprintf(STDERR, "mount: mount failed\n");
		return(SYSERR);
	}
	return(OK);
}

/*------------------------------------------------------------------------
 *  mprint  -  print the current contents of the namespace prefix table
 *------------------------------------------------------------------------
 */
LOCAL	mprint()
{
	struct	nament	*nptr;
	int	i, len, dev;
	char	*p;

	for (i=0 ; i<Nam.nnames ; i++) {
		nptr = & Nam.nametab[i];
		xinu_printf("\"%-24s\"", nptr->npre);
		dev = nptr->ndev;
		if (isbaddev(dev))
			p = "SYSERR";
		else if (nptr->ntyp == REMOTE)
			p = ((struct rfmastr *)devtab[dev].dvioblk)->rname;
		else
			p = devtab[dev].dvnam;
        xinu_printf(" -> (%-12.12s) \"%s\"\n", p, nptr->nrepl);
	}
	return(OK);
}
