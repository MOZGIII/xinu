/* 
 * tclUnixUtil.c --
 *
 *	This file contains a collection of utility procedures that
 *	are present in the Tcl's UNIX core but not in the generic
 *	core.  For example, they do file manipulation and process
 *	manipulation.
 *
 *	The Tcl_Fork and Tcl_WaitPids procedures are based on code
 *	contributed by Karl Lehenbauer, Mark Diekhans and Peter
 *	da Silva.
 *
 */
#include <tclInt.h>
#include <msdos.h>
#include <mffile.h>
#include <errno.h>

/*
 * Data structures of the following type are used by Tcl_Fork and
 * Tcl_WaitPids to keep track of child processes.
 */

typedef struct {
    int pid;			/* Process id of child. */
    WAIT_STATUS_TYPE status;	/* Status returned when child exited or
				 * suspended. */
    int flags;			/* Various flag bits;  see below for
				 * definitions. */
} WaitInfo;

/*
 * Flag bits in WaitInfo structures:
 *
 * WI_READY -			Non-zero means process has exited or
 *				suspended since it was forked or last
 *				returned by Tcl_WaitPids.
 * WI_DETACHED -		Non-zero means no-one cares about the
 *				process anymore.  Ignore it until it
 *				exits, then forget about it.
 */

#define WI_READY	1
#define WI_DETACHED	2

static WaitInfo *waitTable = NULL;
static int waitTableSize = 0;	/* Total number of entries available in
				 * waitTable. */
static int waitTableUsed = 0;	/* Number of entries in waitTable that
				 * are actually in use right now.  Active
				 * entries are always at the beginning
				 * of the table. */
#define WAIT_TABLE_GROW_BY 4

/*
 *----------------------------------------------------------------------
 *
 * Tcl_EvalFile --
 *
 *	Read in a file and process the entire file as one gigantic
 *	Tcl command.
 *
 * Results:
 *	A standard Tcl result, which is either the result of executing
 *	the file or an error indicating why the file couldn't be read.
 *
 * Side effects:
 *	Depends on the commands in the file.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_EvalFile(interp, fileName)
    Tcl_Interp *interp;		/* Interpreter in which to process file. */
    char *fileName;		/* Name of file to process.  Tilde-substitution
				 * will be performed on this name. */
{
    int i, fileId, fileSize, result;
    char *cmdBuffer, *temp, *end, *oldScriptFile;
    Interp *iPtr = (Interp *) interp;
    struct mfblk *mfptr;

    end = "\0";
    oldScriptFile = iPtr->scriptFile;
    iPtr->scriptFile = fileName;
    fileName = Tcl_TildeSubst(interp, fileName); 
    if (fileName == NULL) {
	goto error;
    }
    fileId = xinu_open(DOS, fileName, "ro");
    mfptr= (struct mfblk *) devtab[fileId].dvioblk;
    if (fileId < 0) {
    Tcl_AppendResult(interp, "couldn't open file \"", fileName, 
        "\"",/* Tcl_UnixError(interp), */ (char *) NULL);
	goto error;
    }
    fileSize = mfptr->mf_size;
    cmdBuffer = (char *) ckalloc(fileSize+1);
    i = 0;
    temp = cmdBuffer;
    while ((result=xinu_read(fileId, cmdBuffer, fileSize)) > 0) {
        cmdBuffer += result;
        i += result;
    }
    cmdBuffer = temp;
    if (xinu_close(fileId) != OK) {
        Tcl_AppendResult(interp, "error closing file \"", fileName,
            "\": ", Tcl_UnixError(interp), (char *) NULL);
        ckfree(cmdBuffer);
        goto error;
	}
    cmdBuffer[i] = '\0';
    result = Tcl_Eval(interp, cmdBuffer, 0, &end);
    if (result == TCL_RETURN) {
	result = TCL_OK;
    }
    if (result == TCL_ERROR) {
	char msg[200];

	/*
	 * Record information telling where the error occurred.
	 */

	sprintf(msg, "\n    (file \"%.150s\" line %d)", fileName,
		interp->errorLine);
	Tcl_AddErrorInfo(interp, msg);
    }
    ckfree(cmdBuffer);
    iPtr->scriptFile = oldScriptFile;
    return result;

    error:
	iPtr->scriptFile = oldScriptFile;
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_UnixError --
 *
 *	This procedure is typically called after UNIX kernel calls
 *	return errors.  It stores machine-readable information about
 *	the error in $errorCode returns an information string for
 *	the caller's use.
 *
 * Results:
 *	The return value is a human-readable string describing the
 *	error, as returned by strerror.
 *
 * Side effects:
 *	The global variable $errorCode is reset.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_UnixError(interp)
    Tcl_Interp *interp;		/* Interpreter whose $errorCode variable
				 * is to be changed. */
{
    char *id, *msg;

    id = Tcl_ErrnoId();
    msg = strerror(errno);
    Tcl_SetErrorCode(interp, "UNIX", id, msg, (char *) NULL);
    return msg;
}

/*
 *----------------------------------------------------------------------
 *
 * TclMakeFileTable --
 *
 *	Create or enlarge the file table for the interpreter, so that
 *	there is room for a given index.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The file table for iPtr will be created if it doesn't exist
 *	(and entries will be added for stdin, stdout, and stderr).
 *	If it already exists, then it will be grown if necessary.
 *
 *----------------------------------------------------------------------
 */

void
TclMakeFileTable(iPtr, index)
    Interp *iPtr;		/* Interpreter whose table of files is
				 * to be manipulated. */
    int index;			/* Make sure table is large enough to
				 * hold at least this index. */
{
    /*
     * If the table doesn't even exist, then create it and initialize
     * entries for standard files.
     */

    if (iPtr->numFiles == 0) {
	OpenFile *filePtr;
	int i;

	if (index < 2) {
	    iPtr->numFiles = 3;
	} else {
	    iPtr->numFiles = index+1;
	}
	iPtr->filePtrArray = (OpenFile **) ckalloc((unsigned)
		((iPtr->numFiles)*sizeof(OpenFile *)));
	for (i = iPtr->numFiles-1; i >= 0; i--) {
	    iPtr->filePtrArray[i] = NULL;
	}

	filePtr = (OpenFile *) ckalloc(sizeof(OpenFile));
    filePtr->f = (char *) STDIN;
	filePtr->f2 = NULL;
	filePtr->readable = 1;
	filePtr->writable = 0;
	filePtr->numPids = 0;
	filePtr->pidPtr = NULL;
	filePtr->errorId = -1;
	iPtr->filePtrArray[0] = filePtr;

	filePtr = (OpenFile *) ckalloc(sizeof(OpenFile));
    filePtr->f = (char *) STDOUT;
	filePtr->f2 = NULL;
	filePtr->readable = 0;
	filePtr->writable = 1;
	filePtr->numPids = 0;
	filePtr->pidPtr = NULL;
	filePtr->errorId = -1;
	iPtr->filePtrArray[1] = filePtr;

	filePtr = (OpenFile *) ckalloc(sizeof(OpenFile));
    filePtr->f = (char *) STDERR;
	filePtr->f2 = NULL;
	filePtr->readable = 0;
	filePtr->writable = 1;
	filePtr->numPids = 0;
	filePtr->pidPtr = NULL;
	filePtr->errorId = -1;
	iPtr->filePtrArray[2] = filePtr;
    } else if (index >= iPtr->numFiles) {
	int newSize;
	OpenFile **newPtrArray;
	int i;

	newSize = index+1;
	newPtrArray = (OpenFile **) ckalloc((unsigned)
		((newSize)*sizeof(OpenFile *)));
	memcpy((VOID *) newPtrArray, (VOID *) iPtr->filePtrArray,
		iPtr->numFiles*sizeof(OpenFile *));
	for (i = iPtr->numFiles; i < newSize; i++) {
	    newPtrArray[i] = NULL;
	}
	ckfree((char *) iPtr->filePtrArray);
	iPtr->numFiles = newSize;
	iPtr->filePtrArray = newPtrArray;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TclGetOpenFile --
 *
 *	Given a string identifier for an open file, find the corresponding
 *	open file structure, if there is one.
 *
 * Results:
 *	A standard Tcl return value.  If the open file is successfully
 *	located, *filePtrPtr is modified to point to its structure.
 *	If TCL_ERROR is returned then interp->result contains an error
 *	message.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TclGetOpenFile(interp, string, filePtrPtr)
    Tcl_Interp *interp;		/* Interpreter in which to find file. */
    char *string;		/* String that identifies file. */
    OpenFile **filePtrPtr;	/* Address of word in which to store pointer
				 * to structure about open file. */
{
    int fd = 0;			/* Initial value needed only to stop compiler
                        * warnings. */
    Interp *iPtr = (Interp *) interp;

    if ((string[0] == 'f') && (string[1] == 'i') && (string[2] == 'l')
        && (string[3] == 'e')) {    /* an & was missing here */
        char *end;

        fd = strtoul(string+4, &end, 10);
        if ((end == string+4) || (*end != 0)) {
            goto badId;
        }
    } else if ((string[0] == 's') && (string[1] == 't')
                && (string[2] == 'd')) {
                if (strcmp(string+3, "in") == 0) {
                    fd = 0;
                } else if (strcmp(string+3, "out") == 0) {
                    fd = 1;
                } else if (strcmp(string+3, "err") == 0) {
                    fd = 2;
                } else {
                    goto badId;
                }
    } else {
        badId:
        Tcl_AppendResult(interp, "bad file identifier \"", string,
            "\"", (char *) NULL);
        return TCL_ERROR;
        }

    if (fd >= iPtr->numFiles) {
        if ((iPtr->numFiles == 0) && (fd <= 2)) {
            TclMakeFileTable(iPtr, fd);
        } else {
	    notOpen:
	    Tcl_AppendResult(interp, "file \"", string, "\" isn't open",
		    (char *) NULL);
	    return TCL_ERROR;
        }
    }
    if (iPtr->filePtrArray[fd] == NULL) {
	goto notOpen;
    }
    *filePtrPtr = iPtr->filePtrArray[fd];
    return TCL_OK;
}
