/* x_rls.c - x_rls */

#include <conf.h>
#include <kernel.h>
#include <shell.h>
#include <network.h>
#include <name.h>
#include <mem.h>


/*------------------------------------------------------------------------
 *  x_rls  -  (command rls) list contents of remote file system directory
 *------------------------------------------------------------------------
 */
COMMAND x_rls(nargs, args)
int     nargs;
char	*args[];
{
	char	*p, *buf;
	int	dev, len;
	char	str[256];
	struct	dirent	{	/* UNIX directory entry		*/
		long	d_inum;	/* file's inode number		*/
		short	d_rlen;	/* length of this record	*/
		short	d_nlen;	/* length of this file's name	*/
		char	d_nam[1]; /* start of file name		*/
	};
	struct	dirent	*d;
	Bool	aflag;

	aflag = FALSE;
	if (nargs > 1 && strcmp(p=args[1],"-a") == 0) {
		nargs--;
		aflag = TRUE;
		p = args[2];
        }
	if (nargs == 1)
	  p = ".";
	else if (nargs != 2) {
        xinu_fprintf(STDOUT,"Usage: rls [-a] directory\n");
		return(SYSERR);
	}
	if ( ((long)(buf=(char *)getmem(512))) == SYSERR) {
        xinu_fprintf(STDERR, "rls: no memory\n");
		return(SYSERR);
	}
    if (nammap(buf, p) != REMOTE || (dev=xinu_open(NAMESPACE, buf, "ro")) == SYSERR) {
        xinu_fprintf(STDERR, "cannot open %s\n", buf);
		freemem(buf, 512);
		return(SYSERR);
	}
	while ( (len = xinu_read(dev, buf, 512)) > 0) {
		for (p=buf ; p< &buf[512] ; p += d->d_rlen) {
			d = (struct dirent *)p;
			/* this could be a Vax or a Sun, so be */
			/* prepared to swap integer fields     */
			if (d->d_nlen != xinu_strlen(d->d_nam)) {
			    d->d_nlen = vax2h(d->d_nlen);
			    d->d_rlen = vax2h(d->d_rlen);
			}
			
			if (d->d_inum == 0)
			  continue;
			if (len < 512 ||
			    d->d_nlen != xinu_strlen(d->d_nam) ||
			    d->d_nlen > 255 ||
			    d->d_rlen < sizeof(struct dirent) ||
			    d->d_rlen > &buf[512] - p) {
                xinu_fprintf(STDERR, "Not a directory\n");
			    xinu_close(dev);
                freemem(buf, 512);
			    return(SYSERR);
		        }
			if (aflag || d->d_nam[0] != '.') {
				xinu_strcpy(str, d->d_nam);
				strcat(str, "\n");
                xinu_write(STDOUT, str, xinu_strlen(str));
			}
			if (d->d_rlen == 0)
			  break;
		}
	}
	freemem(buf, 512);
	xinu_close(dev);
	return(OK);
}
