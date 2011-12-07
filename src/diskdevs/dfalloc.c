/* dfalloc.c - dfalloc */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 *  dfalloc  --  allocate a device table entry for a disk file; return id
 *------------------------------------------------------------------------
 */
#ifdef	Ndf
dfalloc()		/* assume exclusion for dir. provided by caller	*/
{
	int	i;

	for (i=0 ; i<Ndf ; i++)
		if (fltab[i].fl_pid == 0) {
			fltab[i].fl_pid = xinu_getpid();
			return(i);
		}
	return(SYSERR);
}
#endif
