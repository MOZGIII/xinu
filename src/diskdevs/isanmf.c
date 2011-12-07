/* isanmf.c - isanmf */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <mffile.h>

/*------------------------------------------------------------------------
 *  isanmf  --  check if the device is a MSDOS file device
 *------------------------------------------------------------------------
 */
SYSCALL isanmf(dev)
int dev;
{
	int		mdevno;

    if (isbaddev(dev) || (mdevno=devtab[dev].dvminor) >= Nmf)
		return(FALSE);
    return (mftab[mdevno].mf_id == dev);
}
