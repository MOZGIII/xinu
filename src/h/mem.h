/* mem.h - roundew, truncew, roundp, truncp, getstk, freestk */

#ifndef MEM
#define MEM

/*---------------------------------------------------------------------------
 * roundew, truncew - round or truncate address to next even word
 * roundp, truncp   - round or truncate address to next paragraph 
 *---------------------------------------------------------------------------
 */
#define roundew(x)	( (3 + (WORD)(x)) & (~3) )
#define truncew(x)	( ((WORD)(x)) & (~3) )

#define memp(x)		(x)
#define memnext(x)	(((MBLOCK *)(x))->mnext)
#define memlen(x)	(((MBLOCK *)(x))->mlen)
#define seg(x)		FP_SEG(x)
#define off(x)		FP_OFF(x)

#define getstk(n)	getmem(n)
#define freestk(b,s)	freemem(b,s)

typedef struct	mblock	{
    struct mblock *mnext;        
    word mlen;         /* was word */
} MBLOCK;

typedef struct xblock {
    struct xblock *xbback, *xbfore;   /* was para */
    word xblen;            /* was word */
} XBLOCK;

extern	MBLOCK	memlist;		/* head of free memory list	*/
extern  char* maxaddr;  /* max memory address (was para) */
extern  char* endaddr;  /* address beyond loaded mem (was para)*/

#define MMAX    65000           /* maximum memory size      */
#define MBLK    256             /* block size for global alloc  */
#define MMIN    512             /* minimum Xinu Allocation  */
#define MDOS	1024			/* save something for MS-DOS	*/

extern	char	*getmem();
extern  int 	freemem(char *, word);
extern  char	*xmalloc();
extern  char	*xcalloc();
extern	int	xfree();

extern	char	*sys_stkhqq();		/* get lower stack boundary	*/

#endif
