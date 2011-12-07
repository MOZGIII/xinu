/* name.h */

/* Constants that define the name mapping table sizes */

#define	NAMPLEN		32		/* max size of a name prefix	*/
#define	NAMRLEN		32		/* max size of a replacement	*/
#define	NAMLEN		80		/* maximum size of a file name	*/
#define	NNAMES		32		/* number of prefix definitions	*/
#define	REMOTE		 1		/* indicates a remote mount  	*/
#define	RLOCAL		 0  		/* mount can be overridden	*/

/* Definition of the name prefix table that defines all name mappings */

struct	nament	{			/* definition of prefix mapping	*/
	char	npre[NAMPLEN];		/* prefix of a name		*/
	char	nrepl[NAMRLEN];		/* replacement for that prefix	*/
	int	ndev;			/* device for this prefix	*/
	int	ntyp;			/* type of mount REMOTE/LOCAL	*/
};

struct	nam	{			/* all name space variables	*/
	int	nnames;			/* number of entries in nametab	*/
	struct	nament	nametab[NNAMES];/* actual table of mappings	*/
};

extern struct nam Nam;

#ifndef	NAMESPACE
#define	NAMESPACE	SYSERR
#endif
