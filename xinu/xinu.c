/* xinu.c - xmain */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 *  xmain  --  main xinu driver
 *------------------------------------------------------------------------
 */

xmain(argc,argv)
int	argc;
char	*argv[];
{
    login();
    return OK;
}
