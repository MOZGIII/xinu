/* shell.h */

#include <mark.h>

/* Declarations for the Xinu shell */

#define SHSTK       8000   /* size of shell stack */
#define SHPRIO      20      /* shell priority      */
#define SHNAME      "sh" /* shell process and command name */

#define	SHEXIT		-5		/* shell exit return		*/
#define	SHNAMLEN	9		/* length of user name + 1	*/
#define SHBUFLEN    512     /* length of general buffer */
#define SHPRLEN     128     /* maximum length of a prompt */
#define SHMAXARG    128     /* maximum no. of args to cmd */
#define SHSTRLEN    1024    /*length of area for strings  */
#define SHCMDSTK    4096    /* size of command proc. stack  */
#define	SHCMDPRI	20		/* command process priority	*/

struct  loginfo {                   /* login information */
        MARKER  lmark;              /* marked if lused valid */
        Bool    lused;              /* TRUE => user logged in */
        char    luser[SHNAMLEN];    /* current user name */
        int     ldev;               /* device login shell is using */
        long    llogin;             /* time user logged in */
        long    llast;              /* time shell last touched */
};

extern struct loginfo Log;
    
struct  shvars  {               /* shell global variables      */
    int     shcmddev;           /* device to get commands from */
    char    shbuf[SHBUFLEN];    /* buffer for command input */
    char    *shnextc;           /* pointer to next char to read */
    char    *shend;             /* pointer to end of cmd input */
    char    shprompt[SHPRLEN];  /* shell's prompt for command  */
    char    shprcon[SHPRLEN];   /* continuation prompt      */
    int     shlastc;            /* last char read from cmd input */
    char    *shargs[SHMAXARG];  /* pointers to command's args */
    char    shstrbuf[SHSTRLEN]; /* holds strings for a command  */
};
