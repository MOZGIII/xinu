/* doscall.c - doscall, dos_creat, dos_open, dos_close, dos_read, dos_write, 
   dos_lseek, dos_mkdir, dos_mkdirs, dos_rmdir, dos_rmdirs, dos_unlink,
   dos_rename, dos_ffirst, dos_fnext, dos_setdta */

#include <dos.h>
#include <conf.h>
#include <kernel.h>
#include <fserver.h>
#include <string.h>
#include <errno.h>

#define AX(r)		((r).x.ax)
#define BX(r)		((r).x.bx)
#define	CX(r)		((r).x.cx)
#define DX(r)		((r).x.dx)
#define DI(r)		((r).x.di)
#define CFLAG(r)	((r).x.cflag)
#define AH(r)		((r).h.ah)
#define AL(r)		((r).h.al)
#define BH(r)		((r).h.bh)
#define BL(r)		((r).h.bl)
#define CH(r)		((r).h.ch)
#define CL(r)		((r).h.cl)
#define DH(r)		((r).h.dh)
#define DL(r)		((r).h.dl)
#define DS(r)		((r).ds)
#define ES(r)		((r).es)

#define DOS_SETDTA	0x1A
#define DOS_MKDIR	0x39
#define DOS_RMDIR	0x3a
#define DOS_CREAT	0x3c
#define DOS_OPEN	0x3d
#define DOS_CLOSE	0x3e
#define DOS_READ	0x3f
#define DOS_WRITE	0x40
#define DOS_UNLINK	0x41
#define DOS_LSEEK	0x42
#define DOS_FFIRST	0x4E
#define DOS_FNEXT	0x4F
#define DOS_RENAME	0x54


/* alias for converting between longs & 2 ints	*/
/* this is highly machine/compiler specific	*/
/* 8088 longs are stored lowword/highword	*/

struct L2I {
	int	lowword;
	int	highword;
};

/* Sorry. No DOS calls under Linux :) */
/*------------------------------------------------------------------------
 *  doscall  --  call a dos function
 *------------------------------------------------------------------------
 */
/*doscall(regsp,sregsp)
union REGS *regsp;
struct SREGS *sregsp;
{
	intdosx(regsp,regsp,sregsp); // make the DOS call		
	return(CFLAG(*regsp));	// nonzero return means error		
}*/

/*------------------------------------------------------------------------
 *  dos_creat  --  create a dos file
 *------------------------------------------------------------------------
 */
dos_creat(pathname,attr)
char *pathname;	/* filename */
int attr;	/* file attributes */
{
    return creat(pathname, attr);
}

/*------------------------------------------------------------------------
 *  dos_open  --  open a dos file
 *------------------------------------------------------------------------
 */
dos_open(pathname,mode)
char *pathname;		/* DOS filename	*/
int mode;		/* DOS file mode bits */
{
    int fd = open(pathname, mode);
    return fd;
}

/*------------------------------------------------------------------------
 *  dos_close  --  close a dos file
 *------------------------------------------------------------------------
 */
dos_close (fd)
int fd;		/* file handle to close */
{
    return close(fd);
}

/*------------------------------------------------------------------------
 *  dos_read  --  read from a dos file
 *------------------------------------------------------------------------
 */
dos_read (fd,buffer,count)
int fd;		/* file handle to read from */
char *buffer;	/* destination buffer */
int count;	/* no. of bytes to transfer */
{
    return read(fd, buffer, count);
}

/*------------------------------------------------------------------------
 *  dos_write  --  write to a dos file
 *------------------------------------------------------------------------
 */
dos_write (fd,buffer,count)
int fd;		/* file handle to write to */
char *buffer;	/* source buffer */
int count;	/* no. of bytest to transfer */
{
    return write(fd, buffer, count);
}

/*------------------------------------------------------------------------
 *  dos_lseek  --  perform an LSEEK on a dos file
 *------------------------------------------------------------------------
 */
long
dos_lseek(fd,offset,origin)
int fd;		/* file handle */
long offset;	/* offset into file */
int origin;	/* origin of seek - 0=beginning, 1=current, 2=end */
{
    return lseek(fd, offset, origin);
}

/*------------------------------------------------------------------------
 *  dos_setdta  --  set disk transfer area
 *------------------------------------------------------------------------
 */
/*dos_setdta(buf)
char	*buf;				// buf to write data into 
{
	union	REGS	regs;
	struct  SREGS	sregs;
	
	segread(&sregs);
	AH(regs) = DOS_SETDTA;
	DX(regs) = FP_OFF(buf);
	DS(sregs) = FP_SEG(buf);
	if ( doscall(&regs,&sregs) )
		return(SYSERR);
	return(OK);
}*/
/*------------------------------------------------------------------------
 *  dos_unlink  --  remove a file entry
 *------------------------------------------------------------------------
 */
dos_unlink(path)
char *path;		/* directory name */
{
    return unlink(path);
}

/*------------------------------------------------------------------------
 *  dos_mkdir  --  make a directory
 *------------------------------------------------------------------------
 */
dos_mkdir(path)
char *path;		/* directory name */
{
    return mkdir(path);
}

/*------------------------------------------------------------------------
 *  dos_mkdirs  --  make a directory tree
 *------------------------------------------------------------------------
 */
dos_mkdirs(path)
char *path;		/* directory name */
{
	char	*subdir;
	char	fname[RNAMLEN];
	int	ret = OK;

	for (subdir=strchr(path+1, '/') ;
	    subdir != NULL ;
	    subdir = strchr(subdir+1, '/')) {
		strncpy(fname, path, subdir-path);
		fname[subdir-path] = NULLCH;
		ret = dos_mkdir(fname);
	}
	return(ret);
}

/*------------------------------------------------------------------------
 *  dos_rmdir  --  remove a directory
 *------------------------------------------------------------------------
 */
dos_rmdir(path)
char *path;		/* directory name */
{
    rmdir(path);
}

/*------------------------------------------------------------------------
 *  dos_rmdirs  -- remove a directory tree
 *------------------------------------------------------------------------
 */
dos_rmdirs(path)
char *path;		/* directory name */
{
	char	*subdir;
	char	fname[RNAMLEN];

	strncpy(fname, path, sizeof(fname));
	fname[sizeof(fname)-1] = NULLCH;
	while ((subdir=strrchr(fname,'/')) != NULL && subdir != fname) {
		*subdir = NULLCH;
		if (dos_rmdir(fname) < 0)
			return(OK);
	}
	return(OK);
}

/*------------------------------------------------------------------------
 *  dos_rename  --  rename a file entry
 *------------------------------------------------------------------------
 */
dos_rename(path, newpath)
char *path;		/* file name */
char *newpath;		/* file name */
{
    return rename(path, newpath);
}

/*------------------------------------------------------------------------
 *  dos_ffirst  --  find first entry in directory that matches attributes
 *------------------------------------------------------------------------
 */
/*dos_ffirst(path, attr)
char *path;		// dir name 
int   attr;		// attributes 
{
	union	REGS	regs;
	struct  SREGS	sregs;

	segread(&sregs);
	DX(regs) = FP_OFF(path);
	DS(sregs) = FP_SEG(path);
	AH(regs) = DOS_FFIRST;
	CX(regs) = attr;
	if ( doscall(&regs,&sregs) )
		return(-AX(regs));
	return(OK);
}*/

/*------------------------------------------------------------------------
 *  dos_fnext  --  find next entry in directory 
 *------------------------------------------------------------------------
 */
/*dos_fnext()
{
	union	REGS	regs;
	struct  SREGS	sregs;

	segread(&sregs);
	AH(regs) = DOS_FNEXT;
	if ( doscall(&regs,&sregs) )
		return(-AX(regs));
	return(OK);
}
*/
