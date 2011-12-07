/* x_pipstat.c - x_pipstat.c */

#include <conf.h>
#include <kernel.h>
#include <pipe.h>

/*------------------------------------------------------------------------
 *  x_pipstat  -  show status of all pipes
 *------------------------------------------------------------------------
 */
COMMAND x_pipstat(nargs, args)
int	nargs;
char	**args[];
{
	int	i;

    for (i=0; i<Npip ; i++)
        xinu_fprintf(STDOUT,"pipe %d dev %d state %d wpid %d rpid %d asem %d \
ssem %d data %d\n",i,pipe[i].dvnum,pipe[i].pstate,pipe[i].wpid,pipe[i].rpid,
scount(pipe[i].asem),scount(pipe[i].ssem),pipe[i].data);
	return(OK);
}
