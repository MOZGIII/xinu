/* mfgetc.c - mfgetc */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <proc.h>
#include <mffile.h>

/*------------------------------------------------------------------------
 *  mfgetc  --  get next character from (buffered) disk file
 *------------------------------------------------------------------------
 */
mfgetc(devptr)
struct devsw *devptr;
{
	struct	mfblk	*mfptr;
	char	nextch;
	int	ps;

	disable(ps);
    mfptr = (struct mfblk *)devptr->dvioblk;
    if (!(mfptr->mf_mode & FLREAD)) {
		restore(ps);
		return(SYSERR);
	}
	if ( mfptr->mf_pos >= mfptr->mf_size ) {
		restore(ps);
		return(EOF);
	}
	if ( mfptr->mf_bptr >= &mfptr->mf_buff[DBUFSIZ] ) {
		mfsflush(mfptr);
		mfsetup(mfptr);
	}
	nextch = *(mfptr->mf_bptr)++;
	mfptr->mf_pos++;
	if ( !(mfptr->mf_mode & FLBINARY) && nextch == RETURN ) {
		if ( mfptr->mf_pos >= mfptr->mf_size ) {
			restore(ps);
			return(((int)nextch) & 0xff);
		}
		if ( mfptr->mf_bptr >= &mfptr->mf_buff[DBUFSIZ] ) {
			mfsflush(mfptr);
			mfsetup(mfptr);
		}
		if ( *(mfptr->mf_bptr) == NEWLINE ) {
			(mfptr->mf_bptr)++;
			mfptr->mf_pos++;
			nextch = NEWLINE;
		}
	}
	restore(ps);
	return( ((int)nextch) & 0xff );
}
