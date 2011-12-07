/* dscntl.c - dscntl */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <disk.h>
#include <file.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  dscntl  --  control disk driver/device
 *------------------------------------------------------------------------
 */
dscntl(devptr, func, arg1, arg2)
struct	devsw	*devptr;
int	func;
char	*arg1, *arg2;
{
	int	stat;
	int	ps;
	int	dsksync();
	int	dskzap();
	int	dskname();
	int	dskmkfs();

	disable(ps);
	switch (func) {
		case DSKSYNC:
			stat = dsksync(devptr);
			break;
		case FLREMOVE:
			stat = dskzap(devptr->dvnum,arg1);
			break;
		case FLRENAME:
			stat = dskname(devptr->dvnum,arg1,arg2);
			break;
		case FLACCESS:
			stat = SYSERR;
			break;
		case FLFORMAT:
			stat = dskmkfs(devptr->dvnum,28,720,atoi(arg1));
			break;
		case FLCHDSK:
			stat = dskchdsk(devptr);
			break;
		default:
			stat = SYSERR;
			break;
	}
	restore(ps);
	return(stat);
}
