/* cmd.h */

/* Declarations for all commands known by the shell */

struct  cmdent  {           /* entry in command table        */
    char    *cmdnam;        /* name of command               */
    Bool    cbuiltin;       /* Is this a builtin command ?   */
    int     cmdstk;         /* command stack size */
    int     cmdpri;         /* command priority */
    int     (*cproc)();     /* procedure that implements cmd */
};

extern	struct	cmdent	cmds[];
extern  int             ncmds;
