/* tcpputc.c - tcpputc */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tcpputc  -  write one character to a TCP pseudo-device
 *------------------------------------------------------------------------
 */
int tcpputc(pdev, ch)
struct	devsw	*pdev;
char		ch;
{
	return tcpwr(pdev, &ch, 1, TWF_NORMAL);
}
