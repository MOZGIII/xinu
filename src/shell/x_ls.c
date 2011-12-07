/* ls.c - ls */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <sleep.h>
#include <io.h>
#include <disk.h>
#include <file.h>
#include <name.h>
#include <dirent.h>

#define	SCRNSIZ	80			/* no. of cols per screen	*/
#define	FIELDSZ	3			/* size of a screen field	*/

struct fnode {
	char	fname[FDNLEN];
	long	flen;
};

struct dirstruct {
	char	dvnam[10];
	int	dnfiles;
	struct	fnode	flist[NFDES];
};
LOCAL lddskdir(struct	dirstruct *dirtab);
LOCAL dircmp(struct fnode *p1, struct fnode *p2);
/*------------------------------------------------------------------------
 *  x_ls  --  (commands ls/ll) generate UNIX like ls (ll) output
 *------------------------------------------------------------------------
 */
COMMAND  x_ls(nargs,args)
int	nargs;
char	**args;
{
	int     dskdir();
	int	device;
	char	*drvname;
	struct	dirstruct *dirtab;
	int	i,j,inc;
	int	dircmp();
	int	col;			/* no. of columns per line	*/
	int	fnlen, f;

	if ( nargs > 2 ) {
		xinu_fprintf(STDERR,"Usage: %s [drive]\n",args[0]);
		return(SYSERR);
	}
	
	dirtab = (struct dirstruct*)xmalloc(sizeof(struct dirstruct));
	
	if (dirtab  == 0 ) {
		xinu_fprintf(STDERR,"%s: not enough memory\n",args[0]);
		return(SYSERR);
	}

	if ( (fnlen = lddskdir(dirtab)) == SYSERR) {
		xinu_fprintf(STDERR,"%s: error while reading current dir\n");
		return(SYSERR);
	}

	fnlen++;
	if ( (f=(fnlen%FIELDSZ)) != 0 )
		fnlen += (FIELDSZ-f);
	col = SCRNSIZ/fnlen;
	if ( dirtab->dnfiles == 0 )
		xinu_fprintf(STDERR,"No files on %s\n",dirtab->dvnam);
	else {
		qsort(dirtab->flist,dirtab->dnfiles,sizeof(struct fnode),dircmp);
		if ( strcmp(args[0],"ls") == 0 ) {
			if ( (inc=(dirtab->dnfiles+col-1)/col) <= 0)
				inc = 1;
			for(i=0 ; i<inc && i<dirtab->dnfiles ; i++) {
				for (j=i ; j<dirtab->dnfiles ; j+=inc)
					xinu_printf("%-*s  ",fnlen,dirtab->flist[j].fname);
				xinu_printf("\n");
			}
		} else {
			xinu_printf("total %d\n",dirtab->dnfiles);
			for(i=0; i<dirtab->dnfiles; i++) {
				xinu_printf("%-10s %8ld\n",
				dirtab->flist[i].fname,dirtab->flist[i].flen);
			}
		}
	}
	return(OK);
}

/*------------------------------------------------------------------------
 *  lddskdir  --  load current dir into directory entry structure
 *------------------------------------------------------------------------
 */
LOCAL lddskdir(struct	dirstruct   *dirtab)
{
	DBADDR	firstdb, lastdb;
	DIR     *dirptr;
	int	l;
	int	fnlen;			/* no. of chars per fname	*/
	int 	ps;
	char *cwd;
	struct dirent *dent;
	int filecount = 0;
	disable(ps);

	cwd = xmalloc(1024);
	getcwd(cwd, 1024);
	dirptr = opendir(cwd);
	xfree(cwd);
	if (dirptr == 0)
	    return(SYSERR);
	
	dent = NULL;
	fnlen = 0;

	while(((dent = readdir(dirptr)) != 0) && filecount < NFDES)
	{
	    l = xinu_strlen(dent->d_name);
	    if ( l <= 0 || l >= FDNLEN ) {
		char fmt[10];
		xinu_sprintf(fmt, "%%%d.%ds~", FDNLEN-2, FDNLEN-2);
		xinu_sprintf(dirtab->flist[filecount].fname, fmt, dent->d_name);
		l = xinu_strlen("*ERROR*");
	    } else
		xinu_strcpy(dirtab->flist[filecount].fname, dent->d_name);
	    if ( l > fnlen)
		fnlen = l;
	    dirtab->flist[filecount].flen = l;
	    filecount++;       
	}
	xinu_strcpy(dirtab->dvnam, "DISK");
	
	dirtab->dnfiles = filecount;
	restore(ps);
	closedir(dirptr);
	return(fnlen);
}

/*------------------------------------------------------------------------
 *  dircmp  --  compare two entries in the directory list (see strcmp)
 *------------------------------------------------------------------------
 */
LOCAL dircmp(p1,p2)
struct fnode *p1, *p2;
{
#ifdef DEBUG
kprintf("comparing %s and %s\n",p1->fname,p2->fname);
#endif
	return(strcmp(p1->fname,p2->fname));
}
