/* mfputc.c - mfputc */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <mffile.h>
#include <tty.h>

/*------------------------------------------------------------------------
 *  mfputc  --  put a character onto a (buffered) disk file
 *------------------------------------------------------------------------
 */
mfputc(devptr, ch)
struct	devsw	*devptr;
char	ch;
{
	struct	mfblk	*mfptr;
	int	ps;

	disable(ps);
	mfptr = (struct mfblk *) devptr->dvioblk;
	if ( mfptr->mf_mode&FLWRITE == 0 ) {
		restore(ps);
		return(SYSERR);
	}
	if ( ch == NEWLINE && (mfptr->mf_mode & FLBINARY) == 0) {
		/* convert NEWLINE -> RETURN/NEWLINE */
		if ( mfptr->mf_bptr >= &mfptr->mf_buff[DBUFSIZ] ) {
			mfsflush(mfptr);
			mfsetup(mfptr);
		}
		mfptr->mf_pos++;
		if ( mfptr->mf_pos > mfptr->mf_size )
			mfptr->mf_size = mfptr->mf_pos;
		*(mfptr->mf_bptr)++ = RETURN;
	}
	if ( mfptr->mf_bptr >= &mfptr->mf_buff[DBUFSIZ] ) {
		mfsflush(mfptr);
		mfsetup(mfptr);
	}
	mfptr->mf_pos++;
	if ( mfptr->mf_pos > mfptr->mf_size )
		mfptr->mf_size = mfptr->mf_pos;
	*(mfptr->mf_bptr)++ = ch;
	mfptr->mf_dch = TRUE;
	restore(ps);
	return(OK);
}
