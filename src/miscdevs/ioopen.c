/* ioopen.c - ioopen */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 *  ioopen  --  return descriptor iff name is null
 *------------------------------------------------------------------------
 */
ioopen(devptr, name, mode)
struct	devsw	*devptr;
char	*name;
char	*mode;
{
    return ( (name == NULLPTR || *name == NULLCH) ? devptr->dvnum : SYSERR);
}
