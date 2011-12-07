/* fs_cvtmd.c - fs_cvtmd */

#include <fs.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 *  fs_cvtmd  -  convert Xinu file protection mode to analogous char str
 *------------------------------------------------------------------------
 */
int fs_cvtmd(xmode, mode)
int xmode;
char *mode;
{
	if ((xmode&FLREAD) && (xmode&FLWRITE)) /* Xinu Read + Write	*/
		xinu_strcpy(mode, "brw");
	else if (xmode & FLREAD)	/* Xinu read only		*/
		xinu_strcpy(mode, "br");
	else if (xmode & FLWRITE)	/* Xinu write only		*/
		xinu_strcpy(mode, "bw");
	else				/* Xinu neither of RW specified	*/
		xinu_strcpy(mode, "brw");
	return(OK);
}
