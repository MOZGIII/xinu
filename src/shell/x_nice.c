/* x_nice.c - x_nice */

#include <conf.h>
#include <kernel.h>
#include <shell.h>
#include <cmd.h>

/*----------------------------------------------------------------------------
 *  x_nice  --  (command nice) run a program with modified scheduling priority
 *----------------------------------------------------------------------------
 */
COMMAND	x_nice(nargs, args)
int nargs;
char *args[];
{
  int     len, i;
  int     com;
  int     child;
  int     priority;

  if (nargs < 3 ) {
    xinu_fprintf(STDERR,"nice: priority command [arguments]\n");
    return(SYSERR);
  }

  priority = atoi(args[1]); // or default to SHCMDPRI

  /* Priority must be positive */

  if (priority <= 0) {
    xinu_fprintf(STDERR,"nice: priority must be greater than zero\n");
    return(SYSERR);
  }

  /* Look up command in table */

  for (com = 0 ; com < ncmds ; com++)
    if (strcmp(cmds[com].cmdnam, args[2])==0)
      break;

  /* Only built-in commands allowed */

  if (com >= ncmds) {
    xinu_fprintf(STDERR,"nice: commang not found\n");
    return(SYSERR);
  }

  /* Handle built-in commands with a procedure call */

  if (cmds[com].cbuiltin) {
    xinu_fprintf(STDERR,"nice: can not be used with built in commands\n");
    return(SYSERR);
  }

  len = 0;

  for (i = 2; i < nargs; i++) {
    len += xinu_strlen(args[i]) + 1;
  }

  /* Compute space needed for string arg ptrs (in bytes) */
  len += (nargs-2+1)*sizeof(char *) + sizeof(char **);
  if (isodd(len))
     ++len;

  /* Create process to execute conventional command */

  if ((child=create(cmds[com].cproc, cmds[com].cmdstk+len,
      priority, args[2], 1+(len/sizeof(int)), nargs-2)) == SYSERR) {
      xinu_fprintf(STDERR,"nice: cannot create \"%s\"\n", args[2]);
      return(SYSERR);
  }
  addarg(child, nargs-2, args+2, len);
  //resume(child);

  setnok(xinu_getpid(), child);
  recvclr();
  resume(child);

  return OK;
}
