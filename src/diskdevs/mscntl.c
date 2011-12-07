/* mscntl.c - mscntl */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>
#include <mffile.h>

/*------------------------------------------------------------------------
 *  mscntl  --  ioctl for the MS-DOS file system
 *------------------------------------------------------------------------
 */
int mscntl(devptr, op, arg, arg2)
struct devsw *devptr;
int op;
char *arg, *arg2;
{
	switch (op) {
	case FLREMOVE:
		return(dos_unlink(arg));
	case FLRENAME:
		return(dos_rename(arg, arg2));
	case FLMKDIR:
		return(dos_mkdir(arg));
	case FLMKDIRS:
		return(dos_mkdirs(arg));
	case FLRMDIR:
		return(dos_rmdir(arg));
	case FLRMDIRS:
		return(dos_rmdirs(arg));
	default:
		return(SYSERR);
	}
	return(OK);
}
