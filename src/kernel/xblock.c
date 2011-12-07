/* xblock.c - xbinsert, xbdelete */

#include <conf.h>
#include <kernel.h>
#include <dos.h>
#include <proc.h>
#include <mem.h>

/*------------------------------------------------------------------------
 *  xbinsert  --  insert an XBLOCK into the current process memory list
 *------------------------------------------------------------------------
 */
xbinsert(xmp, nbytes, pid)
XBLOCK	*xmp;
int	nbytes;
int	pid;
{
	struct pentry	*pptr;
	int		xm, xb, xf;
	XBLOCK		*xbp, *xfp;

	xm = (unsigned int)xmp;
	pptr = &proctab[pid];
	xb = (int)pptr->pmlist;
	if (xb == 0)
		panic("xbinsert: missing stack block");
	xbp = (XBLOCK *)memp(xb);
	xf = (int)xbp->xbfore;
	if (xf == 0)
		panic("xbinsert: invalid memory list");
	xfp = (XBLOCK *)memp(xf);
	xmp->xbback = (struct xblock*)xb;
	xmp->xbfore = (struct xblock*)xf;
	xmp->xblen = nbytes;
	xbp->xbfore = (struct xblock*)xm;
	xfp->xbback = (struct xblock*)xm;
	pptr->pmalloc += nbytes;
}

/*------------------------------------------------------------------------
 *  xbdelete  --  delete an XBLOCK from the current process memory list
 *------------------------------------------------------------------------
 */
word
xbdelete(xmp, pid)
XBLOCK	*xmp;
int	pid;
{
	struct pentry	*pptr;
	int		xf, xm, xb, sb;
	XBLOCK		*xfp, *xbp;
	int		nbytes;

	xm = seg(xmp);
	pptr = &proctab[pid];
	sb = (int)pptr->pmlist;
	if (sb == 0)
		panic("xbdelete: missing stack block");
	if (sb == xm)
		panic("xbdelete: cannot delete stack block");
	nbytes = xmp->xblen;
	pptr->pmalloc -= nbytes;
	xb = (int)xmp->xbback;
	if ( xb == 0 )
		panic("xbdelete: empty back pointer");
	xbp = (XBLOCK *)memp(xb);
	if ( (int)xbp->xbfore != xm )
		panic("xbdelete: inconsistent back pointer");
	xf = (int)xmp->xbfore;
	if (xf == 0)
		panic("xbdelete: empty fore pointer");
	xfp = (XBLOCK *)memp(xf);
	if ((int)xfp->xbback != xm)
		panic("xbdelete: inconsistent fore pointer");
	xfp->xbback = (struct xblock*)xb;
	xbp->xbfore = (struct xblock*)xf;
	return(nbytes);
}
