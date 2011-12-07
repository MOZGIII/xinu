/*  x_tcl.c -- Xinu TCL shell */

#include <tcl.h>
#include <io.h>
#include <shell.h>

int cmdEcho();

int quitFlag;                       /* this is nasty, fix it */
LSYSCALL mem();

/*------------------------------------------------------------------------
 *  x_tcl  -  TCL Version 6.7 shell
 *------------------------------------------------------------------------
 */
COMMAND x_tcl(argc, argv)
int     argc;
char    *argv[];
{
    Tcl_Interp *interp;
    Tcl_CmdBuf buffer;
    char buf[1000];
#define LENGTH 256    
    char line[LENGTH], *cmd, *args, *fileName;
    int len, dev, ch, result=0, gotPartial;
    int tcl = TRUE;
    int prompt;
    int i;

    interp = Tcl_CreateInterp();
    Tcl_CreateCommand(interp, "echo", cmdEcho, (ClientData) "echo",
	    (Tcl_CmdDeleteProc *) NULL);
    buffer = Tcl_CreateCmdBuf();
    dev = STDIN;
    prompt = TRUE;
    fileName = NULLPTR;
    if ((argc>1) && (argv[1][0] != '-')) {
        fileName = argv[1];
        prompt = FALSE;
        argc--;
        argv++;
    }   
    
    /*
     * Make command-line arguments available in the Tcl variables "argc"
     * and "argv".  
     */

    args = Tcl_Merge(argc-1, argv+1);
    Tcl_SetVar(interp, "argv", args, TCL_GLOBAL_ONLY);
    ckfree(args); 
    xinu_sprintf(buffer, "%d", argc-1);
    Tcl_SetVar(interp, "argc", (char*)buffer, TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp, "argv0", (fileName != NULL) ? fileName : argv[0],
	    TCL_GLOBAL_ONLY);
    if (fileName != NULL) {
        result = Tcl_EvalFile(interp, fileName);
        if (*interp->result != 0)
            xinu_fprintf(STDOUT, "%s\n", interp->result);
        goto done;
    }
    quitFlag = 0;
    gotPartial = 0;
    while (tcl) {
        if (!gotPartial) {
            if (prompt)
                xinu_fprintf(STDOUT,"tcl> ");
        }
        if ((len=read(dev,line, LENGTH)) < 0) {
            if (!gotPartial) {
                goto done;
            }
            line[0] = 0;
        }
        line[len] = '\0';        
        cmd = Tcl_AssembleCmd(buffer, line);
        if (cmd == NULL) {
            gotPartial = 1;
            continue;
        }

        gotPartial = 0;
        result = Tcl_Eval(interp, cmd, 0, (char **) NULL);
        if (result == TCL_OK) {
            if (*interp->result != 0) {
                xinu_fprintf(STDOUT, "%s\n", interp->result);
            }
            if (quitFlag) {
                goto done;
            }
        } else {
            if (result == TCL_ERROR) {
                xinu_fprintf(STDERR, "Error");
            } else {
                xinu_fprintf(STDERR, "Error %d", result);
            }
            if (*interp->result != 0) {
                xinu_fprintf(STDERR, ": %s\n", interp->result);
             } else {
                 xinu_fprintf(STDERR, "\n");
             }
        }
    }
done:   
        Tcl_DeleteInterp(interp); 
        Tcl_DeleteCmdBuf(buffer);
        return OK;        
}
/*------------------------------------------------------------------------
 *  cmdEcho  -  (tcl echo command) echo arguments to STDOUT
 *------------------------------------------------------------------------
 */
int
cmdEcho(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char *argv[];
{
    int i;

    for (i = 1; ; i++) {
        if (argv[i] == NULLPTR) {
            if (i != argc) {
                echoError:
                xinu_sprintf(interp->result,
                    "argument list not NULL-terminated in \"%s\" command",
                    argv[0]);
            }
	    break;
        }
        if (i >= argc) {
            goto echoError;
        }
        xinu_fprintf(STDOUT, "%s", argv[i]);
        if (i < (argc-1)) {
            xinu_fprintf(STDOUT," ");
        }
    }
    xinu_fprintf(STDOUT,"\n");
    return TCL_OK;
}
