/* x_dos.c - x_dos */

#include <conf.h>
#include <kernel.h>
#include <shell.h>

/*------------------------------------------------------------------------
 *  x_dos  --  (builtin command dos) terminate XINU and return to DOS
 *------------------------------------------------------------------------
 */
BUILTIN x_dos(nargs, args)
int	nargs;
char	*args[];
{
    scrollup(0,0x184f,0,0);     /* clear the screen */
    xdone();                    
    return(OK);
}
