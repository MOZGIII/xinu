/* xinu.c --  PC Xinu support */

#include <tclInt.h>
#include <fcntl.h>
#include <msdos.h>
#include <sleep.h>

/*
 *--------------------------------------------------------------
 *
 * Tcl_TildeSubst --
 *
 *	Given a name starting with a tilde, produce a name where
 *	the tilde and following characters have been replaced by
 *	the home directory location for the named user.
 *
 * Results:
 *	The result is a pointer to a static string containing
 *	the new name.  This name will only persist until the next
 *	call to Tcl_TildeSubst;  save it if you care about it for
 *	the long term.  If there was an error in processing the
 *	tilde, then an error message is left in interp->result
 *	and the return value is NULL.
 *
 * Side effects:
 *	None that the caller needs to worry about.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_TildeSubst(interp, name)
	Tcl_Interp *interp;		/* Interpreter in which to store error
				 * message (if necessary). */
	char *name;			/* File name, which may begin with "~/"
				 * (to indicate current user's home directory)
				 * or "~<user>/" (to indicate any user's (not supported!!!)
				 * home directory). */
{
#define STATIC_BUF_SIZE 50
	static char staticBuf[STATIC_BUF_SIZE];
	static int curSize = STATIC_BUF_SIZE;
	static char *curBuf = staticBuf;
	char *dir;
	int length;
	register char *p;

	if (name[0] != '~') {
	return name;
	}

	/*
	 * First, find the directory name corresponding to the tilde entry.
	 */

    /* ~<user>/ not supported by DOS filesystem */

    dir = "C:/XINU79/PCXNET/MAIN";    /* make this the HOME directory */
	if (dir == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, "couldn't find HOME environment ",
			"variable to expand \"", name, "\"", (char *) NULL);
        return (char *) NULL;
	}
	p = name+1;

	/*
	 * Grow the buffer if necessary to make enough space for the
	 * full file name.
	 */

	length = xinu_strlen(dir) + xinu_strlen(p);
	if (length >= curSize) {
        if (curBuf != staticBuf) {
            ckfree(curBuf);
        }
        curSize = length + 1;
        curBuf = (char *) ckalloc((unsigned) curSize);
	}

	/*
	 * Finally, concatenate the directory name with the remainder
	 * of the path in the buffer.
	 */

	strcpy(curBuf, dir);
	strcat(curBuf, p);
	return curBuf;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_OpenCmd --
 *
 *	This procedure is invoked to process the "open" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_OpenCmd(notUsed, interp, argc, argv)
	ClientData notUsed;			/* Not used. */
	Tcl_Interp *interp;			/* Current interpreter. */
	int argc;				/* Number of arguments. */
	char **argv;			/* Argument strings. */
{
    Interp *iPtr = (Interp *) interp;
    int pipeline, fd;
    char *access;
    register OpenFile *filePtr;

    if (argc == 2) {
        access = "w";     
	} else if (argc == 3) {
                access = argv[2];
            } else {
                Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
                " filename ?access?\"", (char *) NULL);
                return TCL_ERROR;
            }

	filePtr = (OpenFile *) ckalloc(sizeof(OpenFile));
	filePtr->f = NULL;
	filePtr->f2 = NULL;
	filePtr->readable = 0;
	filePtr->writable = 0;
	filePtr->numPids = 0;
	filePtr->pidPtr = NULL;
	filePtr->errorId = -1;

	/*
	 * Verify the requested form of access.
	 */

	pipeline = 0;
	if (argv[1][0] == '|') {
        pipeline = 1;
	}
	switch (access[0]) {
        case 'w':   filePtr->writable = 1;
        case 'r':   filePtr->readable = 1;
                    break;
        case 'a':   filePtr->writable = 1;
                    break;
        default:    badAccess:
                    Tcl_AppendResult(interp, "illegal access mode \"", access,
                    "\"", (char *) NULL);
                    goto error;
	}
	if (access[1] == '+') {
        filePtr->readable = filePtr->writable = 1;
        if (access[2] != 0) {
            goto badAccess;
        }
    } else  if (access[1] != 0) {
                goto badAccess;
            }

	/*
	 * Open the file or create a process pipeline.
	 */

	if (!pipeline) {
        char *fileName = (argv[1]);

        if (fileName[0] == '~') {
            fileName = Tcl_TildeSubst(interp, fileName);
            if (fileName == NULL) {
                goto error;
            }
        }
        filePtr->f = (char*)xinu_open(DOS, fileName, access);
        if ((int)filePtr->f == SYSERR) {
            Tcl_AppendResult(interp, "couldn't open \"", argv[1],
                            "\": ", Tcl_UnixError(interp), (char *) NULL);
            goto error;
        }
    } else { 
        if ((int)(filePtr->f = (char*)xinu_open(PIPE, "pipe", access)) == SYSERR) {
            interp->result="couldn't open pipe";
            goto error;
        } else {
            int nargs = 3;
            char command[200];
            char *args[3] = {"shell","-c"};
            strcpy(command,&argv[1][1]);
            strcat(command,"<pipe &");
            args[2] = command;
            if (shell(nargs,args) == SYSERR) {
                Tcl_AppendResult(interp, "Xinu Shell SYSERR", NULLPTR );
                return(TCL_ERROR);
            }
        }       
	}

	/*
	 * Enter this new OpenFile structure in the table for the
	 * interpreter.  May have to expand the table to do this.
	 */

    fd = (int) filePtr->f;   /* fd in Xinu are integers */
	TclMakeFileTable(iPtr, fd);
    if (iPtr->filePtrArray[fd] != NULL) {                
        panic("Tcl_OpenCmd found file already open");
	}
	iPtr->filePtrArray[fd] = filePtr;
	sprintf(interp->result, "file%d", fd);
	return TCL_OK;

	error:
	if (filePtr->f != NULL) {
        close(filePtr->f);
	}
	if (filePtr->f2 != NULL) {
        close(filePtr->f2);
	}
	if (filePtr->numPids > 0) {
        Tcl_DetachPids(filePtr->numPids, filePtr->pidPtr);
        ckfree((char *) filePtr->pidPtr);
    }
    if (filePtr->errorId != -1) {
        close(filePtr->errorId);
    }
	ckfree((char *) filePtr);
	return TCL_ERROR;
}


/*
 *----------------------------------------------------------------------
 *
 * Tcl_TimeCmd --
 *
 *	This procedure is invoked to process the "time" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_TimeCmd(dummy, interp, argc, argv)
	ClientData dummy;			/* Not used. */
	Tcl_Interp *interp;			/* Current interpreter. */
	int argc;				/* Number of arguments. */
	char **argv;			/* Argument strings. */
{
	int count, i, result;
	long start, stop;

	if (argc == 2) {
	count = 1;
	} else if (argc == 3) {
	if (Tcl_GetInt(interp, argv[2], &count) != TCL_OK) {
		return TCL_ERROR;
	}
	} else {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" command ?count?\"", (char *) NULL);
	return TCL_ERROR;
	}
    start = tod;                            /* PC ticks */
	for (i = count ; i > 0; i--) {
    result = Tcl_Eval(interp, argv[1], 0, (char **) NULLPTR);
	if (result != TCL_OK) {
		if (result == TCL_ERROR) {
		char msg[60];
		sprintf(msg, "\n    (\"time\" body line %d)",
			interp->errorLine);
		Tcl_AddErrorInfo(interp, msg);
		}
		return result;
	}
	}
    stop = tod;
	Tcl_ResetResult(interp);
    sprintf(interp->result, "%D ticks per iteration", (stop-start)/count);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * CleanupChildren --
 *
 *	This is a utility procedure used to wait for child processes
 *	to exit, record information about abnormal exits, and then
 *	collect any stderr output generated by them.
 *
 * Results:
 *	The return value is a standard Tcl result.  If anything at
 *	weird happened with the child processes, TCL_ERROR is returned
 *	and a message is left in interp->result.
 *
 * Side effects:
 *	If the last character of interp->result is a newline, then it
 *	is removed.  File errorId gets closed, and pidPtr is freed
 *	back to the storage allocator.
 *
 *----------------------------------------------------------------------
 */

int
CleanupChildren(interp, numPids, pidPtr, errorId)
	Tcl_Interp *interp;		/* Used for error messages. */
	int numPids;		/* Number of entries in pidPtr array. */
	int *pidPtr;		/* Array of process ids of children. */
	int errorId;		/* File descriptor index for file containing
				 * stderr output from pipeline.  -1 means
				 * there isn't any stderr output. */
{
 /* This routine was formerly used by the Tcl_Exec() and Tcl_Open()
  * calls, which have no use for it, since msdos supports no
  * independent child processes
  */
  return(TCL_OK);
}

/*************************************************************/



/* msdos version extract of unixutil.c */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_Fork --
 *
 *	Create a new process using the vfork system call, and keep
 *	track of it for "safe" waiting with Tcl_WaitPids.
 *
 * Results:
 *	The return value is the value returned by the vfork system
 *	call (0 means child, > 0 means parent (value is child id),
 *	< 0 means error).
 *
 * Side effects:
 *	A new process is created, and an entry is added to an internal
 *	table of child processes if the process is created successfully.
 *
 *----------------------------------------------------------------------
 */

/*
 * This function is removed since it is only used by TCL_Exec(),
 * which uses the msdos system() call to make this kind of action!
 *
 */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_WaitPids --
 *
 *	This procedure is used to wait for one or more processes created
 *	by Tcl_Fork to exit or suspend.  It records information about
 *	all processes that exit or suspend, even those not waited for,
 *	so that later waits for them will be able to get the status
 *	information.
 *
 * Results:
 *	-1 is returned if there is an error in the wait kernel call.
 *	Otherwise the pid of an exited/suspended process from *pidPtr
 *	is returned and *statusPtr is set to the status value returned
 *	by the wait kernel call.
 *
 * Side effects:
 *	Doesn't return until one of the pids at *pidPtr exits or suspends.
 *
 *----------------------------------------------------------------------
 */

/*
 * This function is removed since it is only used by CleanupChildren(),
 * which is only a dummy function!
 *
 */



/*
 *----------------------------------------------------------------------
 *
 * Tcl_DetachPids --
 *
 *	This procedure is called to indicate that one or more child
 *	processes have been placed in background and are no longer
 *	cared about.  They should be ignored in future calls to
 *	Tcl_WaitPids.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_DetachPids(numPids, pidPtr)
	int numPids;		/* Number of pids to detach:  gives size
				 * of array pointed to by pidPtr. */
	int *pidPtr;		/* Array of pids to detach:  must have
				 * been created by Tcl_Fork. */
{
	/*
	 * This is a dummy function to fulfill consistency.
	 * Background processing is not allowed under MSDOS.
	 *
	 */
}




/*
 *----------------------------------------------------------------------
 *
 * Tcl_ExecCmd --
 *
 *	This procedure is invoked to process the "exec" Tcl command.
 *  Exec passes ALL it's arguments to the Xinu Shell for processing.
 *  The Xinu Shell returns SYSERR if it encountered an internal error
 *  or OK if the Shell executed correctly. If the commands passed to
 *  it were in error, it prints it's error message to STDERR and then
 *  returns OK.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *  Depends on the Xinu Shell commands issued
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ExecCmd(dummy, interp, argc, argv)
	ClientData dummy;			/* Not used. */
	Tcl_Interp *interp;			/* Current interpreter. */
    int argc;                   /* Number of arguments. */
    char **argv;                /* Argument strings. */
{
    int     i, dev;
    char command[200]="";
#define BUFSIZE     1000
    char buf[BUFSIZE+1];
    int nargs = 3;
    char *args[3] = {"shell", "-c"};

    for (i=1; i<argc; i++) {    /* build a command string for Xinu Shell */    
        strcat(command, argv[i]);
        strcat(command," ");    /* separate with space */
    }
    strcat(command,">pipe &");  /* redirect STDOUT to pipe */
    args[2] = command;
    if (shell(nargs,args) == SYSERR) {
        Tcl_AppendResult(interp, "Xinu shell error", NULLPTR );
        return(TCL_ERROR);
    }
    dev = xinu_open(NAMESPACE, "pipe", "ro");
    if (dev < 0) {
        xinu_printf("TclExec: couldn't open pipe\n");
        return TCL_OK;
    } 
    while (TRUE) {
        i=read(dev, buf, BUFSIZE);
        if (i <= 0) break;
        buf[i] = 0;
        Tcl_AppendResult(interp, buf, NULLPTR);
    }
    close(dev);
    
    /*
     * If the last character of interp->result is a newline, then remove
     * the newline character (the newline would just confuse things).
     * Special hack: must replace the old terminating null character
     * as a signal to Tcl_AppendResult et al. that we've mucked with
     * the string.
     */

    i = xinu_strlen(interp->result);
    if ((i > 0) && (interp->result[i-1] == '\n')) {
    interp->result[i-1] = '\0';
    interp->result[i] = 'x';
    }
        
    return TCL_OK;    
}

/*
 * This function fakes a wrong uid output for Tcl_FileCmd().
 * For future versions is it easier to port.
 *
 */

int geteuid()
{
	return(0);
}
