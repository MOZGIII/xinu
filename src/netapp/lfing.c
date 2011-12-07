/* lfing.c - lfing */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <shell.h>

extern struct loginfo Log;

/*------------------------------------------------------------------------
 *  lfing  -  print local FINGER info on the given descriptor
 *------------------------------------------------------------------------
 */
int lfing(nam, output)
char	*nam;
int	output;
{
    char *header = "Login       Time\n";
    char buf[40];
    char obuf[128];
    
	if (nam == NULL || *nam == '\0' || *nam == '\r') {
		/* short form */
		/* not really here yet... fake it */

        xinu_write(output, header, xinu_strlen(header));
        ascdate(Log.llogin, buf);
        xinu_sprintf(obuf, "%s       %s\n", Log.luser, buf);
        xinu_write(output, obuf, xinu_strlen(obuf));  /* more efficient */
/*        xinu_fprintf(output, "%s       %s\n", Log.luser, buf);   */
		return OK;
	}
write(output, "Not implemented\n", 16);
	/* not implemented (yet) */
	return SYSERR;
}
