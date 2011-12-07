/* tcpwrite.c - tcpwrite */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tcpwrite  -  write one buffer from a TCP pseudo-device
 *------------------------------------------------------------------------
 */
int tcpwrite(pdev, pch, len)
struct	devsw	*pdev;
char		*pch;
int		len;
{
    return tcpwr(pdev, pch, len, TWF_NORMAL);
}
