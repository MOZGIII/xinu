/* rfread.c - rfread */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rfread  --  read one or more bytes from a remote file
 *------------------------------------------------------------------------
 */
rfread(devptr, buff, len)
struct	devsw	*devptr;
char	*buff;
int	len;
{
	int	rlen;

	len = rfio(devptr, FS_READ, buff, min(len,RDATLEN));
	if (len == 0)
		return(EOF);
	return(len);
}
