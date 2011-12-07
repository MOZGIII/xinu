/* x_devs.c - x_devs */

#include <conf.h>
#include <kernel.h>
#include <dos.h>
#include <io.h>

LOCAL	char hd1[] =
	"Num Device     mdev port ivec ovec   ioblk   refct\n";
LOCAL	char hd2[] =
	"--- ---------  ---- ---- ---- ---- --------- -----\n";

/*------------------------------------------------------------------------
 *  x_devs  --  (command devs) print main fields of device switch table
 *------------------------------------------------------------------------
 */
COMMAND x_devs(nargs, args)
int nargs;
char *args[];
{
	struct	devsw	*dvp;
	char	str[80];
	int	i;

	if ( nargs != 1 ) {
		xinu_fprintf(STDERR,"Usage: devs\n");
		return(SYSERR);
	}
    xinu_write(STDOUT, hd1, sizeof(hd1));
    xinu_write(STDOUT, hd2, sizeof(hd2));
	for (i=0 ; i<NDEVS ; i++) {
		dvp = &devtab[i];
        xinu_sprintf(str, "%2d. %-9s  %4d %04x %04x %04x %04x:%04x %5d\n",
			i, dvp->dvnam, dvp->dvminor,dvp->dvport, 
			dvp->dvivec, dvp->dvovec,
			seg(dvp->dvioblk), off(dvp->dvioblk),
			dvp->dvrefct);
        xinu_write(STDOUT, str, xinu_strlen(str));    
	}
}
