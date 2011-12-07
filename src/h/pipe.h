/* pipe.h */

#define POPEN   1
#define PCLOSED 0
#ifndef Npipe
#define Npipe   4               /* number of pipes */
#endif

struct  pipe    {
        int     dvnum;          /* device number of this pipe */
        int     pstate;         /* pipe POPEN or PCLOSED */
        int     wpid;           /* pid of writer */
        int     rpid;           /* pid of reader */
        int     ssem;           /* space available semaphore */
        int     asem;           /* data  available semaphore  */
        int     data;           /* pipe data */
};
extern struct pipe pipe[];
