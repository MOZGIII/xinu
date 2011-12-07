/* dgmcntl.c - dgmcntl */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  dgmcntl  -  control the network and datagram pseudo devices
 *------------------------------------------------------------------------
 */
int dgmcntl(pdev, com, addr)
struct	devsw	*pdev;
int		com;
char		*addr;
{
	int	status;

	switch (com)	{
	default:
		status = SYSERR;
	}
	return status;
}
