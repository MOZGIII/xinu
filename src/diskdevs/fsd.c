/* fsd.c - fsd */

#include <fs.h>

/* Note that only one copy of this process may run, since it uses
 * global data structures
 */

struct	filent	fcache[MAXNFILES + 1];	/* Table of all open files	*/
int	freelst;			/* pointer to fcache free list	*/

/* Areas to hold incoming request packet and address of sender */
struct	xgram	xgm;			/* actual request/reply packet	*/

struct	reqmap	rq[] = {		/* table  of requests & handlers*/
	FS_OPEN,	fsopen,
	FS_CLOSE,	fsclose,
	FS_READ,	fsread,
	FS_WRITE,	fswrite,
    FS_REMOVE,  fsremove,
	FS_RENAME,	fsrename,
	FS_MKDIR,	fsmkdir,
	FS_RMDIR,	fsrmdir,
	FS_ACCESS,	fsaccess,
	FS_TRANS,	fstrans
};

#define	NREQS		(sizeof(rq)/sizeof(struct reqmap))

/*------------------------------------------------------------------------
 *  fsd  -  XINU remote file system daemon
 *------------------------------------------------------------------------
 */
PROCESS fsd()
{
	int	len;      		/* length of request		*/
	int	fplen;			/* length         		*/
	int	sndlen;			/* length of reply or zero	*/
	int	reqop;			/* operation in latest request	*/
	int	i;			/* index of request in rq	*/
	int	dgd;			/* Internet datagram device	*/
	struct	fpacket *fp;

	xinu_close(STDIN);
	xinu_close(STDOUT);
	if ((dgd=fs_init()) == SYSERR) {
		xinu_fprintf(STDERR,"fsd: initialization failure\n");
		return(SYSERR);
	}
	while (TRUE) {
		if ((len=fs_getrq(dgd, &xgm)) == SYSERR) {
			xinu_fprintf(STDERR,"fsd: request error\n");
			return(SYSERR);
		}
		fp = (struct fpacket *)(xgm.xg_data);
		reqop = fp->fp_h.f_op;
		for (i=0 ; i<NREQS ; i++)
			if (rq[i].req == reqop)
				break;
		if (i >= NREQS) {
			fp->fp_h.f_op = FS_ERROR;
			sndlen = FHDRLEN;
		} else
			sndlen = (*rq[i].reqproc)(fp, len);
		if (sndlen > 0)
			fs_sndrp(dgd, &xgm, sndlen);
	}
}
