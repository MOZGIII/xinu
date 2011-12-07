/* x_exit.c - x_exit */

#include <conf.h>
#include <kernel.h>
#include <shell.h>

/*------------------------------------------------------------------------
 *  x_exit  --  (builtin command exit) exit from the shell
 *------------------------------------------------------------------------
 */
BUILTIN	x_exit(nargs, args)
int	nargs;
char	*args[];
{
    return(SHEXIT);
}
