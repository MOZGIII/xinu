/* xioopen.c -- xioopen */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <disk.h>
#include <file.h>
#include <name.h>
#include <sio.h>

/*------------------------------------------------------------------------
 *  xioopen  --  open a standard I/O device for this process
 *------------------------------------------------------------------------
 */
xioopen(devptr, name, mode)
struct devsw *devptr;
char *name;
char *mode;
{
	int		dev;

    if (name == NULLPTR || *name == NULLCH)
		return(SYSERR);
	if ((dev=newsiodv(xinu_open(NAMESPACE, name, mode))) == SYSERR)
		return(SYSERR);
	return(dev);
}
