/* naminit.c - naminit */

#include <conf.h>
#include <kernel.h>
#include <name.h>

struct	nam	Nam;

/*------------------------------------------------------------------------
 *  naminit  -  initialize the syntactic namespace pseudo-device
 *------------------------------------------------------------------------
 */
naminit(devptr, flag)
struct devsw *devptr;
int flag;
{
    if (!flag) return OK;
    
	Nam.nnames = 0;

	/* Xinu namespace definition.  
	 * Note that ds0 (drive 0) is the default device.  The default
	 * device is *NOT* the default disk to prevent a potential
	 * circular definition.
	 */

    mount(NULLSTR,      DOS,        NULLSTR,    RLOCAL);
	mount("/dev/",		NAMESPACE,	NULLSTR,	RLOCAL);
	mount("tty",		NAMESPACE,	NULLSTR,	RLOCAL);
	mount("console",	CONSOLE,	NULLSTR,	RLOCAL);
	mount("null",		NULLDEV,	NULLSTR,	RLOCAL);
#ifdef Ndsk
    mount("ds0:",       DS0,        NULLSTR,    RLOCAL);
#endif
#ifdef Nsl
    mount("sl0:",       SL0,        NULLSTR,    RLOCAL);
#endif
    mount("pipe",       PIPE,       "",         RLOCAL);
    mount("a:",         DOS,        "a:",       RLOCAL);
    mount("b:",         DOS,        "b:",       RLOCAL);
    mount("c:",         DOS,        "c:",       RLOCAL);
    mount("d:",         DOS,        "d:",       RLOCAL);
    mount("e:",         DOS,        "e:",       RLOCAL);
    return OK;
}
