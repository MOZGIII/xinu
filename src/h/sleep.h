/* sleep.h */

#define	TICSN	19663
#define	TICSD	1080
#define TODQ	54
#define TODR	18198

extern	int	clockq;		/* q index of sleeping process list	*/
extern	int	*sltop;		/* address of first key on clockq	*/
extern	int	slnempty;	/* 1 iff clockq is nonempty		*/
extern	long	clktime;	/* time of day (seconds since epoch)	*/
extern	long	tod;		/* tics since (tics since startup)	*/

extern	int	defclk;		/* >0 iff clock interrupts are deferred	*/
extern	int	clkdiff;	/* number of clock ticks deferred	*/

extern	int	count10;	/* used to ignore 9 of 10 ticks		*/
extern	int	clmutex;	/* mutual exclusion sem. for clock	*/
