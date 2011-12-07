/* isatty.c - isatty */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <tty.h>

/*------------------------------------------------------------------------
 *  isatty  --  check if the device is a "tty" device
 *------------------------------------------------------------------------
 */
SYSCALL isatty(dev)
int dev;
{
	int		mdevno;

	if (isbaddev(dev) || (mdevno=devtab[dev].dvminor) >= Ntty)
		return(FALSE);
	return (tty[mdevno].dnum == dev);
}
