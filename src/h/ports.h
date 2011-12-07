/* ports.h - isbadport */

#ifdef	MEMMARK
#ifndef	unmarked
#include <mark.h>
#endif
#endif
#ifndef	NPORTS
#define	NPORTS		20		/* default max number of ports	*/
#endif
#define MAXMSGS     512     /* maximum messages on all ports*/
#define	PTFREE		1		/* port is free			*/
#define	PTLIMBO		2		/* port is being deleted/reset	*/
#define	PTALLOC		3		/* port is allocated		*/
#define	PTEMPTY		-1		/* initial semaphore entries	*/

#define PTNODISP    0       /* ptclear() null disposal func */

struct	ptnode	{			/* node on list of message ptrs	*/
	char	*ptmsg;			/* message pointer		*/
	struct ptnode	*ptnext;	/* address of next node on list	*/
};

struct	pt	{			/* entry in the port table	*/
	char	ptstate;		/* port state (FREE/LIMBO/ALLOC)*/
	int	ptssem;			/* sender semaphore		*/
	int	ptrsem;			/* receiver semaphore		*/
	int	ptmaxcnt;		/* max messages to be queued	*/
	int	ptct;			/* no. of messages in queue	*/
	struct	ptnode	*pthead;	/* list of message pointers	*/
	struct	ptnode	*pttail;	/* tail of message list		*/
};

extern	struct	ptnode	*ptfree;	/* list of free nodes		*/
extern	struct	pt	ports[];	/* port table			*/
extern	int	ptnextp;		/* next port to examine when	*/
					/*   looking for a free one	*/

#ifdef	MEMMARK
extern	MARKER	ptmark;
#endif

#define	isbadport(portid)	( (portid)<0 || (portid)>=NPORTS )
extern int pdelete(int, int (*)());
extern char *preceive();
extern char *preceivi();
