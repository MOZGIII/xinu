/* addarg.c - addarg */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <dos.h>

typedef int (*fptr)();			/* function pointer type	*/

/*------------------------------------------------------------------------
 *  addarg  -  copy arguments to area reserved on process stack
 *------------------------------------------------------------------------
 */
addarg(pid, nargs, args, len)
int	pid;				/* process to receive arguments	*/
int	nargs;				/* number of arguments to copy	*/
char    *args[];        /* the arguments                */
int	len;				/* size of arg. area (in bytes)	*/
{
	struct	pentry	*pptr;
	char	**toarg;
	char	**fromarg;
	char	*to;

	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate != PRSUSP)
		return(SYSERR);

	/* skip over DI, SI, FLAGS, BP, procaddr, INITRET, and nargs	*/
	toarg = (char **) (pptr->pregs + 5*sizeof(int) + 2*sizeof(fptr));

	/* set up the pointer to the array of string pointers */
	*toarg++ = (char *)(toarg + 1);

	/* skip over the nargs string pointers and the final NULL	*/
	to = (char *)(toarg + (nargs+1));
	
    for (fromarg = args ; nargs > 0 ; nargs--) {
		(*toarg++) = to;
		xinu_strcpy(to, *fromarg++);
		to += xinu_strlen(to) + 1;
	}
    *toarg = NULLPTR;
	return(OK);
}
