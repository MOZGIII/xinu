/* pipalloc.c - pipalloc */

#include <conf.h>
#include <kernel.h>
#include <pipe.h>

/*------------------------------------------------------------------------
 *  pipalloc  -  allocate a pipe psuedo device; return its descriptor
 *------------------------------------------------------------------------
 */
int pipalloc(mode)
char    *mode;
{
    struct  pipe   *pptr;
	int		i;

    if (!strcmp(mode, "w")) {        /* allocate a new pipe */
        for (i=0 ; i<Npip ; ++i) {
            pptr = &pipe[i];
            if (pptr->pstate == PCLOSED) {
                pptr->pstate = POPEN;
                return i;
            }
        }
    }
    if (!strcmp(mode, "ro")) {       /* allocate the last pipe opened */
        for (i=Npip-1 ; i>=0 ; --i) {           /* search backwards */
            pptr = &pipe[i];
            if (pptr->pstate == POPEN) {
                if (i == Npip-1) return SYSERR;  /* no pipes left */
                return i;
            }
        }
    }  
    return SYSERR;                              /* no pipe open */
}
