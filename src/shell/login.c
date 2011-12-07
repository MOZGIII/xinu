/* login.c - login */

#include <conf.h>
#include <kernel.h>
#include <shell.h>
#include <sleep.h>
#include <network.h>
#include <mem.h>
#include <signal.h>
#include <sys/time.h>
#include <ncurses.h>
#define BUFLEN  128

LOCAL   char    *shargs[] = {SHNAME, "startup"};

struct  loginfo Log;                         /* login information */

/*------------------------------------------------------------------------
 *  login  -  log user onto system
 *------------------------------------------------------------------------
 */
login()
{
    int     len;
    char    *buf;

    Log.lused = FALSE;
    Log.luser[0] = NULLCH;
    buf = getmem((word) BUFLEN);
	while (TRUE) {
        xinu_printf("\nLinXINU 1.0 - The magic of Xinu\n\nlogin: ");
        while (TRUE) {
            len=xinu_read(STDIN,buf,BUFLEN); 
            if ((len==1) || strncmp(buf,"dc\n",len)) {
                xinu_printf("\nSorry, you are not authorized to use Xinu.");
                xdone();
            }
            break;
        }
		if (len == EOF) {
            xinu_read(STDIN, buf, 0);
            Log.lused = FALSE;
			continue;
		}
        buf[len-1] = NULLCH;
        strncpy(Log.luser, buf, SHNAMLEN-1);
        Log.luser[SHNAMLEN-1] = NULLCH;
        Log.ldev = getpdev(xinu_getpid(),STDIN);
        gettime(&Log.llogin);
        Log.llast = Log.llogin;
        Log.lused = TRUE;
        mark(Log.lmark);
        freemem(buf, (word) BUFLEN);
	clear();
        shell(2,shargs);  /* use 1 for no window, 2 for windows*/
        return OK;
	}
}
