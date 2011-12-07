/*
 * tclUnixAZ.c --
 *
 *	This file contains the top-level command procedures for
 *	commands in the Tcl core that require UNIX facilities
 *	such as files and process execution.  Much of the code
 *	in this file is based on earlier versions contributed
 *	by Karl Lehenbauer, Mark Diekhans and Peter da Silva.
 *
 */

#include <tclInt.h>
#include <fcntl.h>
#include <msdos.h>
#include <mffile.h>
#include <stat.h>
#include <stdio.h>
#include <errno.h>
/*
 * The variable below caches the name of the current working directory
 * in order to avoid repeated calls to getwd.  The string is malloc-ed.
 * NULL means the cache needs to be refreshed.
 */

static char *currentDir =  NULL;

/*
 * Prototypes for local procedures defined in this file:
 */

int				CleanupChildren _ANSI_ARGS_((Tcl_Interp *interp,
				int numPids, int *pidPtr, int errorId));
static char *		GetFileType _ANSI_ARGS_((int mode));
static int		StoreStatData _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, struct stat *statPtr));

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CdCmd --
 *
 *	This procedure is invoked to process the "cd" Tcl command.
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
Tcl_CdCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
	Tcl_Interp *interp;			/* Current interpreter. */
	int argc;				/* Number of arguments. */
	char **argv;			/* Argument strings. */
{
    char *dirName;

    if (argc > 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" dirName\"", (char *) NULL);
	return TCL_ERROR;
    }

    if (argc == 2) {
	dirName = argv[1];
    } else {
	dirName = "~";
    }
    dirName = Tcl_TildeSubst(interp, dirName);
    if (dirName == NULL) {
	return TCL_ERROR;
    }
    if (currentDir != NULL) {
	ckfree(currentDir);
	currentDir = NULL;
    }

	if (chdir(dirName) != 0) {
	Tcl_AppendResult(interp, "couldn't change working directory to \"",
		dirName, "\": ", Tcl_UnixError(interp), (char *) NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CloseCmd --
 *
 *	This procedure is invoked to process the "close" Tcl command.
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
Tcl_CloseCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    OpenFile *filePtr;
    int result = TCL_OK;

    if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
	}
    ((Interp *) interp)->filePtrArray[(int)filePtr->f] = NULL;

	/*
     * First close the file (in the case of a process pipeline, there may
     * be two files, one for the pipe at each end of the pipeline).
     */

    if (filePtr->f2 != NULL) {
	if (fclose(filePtr->f2) == EOF) {
	    Tcl_AppendResult(interp, "error closing \"", argv[1],
		    "\": ", Tcl_UnixError(interp), "\n", (char *) NULL);
	    result = TCL_ERROR;
	}
    }
    if (fclose(filePtr->f) == EOF) {
	Tcl_AppendResult(interp, "error closing \"", argv[1],
		"\": ", Tcl_UnixError(interp), "\n", (char *) NULL);
	result = TCL_ERROR;
    }

    /*
     * If the file was a connection to a pipeline, clean up everything
     * associated with the child processes.
     */

    if (filePtr->numPids > 0) {
	if (CleanupChildren(interp, filePtr->numPids, filePtr->pidPtr,
		filePtr->errorId) != TCL_OK) {
		result = TCL_ERROR;
	}
    }

    ckfree((char *) filePtr);
	return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_EofCmd --
 *
 *	This procedure is invoked to process the "eof" Tcl command.
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
Tcl_EofCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
	OpenFile *filePtr;

	if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId\"", (char *) NULL);
	return TCL_ERROR;
	}
	if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }
    if ((int)filePtr->f > 0) {
	interp->result = "1";
    } else {
	interp->result = "0";
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ExitCmd --
 *
 *	This procedure is invoked to process the "exit" Tcl command.
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

extern int quitFlag;

	/* ARGSUSED */
int
Tcl_ExitCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int value;

    if ((argc != 1) && (argc != 2)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" ?returnCode?\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (argc == 1) {
    quitFlag = TRUE;
    return TCL_OK;
    }
    if (Tcl_GetInt(interp, argv[1], &value) != TCL_OK) {
	return TCL_ERROR;
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_FileCmd --
 *
 *	This procedure is invoked to process the "file" Tcl command.
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
Tcl_FileCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
	Tcl_Interp *interp;			/* Current interpreter. */
    int     argc;               /* Number of arguments. */
    char    **argv;            /* Argument strings. */
{
    char    *p;
    int     length, statOp;
    int     mode = 0;           /* Initialized only to prevent
                                 * compiler warning message. */
	struct stat statBuf;
	char *fileName, c;

	if (argc < 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" option name ?arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    c = argv[1][0];
    length = xinu_strlen(argv[1]);

    /*
     * First handle operations on the file name.
	 */

    fileName = Tcl_TildeSubst(interp, argv[2]);
    if (fileName == NULL) {
	return TCL_ERROR;
    }
    if ((c == 'd') && (strncmp(argv[1], "dirname", length) == 0)) {
	if (argc != 3) {
	    argv[1] = "dirname";
	    not3Args:
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " ", argv[1], " name\"", (char *) NULL);
	    return TCL_ERROR;
	}
	p = strrchr(fileName, F_SEP);
	if (p == NULL) {
	    interp->result = ".";
	} else if (p == fileName) {
		interp->result = F_SEPZ;
	} else {
	    *p = 0;
	    Tcl_SetResult(interp, fileName, TCL_VOLATILE);
		*p = F_SEP;
	}
    return TCL_OK;     /* XINU */
	} else if ((c == 'r') && (strncmp(argv[1], "rootname", length) == 0)
	    && (length >= 2)) {
	char *lastSlash;

	if (argc != 3) {
	    argv[1] = "rootname";
	    goto not3Args;
	}
	p = strrchr(fileName, '.');
	lastSlash = strrchr(fileName, F_SEP);
	if ((p == NULL) || ((lastSlash != NULL) && (lastSlash > p))) {
	    Tcl_SetResult(interp, fileName, TCL_VOLATILE);
	} else {
	    *p = 0;
	    Tcl_SetResult(interp, fileName, TCL_VOLATILE);
	    *p = '.';
	}
	return TCL_OK;
    } else if ((c == 'e') && (strncmp(argv[1], "extension", length) == 0)
	    && (length >= 3)) {
	char *lastSlash;

	if (argc != 3) {
		argv[1] = "extension";
		goto not3Args;
	}
	p = strrchr(fileName, '.');
	lastSlash = strrchr(fileName, F_SEP);
	if ((p != NULL) && ((lastSlash == NULL) || (lastSlash < p))) {
	    Tcl_SetResult(interp, p, TCL_VOLATILE);
	}
	return TCL_OK;
	} else if ((c == 't') && (strncmp(argv[1], "tail", length) == 0)
		&& (length >= 2)) {
	if (argc != 3) {
	    argv[1] = "tail";
	    goto not3Args;
	}
	p = strrchr(fileName, F_SEP);
	if (p != NULL) {
	    Tcl_SetResult(interp, p+1, TCL_VOLATILE);
	} else {
	    Tcl_SetResult(interp, fileName, TCL_VOLATILE);
	}
	return TCL_OK;
	}

	/*
     * Next, handle operations that can be satisfied with the "access"
     * kernel call.
	 */

    if (fileName == NULL) {
	return TCL_ERROR;
    }
    if ((c == 'r') && (strncmp(argv[1], "readable", length) == 0)
		&& (length >= 5)) {
	if (argc != 3) {
	    argv[1] = "readable";
		goto not3Args;
	}
	mode = R_OK;
	checkAccess:
    
	if (access(fileName, mode) == -1) {
	    interp->result = "0";
	} else {
		interp->result = "1";
	}
	return TCL_OK;
    } else if ((c == 'w') && (strncmp(argv[1], "writable", length) == 0)) {
	if (argc != 3) {
	    argv[1] = "writable";
	    goto not3Args;
	}
	mode = W_OK;
	goto checkAccess;
    } else if ((c == 'e') && (strncmp(argv[1], "executable", length) == 0)
	    && (length >= 3)) {
	if (argc != 3) {
	    argv[1] = "executable";
		goto not3Args;
	}
	mode = X_OK;
	goto checkAccess;
    } else if ((c == 'e') && (strncmp(argv[1], "exists", length) == 0)
	    && (length >= 3)) {
	if (argc != 3) {
	    argv[1] = "exists";
	    goto not3Args;
	}
	mode = F_OK;
	goto checkAccess;
    }

	/*
     * Lastly, check stuff that requires the file to be stat-ed.
	 */

	if ((c == 'a') && (strncmp(argv[1], "atime", length) == 0)) {
	if (argc != 3) {
	    argv[1] = "atime";
	    goto not3Args;
	}
	if (stat(fileName, &statBuf) == -1) {
	    goto badStat;
	}
	sprintf(interp->result, "%ld", statBuf.st_atime);
	return TCL_OK;
    } else if ((c == 'i') && (strncmp(argv[1], "isdirectory", length) == 0)
	    && (length >= 3)) {
	if (argc != 3) {
	    argv[1] = "isdirectory";
	    goto not3Args;
	}
	statOp = 2;
	} else if ((c == 'i') && (strncmp(argv[1], "isfile", length) == 0)
	    && (length >= 3)) {
	if (argc != 3) {
	    argv[1] = "isfile";
	    goto not3Args;
	}
	statOp = 1;
    } else if ((c == 'l') && (strncmp(argv[1], "lstat", length) == 0)) {
	if (argc != 4) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" lstat name varName\"", (char *) NULL);
	    return TCL_ERROR;
	}

	if (lstat(fileName, &statBuf) == -1) {
	    Tcl_AppendResult(interp, "couldn't lstat \"", argv[2],
			"\": ", Tcl_UnixError(interp), (char *) NULL);
		return TCL_ERROR;
	}
	return StoreStatData(interp, argv[3], &statBuf);
    } else if ((c == 'm') && (strncmp(argv[1], "mtime", length) == 0)) {
	if (argc != 3) {
	    argv[1] = "mtime";
		goto not3Args;
	}
	if (stat(fileName, &statBuf) == -1) {
	    goto badStat;
	}
	sprintf(interp->result, "%ld", statBuf.st_mtime);
	return TCL_OK;
    } else if ((c == 'o') && (strncmp(argv[1], "owned", length) == 0)) {
	if (argc != 3) {
	    argv[1] = "owned";
	    goto not3Args;
	}
	statOp = 0;
#ifdef S_IFLNK
	/*
     * This option is only included if symbolic links exist on this system
     * (in which case S_IFLNK should be defined).
	 */
    } else if ((c == 'r') && (strncmp(argv[1], "readlink", length) == 0)
	    && (length >= 5)) {
	char linkValue[MAXPATHLEN+1];
	int linkLength;

	if (argc != 3) {
	    argv[1] = "readlink";
	    goto not3Args;
	}
	linkLength = readlink(fileName, linkValue, sizeof(linkValue) - 1);
	if (linkLength == -1) {
	    Tcl_AppendResult(interp, "couldn't readlink \"", argv[2],
		    "\": ", Tcl_UnixError(interp), (char *) NULL);
	    return TCL_ERROR;
	}
	linkValue[linkLength] = 0;
	Tcl_SetResult(interp, linkValue, TCL_VOLATILE);
	return TCL_OK;
#endif
    } else if ((c == 's') && (strncmp(argv[1], "size", length) == 0)
	    && (length >= 2)) {
	if (argc != 3) {
	    argv[1] = "size";
	    goto not3Args;
	}
	if (stat(fileName, &statBuf) == -1) {
	    goto badStat;
	}
	sprintf(interp->result, "%ld", statBuf.st_size);
	return TCL_OK;
    } else if ((c == 's') && (strncmp(argv[1], "stat", length) == 0)
	    && (length >= 2)) {
	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " stat name varName\"", (char *) NULL);
	    return TCL_ERROR;
	}

	if (stat(fileName, &statBuf) == -1) {
	    badStat:
	    Tcl_AppendResult(interp, "couldn't stat \"", argv[2],
			"\": ", Tcl_UnixError(interp), (char *) NULL);
		return TCL_ERROR;
	}
	return StoreStatData(interp, argv[3], &statBuf);
    } else if ((c == 't') && (strncmp(argv[1], "type", length) == 0)
	    && (length >= 2)) {
	if (argc != 3) {
	    argv[1] = "type";
	    goto not3Args;
	}
	if (lstat(fileName, &statBuf) == -1) {
		goto badStat;
	}
	interp->result = GetFileType((int) statBuf.st_mode);
	return TCL_OK;
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\": should be atime, dirname, executable, exists, ",
		"extension, isdirectory, isfile, lstat, mtime, owned, ",
		"readable, ",
#ifdef S_IFLNK
		"readlink, ",
#endif
		"root, size, stat, tail, type, ",
		"or writable",
		(char *) NULL);
	return TCL_ERROR;
    }
    if (stat(fileName, &statBuf) == -1) {
	interp->result = "0";
	return TCL_OK;
    }
    switch (statOp) {
	case 0:
		mode = (geteuid() == statBuf.st_uid);
	    break;
	case 1:
		mode = S_ISREG(statBuf.st_mode);
	    break;
	case 2:
	    mode = S_ISDIR(statBuf.st_mode);
	    break;
    }
    if (mode) {
	interp->result = "1";
    } else {
	interp->result = "0";
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * StoreStatData --
 *
 *	This is a utility procedure that breaks out the fields of a
 *	"stat" structure and stores them in textual form into the
 *	elements of an associative array.
 *
 * Results:
 *	Returns a standard Tcl return value.  If an error occurs then
 *	a message is left in interp->result.
 *
 * Side effects:
 *	Elements of the associative array given by "varName" are modified.
 *
 *----------------------------------------------------------------------
 */

static int
StoreStatData(interp, varName, statPtr)
	Tcl_Interp *interp;			/* Interpreter for error reports. */
	char *varName;			/* Name of associative array variable
					 * in which to store stat results. */
	struct stat *statPtr;		/* Pointer to buffer containing
					 * stat data to store in varName. */
{
    char string[30];

    sprintf(string, "%d", statPtr->st_dev);
	if (Tcl_SetVar2(interp, varName, "dev", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%d", statPtr->st_ino);
    if (Tcl_SetVar2(interp, varName, "ino", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%d", statPtr->st_mode);
    if (Tcl_SetVar2(interp, varName, "mode", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%d", statPtr->st_nlink);
    if (Tcl_SetVar2(interp, varName, "nlink", string, TCL_LEAVE_ERR_MSG)
		== NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%d", statPtr->st_uid);
    if (Tcl_SetVar2(interp, varName, "uid", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%d", statPtr->st_gid);
    if (Tcl_SetVar2(interp, varName, "gid", string, TCL_LEAVE_ERR_MSG)
		== NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%ld", statPtr->st_size);
    if (Tcl_SetVar2(interp, varName, "size", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%ld", statPtr->st_atime);
	if (Tcl_SetVar2(interp, varName, "atime", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%ld", statPtr->st_mtime);
    if (Tcl_SetVar2(interp, varName, "mtime", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%ld", statPtr->st_ctime);
    if (Tcl_SetVar2(interp, varName, "ctime", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    if (Tcl_SetVar2(interp, varName, "type",
		GetFileType((int) statPtr->st_mode), TCL_LEAVE_ERR_MSG) == NULL) {
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * GetFileType --
 *
 *	Given a mode word, returns a string identifying the type of a
 *	file.
 *
 * Results:
 *	A static text string giving the file type from mode.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static char *
GetFileType(mode)
    int mode;
{
    if (S_ISREG(mode)) {
	return "file";
    } else if (S_ISDIR(mode)) {
	return "directory";
    } else if (S_ISCHR(mode)) {
	return "characterSpecial";
    } else if (S_ISBLK(mode)) {
	return "blockSpecial";
	} else if (S_ISFIFO(mode)) {
	return "fifo";
    } else if (S_ISLNK(mode)) {
	return "link";
	} else if (S_ISSOCK(mode)) {
	return "socket";
	}
	return "unknown";
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_FlushCmd --
 *
 *	This procedure is invoked to process the "flush" Tcl command.
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
Tcl_FlushCmd(notUsed, interp, argc, argv)
	ClientData notUsed;			/* Not used. */
	Tcl_Interp *interp;			/* Current interpreter. */
	int argc;				/* Number of arguments. */
	char **argv;			/* Argument strings. */
{
    OpenFile *filePtr;
    FILE *f;

    if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }
	if (!filePtr->writable) {
	Tcl_AppendResult(interp, "\"", argv[1],
		"\" wasn't opened for writing", (char *) NULL);
	return TCL_ERROR;
    }
    f = (FILE*)filePtr->f2;
    if (f == NULL) {
	f = (FILE*)filePtr->f;
    }
	if (fflush(f) == EOF) {
	Tcl_AppendResult(interp, "error flushing \"", argv[1],
		"\": ", Tcl_UnixError(interp), (char *) NULL);
	clearerr(f);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetsCmd --
 *
 *	This procedure is invoked to process the "gets" Tcl command.
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
Tcl_GetsCmd(notUsed, interp, argc, argv)
	ClientData notUsed;			/* Not used. */
	Tcl_Interp *interp;			/* Current interpreter. */
	int argc;				/* Number of arguments. */
	char **argv;			/* Argument strings. */
{
#   define BUF_SIZE 200
	char buffer[BUF_SIZE+1];
    int totalCount, done, flags;
    OpenFile *filePtr;
    register FILE *f;

    if ((argc != 2) && (argc != 3)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId ?varName?\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
	}
    if (!filePtr->readable) {
	Tcl_AppendResult(interp, "\"", argv[1],
		"\" wasn't opened for reading", (char *) NULL);
	return TCL_ERROR;
    }

	/*
     * We can't predict how large a line will be, so read it in
     * pieces, appending to the current result or to a variable.
	 */

    totalCount = 0;
    done = 0;
	flags = 0;
    f = (FILE*)filePtr->f;
    while (!done) {
	register int c, count;
	register char *p;

	for (p = buffer, count = 0; count < BUF_SIZE-1; count++, p++) {
	    c = getc(f);
        if (c == EOF) {
        if ((int)filePtr->f < 0) {
		    Tcl_ResetResult(interp);
		    Tcl_AppendResult(interp, "error reading \"", argv[1],
			    "\": ", Tcl_UnixError(interp), (char *) NULL);
		    return TCL_ERROR;
        } else /* if (filePtr->f == EOF) */ {   /* XINU */
		    if ((totalCount == 0) && (count == 0)) {
			totalCount = -1;
		    }
			done = 1;
			break;
		}
	    }
	    if (c == '\n') {
		done = 1;
		break;
	    }
	    *p = c;
	}
	*p = 0;
	if (argc == 2) {
	    Tcl_AppendResult(interp, buffer, (char *) NULL);
	} else {
	    if (Tcl_SetVar(interp, argv[2], buffer, flags|TCL_LEAVE_ERR_MSG)
		    == NULL) {
		return TCL_ERROR;
	    }
	    flags = TCL_APPEND_VALUE;
	}
	totalCount += count;
    }

    if (argc == 3) {
	sprintf(interp->result, "%d", totalCount);
	}
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_PwdCmd --
 *
 *	This procedure is invoked to process the "pwd" Tcl command.
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
Tcl_PwdCmd(dummy, interp, argc, argv)
	ClientData dummy;			/* Not used. */
	Tcl_Interp *interp;			/* Current interpreter. */
	int argc;				/* Number of arguments. */
	char **argv;			/* Argument strings. */
{
	char buffer[MAXPATHLEN+1],*strn;

	if (argc != 1) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], "\"", (char *) NULL);
	return TCL_ERROR;
	}
	if (currentDir == NULL) {
#if TCL_GETWD
	if (getwd(buffer) == NULL) {
		Tcl_AppendResult(interp, "error getting working directory name: ",
			buffer, (char *) NULL);
		return TCL_ERROR;
	}
#else
	if (getcwd(buffer, MAXPATHLEN) == NULL) {
		if (errno == ERANGE) {
		interp->result = "working directory name is too long";
		} else {
		Tcl_AppendResult(interp,
			"error getting working directory name: ",
			Tcl_UnixError(interp), (char *) NULL);
		}
		return TCL_ERROR;
	}
#endif
	currentDir = (char *) ckalloc((unsigned) (xinu_strlen(buffer) + 1));
	strcpy(currentDir, buffer);
    }
    interp->result = currentDir;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_PutsCmd --
 *
 *	This procedure is invoked to process the "puts" Tcl command.
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
Tcl_PutsCmd(dummy, interp, argc, argv)
	ClientData dummy;			/* Not used. */
	Tcl_Interp *interp;			/* Current interpreter. */
	int argc;				/* Number of arguments. */
	char **argv;			/* Argument strings. */
{
    OpenFile *filePtr;
    FILE *f;
    int i, j, fd, newline;
    char *fileId;

    i = 1;
    newline = 1;
    if ((argc >= 2) && (strcmp(argv[1], "-nonewline") == 0)) {
	newline = 0;
	i++;
    }
    if ((i < (argc-3)) || (i >= argc)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		"\" ?-nonewline? ?fileId? string", (char *) NULL);
	return TCL_ERROR;
    }

	/*
	 * The code below provides backwards compatibility with an old
	 * form of the command that is no longer recommended or documented.
	 */

    if (i == (argc-3)) {
	if (strncmp(argv[i+2], "nonewline", xinu_strlen(argv[i+2])) != 0) {
	    Tcl_AppendResult(interp, "bad argument \"", argv[i+2],
		    "\": should be \"nonewline\"", (char *) NULL);
	    return TCL_ERROR;
	}
	newline = 0;
    }
	if (i == (argc-1)) {
    fileId = "STDOUT";
    } else {
	fileId = argv[i];
	i++;
	}

    if (TclGetOpenFile(interp, fileId, &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }
    if (!filePtr->writable) {
	Tcl_AppendResult(interp, "\"", fileId,
		"\" wasn't opened for writing", (char *) NULL);
	return TCL_ERROR;
	}
    f = (FILE*)filePtr->f2;
    if (f == NULL) {
    fd = (int) filePtr->f;
    }
    j = xinu_fprintf(fd, "%s\n",argv[i]);
   if (newline) {
        putc(fd,(int)'\n');
	}
    if (j < 0) {            /* !! check for success of previous writes !! */
        Tcl_AppendResult(interp, "error writing \"", fileId,
            "\": ", Tcl_UnixError(interp), (char *) NULL);
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ReadCmd --
 *
 *	This procedure is invoked to process the "read" Tcl command.
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
Tcl_ReadCmd(dummy, interp, argc, argv)
	ClientData dummy;			/* Not used. */
	Tcl_Interp *interp;			/* Current interpreter. */
	int argc;				/* Number of arguments. */
	char **argv;			/* Argument strings. */
{
	OpenFile *filePtr;
    int bytesLeft, bytesRead, count;
#define READ_BUF_SIZE 4096
    char buffer[READ_BUF_SIZE+1];
    int newline, i;

    if ((argc != 2) && (argc != 3)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId ?numBytes?\" or \"", argv[0],
		" ?-nonewline? fileId\"", (char *) NULL);
	return TCL_ERROR;
    }
	i = 1;
	newline = 1;
    if ((argc == 3) && (strcmp(argv[1], "-nonewline") == 0)) {
	newline = 0;
	i++;
    }

    if (TclGetOpenFile(interp, argv[i], &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }
    if (!filePtr->readable) {
	Tcl_AppendResult(interp, "\"", argv[i],
		"\" wasn't opened for reading", (char *) NULL);
	return TCL_ERROR;
	}

	/*
     * Compute how many bytes to read, and see whether the final
     * newline should be dropped.
	 */

	if ((argc >= (i + 2)) && isdigit(argv[i+1][0])) {
	if (Tcl_GetInt(interp, argv[i+1], &bytesLeft) != TCL_OK) {
		return TCL_ERROR;
	}
    } else {
	bytesLeft = 1<<14;

	/*
	 * The code below provides backward compatibility for an
	 * archaic earlier version of this command.
	 */

	if (argc >= (i + 2)) {
		if (strncmp(argv[i+1], "nonewline", xinu_strlen(argv[i+1])) == 0) {
		newline = 0;
	    } else {
		Tcl_AppendResult(interp, "bad argument \"", argv[i+1],
			"\": should be \"nonewline\"", (char *) NULL);
		return TCL_ERROR;
	    }
	}
    }

	/*
     * Read the file in one or more chunks.
	 */

    bytesRead = 0;
	while (bytesLeft > 0) {
	count = READ_BUF_SIZE;
	if (bytesLeft < READ_BUF_SIZE) {
	    count = bytesLeft;
	}
    count = read(filePtr->f, buffer, count);
    if ((int)filePtr->f < 0) {
		Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, "error reading \"", argv[i],
			"\": ", Tcl_UnixError(interp), (char *) NULL);
	    clearerr(filePtr->f);
	    return TCL_ERROR;
	}
	if (count == 0) {
	    break;
	}
	buffer[count] = 0;
	Tcl_AppendResult(interp, buffer, (char *) NULL);
	bytesLeft -= count;
	bytesRead += count;
    }
	if ((newline == 0) && (bytesRead > 0)
	    && (interp->result[bytesRead-1] == '\n')) {
	interp->result[bytesRead-1] = 0;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SeekCmd --
 *
 *	This procedure is invoked to process the "seek" Tcl command.
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
Tcl_SeekCmd(notUsed, interp, argc, argv)
	ClientData notUsed;			/* Not used. */
	Tcl_Interp *interp;			/* Current interpreter. */
	int argc;				/* Number of arguments. */
	char **argv;			/* Argument strings. */
{
    OpenFile *filePtr;
    int offset, mode;

	if ((argc != 3) && (argc != 4)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId offset ?origin?\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[2], &offset) != TCL_OK) {
	return TCL_ERROR;
    }
    mode = SEEK_SET;
    if (argc == 4) {
	int length;
	char c;

	length = xinu_strlen(argv[3]);
	c = argv[3][0];
	if ((c == 's') && (strncmp(argv[3], "start", length) == 0)) {
		mode = SEEK_SET;
	} else if ((c == 'c') && (strncmp(argv[3], "current", length) == 0)) {
	    mode = SEEK_CUR;
	} else if ((c == 'e') && (strncmp(argv[3], "end", length) == 0)) {
		mode = SEEK_END;
	} else {
	    Tcl_AppendResult(interp, "bad origin \"", argv[3],
		    "\": should be start, current, or end", (char *) NULL);
	    return TCL_ERROR;
	}
    }
    if (seek(filePtr->f, (long) offset) == -1) {
	Tcl_AppendResult(interp, "error during seek: ",
		Tcl_UnixError(interp), (char *) NULL);
	clearerr(filePtr->f);
	return TCL_ERROR;
	}

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SourceCmd --
 *
 *	This procedure is invoked to process the "source" Tcl command.
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
Tcl_SourceCmd(dummy, interp, argc, argv)
	ClientData dummy;			/* Not used. */
	Tcl_Interp *interp;			/* Current interpreter. */
	int argc;				/* Number of arguments. */
	char **argv;			/* Argument strings. */
{
    if (argc == 1) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileName\"", (char *) NULL);
	return TCL_ERROR;
    }
	return Tcl_EvalFile(interp, argv[1]);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_TellCmd --
 *
 *	This procedure is invoked to process the "tell" Tcl command.
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
Tcl_TellCmd(notUsed, interp, argc, argv)
	ClientData notUsed;			/* Not used. */
	Tcl_Interp *interp;			/* Current interpreter. */
	int argc;				/* Number of arguments. */
	char **argv;			/* Argument strings. */
{
    OpenFile *filePtr;
    struct mfblk *mfptr;

    if (argc == 1) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId\"", (char *) NULL);
	return TCL_ERROR;
    }
	if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }
    mfptr= (struct mfblk *) devtab[(int)filePtr->f].dvioblk;
    sprintf(interp->result, "%D", mfptr->mf_pos);
    return TCL_OK;
}
