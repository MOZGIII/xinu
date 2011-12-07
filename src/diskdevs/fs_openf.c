/* fs_openf.c - fs_openf */

#include <fs.h>
#include <mffile.h>
#include <string.h>

/*------------------------------------------------------------------------
 *  fs_openf  -  open a file given its name and Xinu mode
 *------------------------------------------------------------------------
 */
fs_openf(name, xmode)
char	*name;
int	xmode;
{
extern	char	*fs_cvtmd();
	char 	ch_mode[4];
	int	fd;

	fs_cvtmd(xmode, ch_mode);
	if (xmode & FLOLD)
		return(xinu_fopen(name,ch_mode));
	mkdirs(name);
	if ((fd=xinu_fopen(name,ch_mode)) >= 0)
		return(fd);
	else
		return(SYSERR);			
}
