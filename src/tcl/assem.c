/* tclAssem.c -- This file contains procedures to help assemble Tcl commands
                 from an input source  where commands may arrive in pieces,
                * e.g. several lines of type-in corresponding to one command.
*/

#include <tclInt.h>

/*
 * The structure below is the internal representation for a command
 * buffer, which is used to hold a piece of a command until a full
 * command is available.  When a full command is available, it will
 * be returned to the user, but it will also be retained in the buffer
 * until the NEXT call to Tcl_AssembleCmd, at which point it will be
 * removed.
 */

typedef struct {
    char *buffer;		/* Storage for command being assembled.
                        * Malloc-ed, and grows as needed. */
    int bufSize;		/* Total number of bytes in buffer. */
    int bytesUsed;		/* Number of bytes in buffer currently
                        * occupied (0 means there is not a
                        * buffered incomplete command). */
} CmdBuf;

/*
 * Default amount of space to allocate in command buffer:
 */

#define CMD_BUF_SIZE 100

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CreateCmdBuf --
 *
 *	Allocate and initialize a command buffer.
 *
 * Results:
 *	The return value is a token that may be passed to
 *	Tcl_AssembleCmd and Tcl_DeleteCmdBuf.
 *
 * Side effects:
 *	Memory is allocated.
 *
 *----------------------------------------------------------------------
 */

Tcl_CmdBuf
Tcl_CreateCmdBuf()
{
    register CmdBuf *cbPtr;

    cbPtr = (CmdBuf *) ckalloc(sizeof(CmdBuf));
    cbPtr->buffer = (char *) ckalloc(CMD_BUF_SIZE);
    cbPtr->buffer[0] = '\0';
    cbPtr->bufSize = CMD_BUF_SIZE;
    cbPtr->bytesUsed = 0;
    return (Tcl_CmdBuf) cbPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DeleteCmdBuf --
 *
 *	Release all of the resources associated with a command buffer.
 *	The caller should never again use buffer again.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory is released.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_DeleteCmdBuf(buffer)
    Tcl_CmdBuf buffer;		/* Token for command buffer (return value
				 * from previous call to Tcl_CreateCmdBuf). */
{
    register CmdBuf *cbPtr = (CmdBuf *) buffer;

    ckfree(cbPtr->buffer);
    ckfree((char *) cbPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AssembleCmd --
 *
 *	This is a utility procedure to assist in situations where
 *	commands may be read piece-meal from some input source.  Given
 *	some input text, it adds the text to an input buffer and returns
 *	whole commands when they are ready.
 *
 * Results:
 *	If the addition of string to any currently-buffered information
 *	results in one or more complete Tcl commands, then the return value
 *	is a pointer to the complete command(s).  The command value will
 *	only be valid until the next call to this procedure with the
 *	same buffer.  If the addition of string leaves an incomplete
 *	command at the end of the buffer, then NULL is returned.
 *
 * Side effects:
 *	If string leaves a command incomplete, the partial command
 *	information is buffered for use in later calls to this procedure.
 *	Once a command has been returned, that command is deleted from
 *	the buffer on the next call to this procedure.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_AssembleCmd(buffer, string)
    Tcl_CmdBuf buffer;		/* Token for a command buffer previously
                            * created by Tcl_CreateCmdBuf.  */
    char *string;           /* Bytes to be appended to command stream.
                            * Note:  if the string is zero length,
                            * then whatever is buffered will be
                            * considered to be a complete command
                            * regardless of whether parentheses are
                            * matched or not. */
{
    register CmdBuf *cbPtr = (CmdBuf *) buffer;
    int length, totalLength, c;

    /*
     * If an empty string is passed in, just pretend the current
     * command is complete, whether it really is or not.
     */

    length = xinu_strlen(string);
    if (length == 0) {
	cbPtr->buffer[cbPtr->bytesUsed] = 0;
	cbPtr->bytesUsed = 0;
	return cbPtr->buffer;
    }

    /*
     * Add the new information to the buffer.  If the current buffer
     * isn't large enough, grow it by at least a factor of two, or
     * enough to hold the new text.
     */

    length = xinu_strlen(string);
    totalLength = cbPtr->bytesUsed + length + 1;
    if (totalLength > cbPtr->bufSize) {
	unsigned int newSize;
	char *newBuf;

	newSize = cbPtr->bufSize*2;
	if (newSize < totalLength) {
	    newSize = totalLength;
	}
    newBuf = (char *) ckalloc(newSize);
	strcpy(newBuf, cbPtr->buffer);
    ckfree(cbPtr->buffer);
	cbPtr->buffer = newBuf;
	cbPtr->bufSize = newSize;
    }
    strcpy(cbPtr->buffer+cbPtr->bytesUsed, string);
    cbPtr->bytesUsed += length;

    /*
     * See if there is now a complete command in the buffer.
     */

    c = cbPtr->buffer[cbPtr->bytesUsed-1];
    if ((c != '\n') && (c != ';')) {
    return NULLPTR;
    }
    if (Tcl_CommandComplete(cbPtr->buffer)) {
	cbPtr->bytesUsed = 0;
	return cbPtr->buffer;
    }
    return NULLPTR;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CommandComplete --
 *
 *	Given a partial or complete Tcl command, this procedure
 *	determines whether the command is complete in the sense
 *	of having matched braces and quotes and brackets.
 *
 * Results:
 *	1 is returned if the command is complete, 0 otherwise.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_CommandComplete(cmd)
    char *cmd;              /* Command to check. */
{
    register char *p = cmd;

    p = cmd;
    while (1) {
	while (isspace(*p)) {
	    p++;
	}
	if (*p == 0) {
	    return 1;
	}
	p = TclWordEnd(p, 0);
	if (*p == 0) {
	    return 0;
	}
	p++;
    }
}
