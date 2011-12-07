/* x_help.c - x_help */

#include <conf.h>
#include <kernel.h>
#include <shell.h>
#include <cmd.h>

#define COLUMNS 8           /* number of columns to print   */

/*------------------------------------------------------------------------
 *  x_help  --  (command help) print possible command names for user
 *------------------------------------------------------------------------
 */
COMMAND	x_help(nargs, args)
int	nargs;
char	*args[];
{
    int inc;                    /* command names per column     */
    int i;                      /* move through printed rows    */
    int j;                      /* move across printed columns  */
    extern int ncmds;           /* number of commands (in cmd.c)*/
    char    name[80]="./help/";

    if ( nargs > 2 ) {
        xinu_fprintf(STDERR,"Usage: help [command]\n");
		return(SYSERR);
	}
    if (nargs == 1) {
        if ( (inc=(ncmds+COLUMNS-1)/COLUMNS) <= 0)
            inc = 1;
        xinu_fprintf(STDOUT,"\n  LinXinu Version %s      Usage: help [command]\n\n",VERSION);
        for(i=0 ; i<inc && i<ncmds ; i++) {
            xinu_fprintf(STDOUT,"  ");
            for (j=i ; j<ncmds ; j+=inc)
                xinu_fprintf(STDOUT,"%-9s",cmds[j].cmdnam);
            xinu_fprintf(STDOUT,"\n");
        }
        return(OK);
    }
    strcat(name,args[1]);
    strcat(name,".doc");
    args[1]=name;
    x_more(nargs,args);
    return(OK);
}
