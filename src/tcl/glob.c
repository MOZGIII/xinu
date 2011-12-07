/* 
 * tclGlob.c --
 *
 *	This file provides procedures and commands for file name
 *	manipulation, such as tilde expansion and globbing.
 *
 */

#include <tclInt.h>
#include <dirent.h>
#include <stat.h>

/*
 * The structure below is used to keep track of a globbing result
 * being built up (i.e. a partial list of file names).  The list
 * grows dynamically to be as big as needed.
 */

typedef struct {
    char *result;		/* Pointer to result area. */
    int totalSpace;		/* Total number of characters allocated
				 * for result. */
    int spaceUsed;		/* Number of characters currently in use
				 * to hold the partial result (not including
				 * the terminating NULL). */
    int dynamic;		/* 0 means result is static space, 1 means
				 * it's dynamic. */
} GlobResult;

/*
 * Declarations for procedures local to this file:
 */

static void		AppendResult _ANSI_ARGS_((Tcl_Interp *interp,
			    char *dir, char *name, int nameLength));
static int		DoGlob _ANSI_ARGS_((Tcl_Interp *interp, char *dir,
			    char *rem));

/*
 *----------------------------------------------------------------------
 *
 * AppendResult --
 *
 *	Given two parts of a file name (directory and element within
 *	directory), concatenate the two together and append them to
 *	the result building up in interp.
 *
 * Results:
 *	There is no return value.
 *
 * Side effects:
 *	Interp->result gets extended.
 *
 *----------------------------------------------------------------------
 */

static void
AppendResult(interp, dir, name, nameLength)
    Tcl_Interp *interp;		/* Interpreter whose result should be
				 * appended to. */
    char *dir;			/* Name of directory, with trailing
				 * slash (unless the whole string is
				 * empty). */
    char *name;         /* Name of file within directory (NOT
				 * necessarily null-terminated!). */
    int nameLength;		/* Number of characters in name. */
{
    int dirLength, dirFlags, nameFlags;
    char *p, saved;

    /*
     * Next, see if we can put together a valid list element from dir
     * and name by calling Tcl_AppendResult.
     */

    if (*dir == 0) {
	dirFlags = 0;
    } else {
	Tcl_ScanElement(dir, &dirFlags);
    }
    saved = name[nameLength];
    name[nameLength] = 0;
    Tcl_ScanElement(name, &nameFlags);
    if ((dirFlags == 0) && (nameFlags == 0)) {
	if (*interp->result != 0) {
	    Tcl_AppendResult(interp, " ", dir, name, (char *) NULL);
	} else {
	    Tcl_AppendResult(interp, dir, name, (char *) NULL);
	}
	name[nameLength] = saved;
	return;
    }

    /*
     * This name has weird characters in it, so we have to convert it to
     * a list element.  To do that, we have to merge the characters
     * into a single name.  To do that, malloc a buffer to hold everything.
     */

    dirLength = xinu_strlen(dir);
    p = (char *) ckalloc((unsigned) (dirLength + nameLength + 1));
    strcpy(p, dir);
    strcpy(p+dirLength, name);
    name[nameLength] = saved;
    Tcl_AppendElement(interp, p, 0);
    ckfree(p);
}

/*
 *----------------------------------------------------------------------
 *
 * DoGlob --
 *
 *	This recursive procedure forms the heart of the globbing
 *	code.  It performs a depth-first traversal of the tree
 *	given by the path name to be globbed.
 *
 * Results:
 *	The return value is a standard Tcl result indicating whether
 *	an error occurred in globbing.  After a normal return the
 *	result in interp will be set to hold all of the file names
 *	given by the dir and rem arguments.  After an error the
 *	result in interp will hold an error message.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
DoGlob(interp, dir, rem)
    Tcl_Interp *interp;			/* Interpreter to use for error
					 * reporting (e.g. unmatched brace). */
    char *dir;				/* Name of a directory at which to
					 * start glob expansion.  This name
					 * is fixed: it doesn't contain any
					 * globbing chars.  If it's non-empty
					 * then it should end with a slash. */
    char *rem;				/* Path to glob-expand. */
{
    /*
     * When this procedure is entered, the name to be globbed may
     * already have been partly expanded by ancestor invocations of
     * DoGlob.  The part that's already been expanded is in "dir"
     * (this may initially be empty), and the part still to expand
     * is in "rem".  This procedure expands "rem" one level, making
     * recursive calls to itself if there's still more stuff left
     * in the remainder.
     */

    register char *p;
    register char c;
    char *openBrace, *closeBrace;
    int gotSpecial, result;

    /*
     * When generating information for the next lower call,
     * use static areas if the name is short, and malloc if the name
     * is longer.
     */

#define STATIC_SIZE 200

    /*
     * First, find the end of the next element in rem, checking
     * along the way for special globbing characters.
     */
    gotSpecial = 0;
    openBrace = closeBrace = NULL;
    for (p = rem; ; p++) {
	c = *p;
    if ((c == '\0') /*|| (c == '/')*/) {                /* altered */
	    break;
	}
	if ((c == '{') && (openBrace == NULL)) {
	    openBrace = p;
	}
	if ((c == '}') && (closeBrace == NULL)) {
	    closeBrace = p;
	}
	if ((c == '*') || (c == '[') || (c == '\\') || (c == '?')) {
	    gotSpecial = 1;
	}
    }

    /*
     * If there is an open brace in the argument, then make a recursive
     * call for each element between the braces.  In this case, the
     * recursive call to DoGlob uses the same "dir" that we got.
     * If there are several brace-pairs in a single name, we just handle
     * one here, and the others will be handled in recursive calls.
     */

    if (openBrace != NULL) {
	int remLength, l1, l2;
	char static1[STATIC_SIZE];
	char *element, *newRem;

	if (closeBrace == NULL) {
	    Tcl_ResetResult(interp);
	    interp->result = "unmatched open-brace in file name";
	    return TCL_ERROR;
	}
	remLength = xinu_strlen(rem) + 1;
	if (remLength <= STATIC_SIZE) {
	    newRem = static1;
	} else {
	    newRem = (char *) ckalloc((unsigned) remLength);
	}
	l1 = openBrace-rem;
	strncpy(newRem, rem, l1);
	p = openBrace;
	for (p = openBrace; *p != '}'; ) {
	    element = p+1;
	    for (p = element; ((*p != '}') && (*p != ',')); p++) {
		/* Empty loop body:  just find end of this element. */
	    }
	    l2 = p - element;
	    strncpy(newRem+l1, element, l2);
	    strcpy(newRem+l1+l2, closeBrace+1);
	    if (DoGlob(interp, dir, newRem) != TCL_OK) {
		return TCL_ERROR;
	    }
	}
	if (remLength > STATIC_SIZE) {
	    ckfree(newRem);
	}
	return TCL_OK;
    }

    /*
     * If there were any pattern-matching characters, then scan through
     * the directory to find all the matching names.
     */

    if (gotSpecial) {
        DIR *d;
        struct dirent *entryPtr;
        int l1, l2;
        char *pattern, *newDir, *dirName;
        char static1[STATIC_SIZE], static2[STATIC_SIZE];
        struct stat statBuf;

	/*
	 * Be careful not to do any actual file system operations on a
	 * directory named "";  instead, use ".".  This is needed because
	 * some versions of UNIX don't treat "" like "." automatically.
	 */

        if (*dir == '\0') {
            dirName = "*.*";    /* was . */
        } else {
            dirName = dir;
            }
        d = opendir(dirName);
        if (d == NULL) {
            Tcl_ResetResult(interp);
            Tcl_AppendResult(interp, "couldn't read directory \"",
		    dirName, "\": ", Tcl_UnixError(interp), (char *) NULL);
            return TCL_ERROR;
        }
        l1 = xinu_strlen(dir);
        l2 = (p - rem);
        if (l2 < STATIC_SIZE) {
            pattern = static2;
        } else {
            pattern = (char *) ckalloc((unsigned) (l2+1));
            }
        strncpy(pattern, rem, l2);
        pattern[l2] = '\0';
        result = TCL_OK;
        while (1) {
            entryPtr = readdir(d);
            if (entryPtr == NULL) {
                break;
            }
	    /*
	     * Don't match names starting with "." unless the "." is
	     * present in the pattern.
	     */

	    if ((*entryPtr->d_name == '.') && (*pattern != '.')) {
            continue;
	    }
        if (Tcl_StringMatch(entryPtr->d_name, pattern)) {
            int nameLength = xinu_strlen(entryPtr->d_name);

            if (*p == 0) {
                AppendResult(interp, dir, entryPtr->d_name, nameLength);
            } else {
                if ((l1+nameLength+2) <= STATIC_SIZE) {
                newDir = static1;
                } else {
                    newDir = (char *) ckalloc((unsigned) (l1+nameLength+2));
                    }
                sprintf(newDir, "%s%s/", dir, entryPtr->d_name);
                result = DoGlob(interp, newDir, p+1);
                if (newDir != static1) {
                    ckfree(newDir);
                }
                if (result != TCL_OK) {
                    break;
                }
              }
            }
        }
        closedir(d);
        if (pattern != static2) {
            ckfree(pattern);
        }
        return result;
    }

    /*
     * This is the simplest case:  just another path element.  Move
     * it to the dir side and recurse (or just add the name to the
     * list, if we're at the end of the path).
     */

    if (*p == 0) {
	AppendResult(interp, dir, rem, p-rem);
    } else {
	int l1, l2;
	char *newDir;
	char static1[STATIC_SIZE];

	l1 = xinu_strlen(dir);
	l2 = l1 + (p - rem) + 2;
	if (l2 <= STATIC_SIZE) {
	    newDir = static1;
	} else {
	    newDir = (char *) ckalloc((unsigned) l2);
	}
	strcpy(newDir, dir);
	strncpy(newDir+l1, rem, p-rem);
	newDir[l2-2] = '/';
	newDir[l2-1] = 0;
	result = DoGlob(interp, newDir, p+1);
	if (newDir != static1) {
	    ckfree(newDir);
	}
	if (result != TCL_OK) {
	    return TCL_ERROR;
	}
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GlobCmd --
 *
 *	This procedure is invoked to process the "glob" Tcl command.
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
Tcl_GlobCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int i, result, noComplain;

    if (argc < 2) {
	notEnoughArgs:
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" ?-nocomplain? name ?name ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    noComplain = 0;
    if ((argv[1][0] == '-') && (strcmp(argv[1], "-nocomplain") == 0)) {
	if (argc < 3) {
	    goto notEnoughArgs;
	}
	noComplain = 1;
    }

    for (i = 1 + noComplain; i < argc; i++) {
	char *thisName;
    int j;
    
	/*
	 * Do special checks for names starting at the root and for
	 * names beginning with ~.  Then let DoGlob do the rest.
	 */

	thisName = argv[i];
    
    for (j=0 ; j<xinu_strlen(thisName) ; j++)      /* convert alpha to upper */
        if (isalpha(thisName[j]))
            thisName[j] = toupper(thisName[j]);
            
	if (*thisName == '~') {
	    thisName = Tcl_TildeSubst(interp, thisName);
	    if (thisName == NULL) {
		return TCL_ERROR;
	    }
	}
	if (*thisName == '/') {
	    result = DoGlob(interp, "/", thisName+1);
	} else {
        result = DoGlob(interp, "", thisName);
	}
	if (result != TCL_OK) {
	    return result;
	}
    }
    if ((*interp->result == 0) && !noComplain) {
	interp->result = "no files matched glob pattern(s)";
	return TCL_ERROR;
    }
    return TCL_OK;
}
