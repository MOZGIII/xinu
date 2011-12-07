/* shell.c - shell */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <io.h>
#include <shell.h>
#include <cmd.h>
#include <tty.h>
#include <name.h>
#include <mem.h>
LOCAL redirect(int pid, int siodev, char* name, char* mode);
/* Global error messages */
LOCAL   char    fmt[]   = "shell: cannot open %s\n";
LOCAL	char	fmt2[]  = "[%d]\n";

extern long recvclr();

/*------------------------------------------------------------------------
 *  shell  --  Xinu shell with file redirection and background processing
 *------------------------------------------------------------------------
 */
shell(nargs,args)
int nargs;
char *args[];
{
    struct  shvars *shptr;
    int     ncargs, len;
    int     toktype;                    /* Token type from lexan */
    char    *tokptr;                    /* Pointer to token's text */
    char    *innam, *outnam, *errnam;   /* Names for I/O redirection */
    Bool	backgnd;
    int     com, i, j;
    int     child;
    int     pipe;
    int     terminate;
    long    msg;
    
    if ((shptr = (struct shvars*)getmem((word) sizeof(struct shvars))) == (struct shvars*)SYSERR) {
		xinu_fprintf(STDERR,"shell: not enough memory\n");
		return(SYSERR);
	}
    shptr->shlastc = '\n';
    shptr->shnextc = shptr->shend = shptr->shbuf;
    shptr->shprompt[0] = shptr->shprcon[0] = NULLCH;
    if (nargs < 2)
        shptr->shcmddev = STDIN;
    else if (nargs > 2 && strcmp(args[1],"-c") == 0) {
        shptr->shnextc = args[2];
        shptr->shend = args[2] +xinu_strlen(args[2]);
        shptr->shcmddev = BADDEV;
    }
    else { shptr->shcmddev=xinu_fopen(args[1], "ro");
           if (shptr->shcmddev == SYSERR) {
               xinu_fprintf(STDERR, fmt, args[1]);
               freemem((char*)shptr, (word) sizeof(struct shvars));
               return(SYSERR);
           }
        }
    
    if (shptr->shcmddev == STDIN &&
        getpdev(xinu_getpid(), STDIN) == getpdev(xinu_getpid(), STDOUT)) {
            xinu_strcpy(shptr->shprompt, "% ");
            xinu_strcpy(shptr->shprcon, "> ");
        }

    pipe = 0;                               /* we must remember pipe */
    terminate = FALSE;
        
    for (toktype = '\n' ; toktype != EOF ; ) {
        getutim(&Log.llast);
        if (toktype == '\n')
            shgetc(shptr, shptr->shprompt);  /* Read cmd line */
        innam = outnam = errnam = NULL;
        backgnd = FALSE;
        ncargs = 0;
        tokptr = shptr->shstrbuf;
        len = 0;

        /* Parse input */

        while ((toktype=lexan(tokptr,shptr)) != EOF && toktype != '|' &&
               toktype !='\n' && toktype !=';' && toktype != '&') {
                
                switch (toktype) {
                    case '<':           /* I/O redirection */
                    case '>':
                    case '^':
                        for ( ; ; toktype=lexan(tokptr,shptr)) {
                            switch (toktype) {
                                case '<': innam =  tokptr; continue;
                                case '>': outnam = tokptr; continue;
                                case '^': errnam = tokptr; continue;
                                default: break;
                            }
                            break;
                        }
                        if (toktype != '"') {
                            xinu_fprintf(STDERR, "shell: no redirect name \n");
                            break;
                        }
                        tokptr += xinu_strlen(tokptr) + 1;
                        continue; 
                    case ')':           /* Unbalanced right paren */
                        xinu_fprintf(STDERR, "shell: unbalanced \")\"\n");
                        break;
                    case '(':           /* Command to run in subshell */
                        if (ncargs > 0) {
                            xinu_fprintf(STDERR, "shell: misplaced \"(\"\n");
                            break;
                        }
                        shptr->shargs[ncargs++] = SHNAME;
                        shptr->shargs[ncargs++] = "-c";
                        len += sizeof(SHNAME) +3;
                        /* Fall through */
                    default:                /* String added to arg list */
                        shptr->shargs[ncargs++] = tokptr;
                        len    += xinu_strlen(tokptr) + 1;
                        tokptr += xinu_strlen(tokptr) + 1;
                        continue;
                    }
                    ncargs = 0;             /* Error: ignore command line */
                    shptr->shnextc = shptr->shend;
                    if (shptr->shcmddev != STDIN)
                        toktype = EOF;      /* Ignore rest of line */
                    break;
                }
        if (ncargs == 0)
           continue;
        backgnd = (toktype == '&' || toktype == '|');
        shptr->shargs[ncargs] = NULL;

        /* check for #!command */
        
        if ((shptr->shargs[0][0]=='#') && (shptr->shargs[0][1]=='!')) {
            shptr->shargs[0] += 2;                  /* point past the #! */
            len -= 2;
            terminate = TRUE;
            for (i=ncargs, j=1; j < nargs ; i++, j++) {   /* copy args */
                shptr->shargs[i]=shptr->shargs[i-1] +
                                 xinu_strlen(shptr->shargs[i-1]) + 1;
                xinu_strcpy(shptr->shargs[i], args[j]);
                len += xinu_strlen(args[j]) + 1;
                ncargs++;
            }
        } 

        /* Look up command in table */

        for (com = 0 ; com < ncmds ; com++)
            if (strcmp(cmds[com].cmdnam, shptr->shargs[0])==0)
                break;
 
       /* Command could be a file so do: sh command args */

        if (com >= ncmds) {
            for (i=ncargs; i>0; i--)    /* move args down a slot */
                shptr->shargs[i] = shptr->shargs[i-1];
            shptr->shargs[0] = SHNAME;
            len += sizeof(SHNAME); 
            ncargs += 1;
            for (com = 0 ; com < ncmds ; com++)
                if (strcmp(cmds[com].cmdnam, SHNAME)==0)
                    break;
        }
        
        /* Handle built-in commands with a procedure call */

        if (cmds[com].cbuiltin) {
           if (innam!=NULL || outnam!=NULL || errnam!=NULL || backgnd)
              xinu_fprintf(STDERR,"shell: illegal redirection\n");
           else if ((*cmds[com].cproc)(ncargs,shptr->shargs) == SHEXIT)
                   break;
                continue;
        }

        /* compute space needed for string arg ptrs (in bytes) */
        len += (ncargs+1)*sizeof(char *) + sizeof(char **);
            if (isodd(len))
               ++len;
  
      /* create process to execute conventional command */

        if ((child=create(cmds[com].cproc, cmds[com].cmdstk+len, 
            cmds[com].cmdpri, shptr->shargs[0], 1+(len/sizeof(int)), ncargs)) == SYSERR) {
            xinu_fprintf(STDERR,"shell: cannot create \"%s\"\n",
            shptr->shargs[0]);
            continue;
            }
        addarg(child, ncargs, shptr->shargs, len);
        
        /* Handle pipe */
            
        if (pipe) {                         /* previous cmd ended with | */
           pipe = 0;
           if (innam != NULL) {
              xinu_fprintf(STDERR,
                "shell: conflicting STDIN redirection\n");
              continue;
            }
           if (redirect(child, STDIN, "pipe", "ro") == SYSERR) continue;
        } 

        /* Open files and redirect I/O if specified */

        if (redirect(child, STDOUT, outnam, "w") == SYSERR ||
            redirect(child, STDERR, errnam, "w") == SYSERR ||
            redirect(child, STDIN, innam, "ro") == SYSERR)
                continue;
        if (toktype == '|') {               /* cmd ends with | */
           if (outnam != NULL) {
              xinu_fprintf(STDERR,
                "shell: conflicting STDOUT redirection\n");
              continue;
            }
           if (redirect(child, STDOUT, "pipe", "w") == SYSERR) continue;
           pipe=1;
        } 
        
        if ( backgnd ) {
           xinu_fprintf(shptr->shcmddev, fmt2, child);
           resume(child);
        } 
        else {
             setnok(xinu_getpid(), child);
             control(shptr->shcmddev, TCINT);
             recvclr();
             resume(child);
             for (;;) {
                 msg = receive();
                 if ( msg == TMSGINT ) {        /* Ctrl-B pressed */
                     setnok(BADPID, child);
                     xinu_fprintf(STDERR, fmt2, child);
                     break; 
                 }
                 if ( msg == TMSGKILL ) {       /* Ctrl-C pressed */
                     if ( getnok(child) == BADPID) { /* child has no nok */
                         if ( kill(child) == 0 ) /* child is immortal */
                             continue;
                         else
                             break;
                     }
                     break;
                 }
                 if ( msg == child )            /* death of child */
					break;
                 xinu_fprintf(STDERR,
					"shell: message %d ignored\n",msg);
             }
        if (terminate) 
            toktype = EOF;
        }                                           /* end of while */
    }                                               /* end of for */
    xinu_close(shptr->shcmddev);
    freemem((char*)shptr, (word) sizeof(struct shvars));
    return(OK);
}

/*------------------------------------------------------------------------
 *  redirect  --  connect the given standard I/O device to the named one
 *------------------------------------------------------------------------
 */
LOCAL redirect(pid, siodev, name, mode)
int pid, siodev;
char *name, *mode;
{
	int	dev;
    
    if (name == NULL)
		return(OK);
    if ((dev=xinu_open(NAMESPACE, name, mode)) != SYSERR) {
      if (setpdev(pid, siodev, dev) != SYSERR)
           return(OK);
      xinu_close(dev);
    }
 
    xinu_fprintf(STDERR, fmt, name);
    die(pid);                   /* was kill */
    return(SYSERR);
}
