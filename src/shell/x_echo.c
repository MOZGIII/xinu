/* x_echo.c - x_echo */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 *  x_echo  -  (command echo) echo arguments to STDOUT, separated by blanks
 *------------------------------------------------------------------------
 */
COMMAND	x_echo(nargs, args)
int	nargs;
char	**args[];
{
	char	str[80];
	int	i;
	for (nargs--, args++ ; nargs > 0 ; nargs--, args++)
        xinu_fprintf(STDOUT,(nargs>1)?"%s ":"%s\n", *args);
	return(OK);
}
