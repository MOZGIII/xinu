/* bufpool.h */

#ifdef	MEMMARK
#ifndef	unmarked
#include <mark.h>
#endif
#endif

#ifndef	NBPOOLS
#define NBPOOLS 8           /* Maximum number of pools  */
#endif

#ifndef	BPMAXB
#define	BPMAXB	2048			/* Maximum buffer length	*/
#endif

#define	BPMINB	2			/* Minimum buffer length	*/

#ifndef	BPMAXN
#define	BPMAXN	100			/* Maximum buffers in any pool	*/
#endif

struct	bpool	{			/* Description of a single pool	*/
	int	bpsize;			/* size of buffers in this pool	*/
	int	*bpnext;		/* pointer to next free buffer	*/
	int	bpsem;			/* semaphore that counts buffers*/
	};				/*  currently in THIS pool	*/

extern	struct	bpool bptab[];		/* Buffer pool table		*/
extern	int	nbpools;		/* current number of pools	*/
extern  char    *getbuf(int);
extern  char    *getbufi(int);
#ifdef	MEMMARK
extern	MARKER	bpmark;
#endif
