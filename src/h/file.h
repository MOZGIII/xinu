/* file.h */

/* Local disk layout: disk block 0 is directory, then index area, and	*/
/* then data blocks.  Each disk block (512 bytes) in the index area	*/
/* contains 8 iblocks, which are 64 bytes long.  Iblocks are referenced	*/
/* relative to 0, so the disk block address of iblock k is given by	*/
/* truncate(k/8)+1.  The offset of iblock k within its disk block is	*/
/* given by 64*remainder(k,8).  The directory entry points to a linked	*/
/* list of iblocks, and each iblock contains pointers to IBLEN (29) data*/
/* blocks. Index pointers contain a valid data block address or DBNULL.	*/

#define	EOF		-2		/* value returned on end-of-file*/
#define	FLREAD		001		/* fl_mode bit for "read"	*/
#define	FLWRITE		002		/* fl_mode bit for "write"	*/
#define	FLRW		003		/* fl_mode bits for read+write	*/
#define	FLNEW		010		/* fl_mode bit for "new file"	*/
#define	FLOLD		020		/* fl_mode bit for "old file"	*/
#define	FLBINARY	040		/* fl_mode bit for binary file	*/

#define	FLOMODE		'o'		/* Old    => file exists	*/
#define	FLNMODE		'n'		/* New    => file doesn't exist	*/
#define	FLRMODE		'r'		/* Read   => open for reading	*/
#define FLWMODE		'w'		/* Write  => open for writing	*/
#define FLBMODE		'b'		/* Binary => binary read/write	*/

#define	FLREMOVE	3001		/* remove file (name)		*/
#define	FLACCESS	3002		/* test access (name, mode)	*/
#define	FLRENAME	3003		/* change file name (old, new)	*/
#define	FLFORMAT	3004		/* PC-Xinu format floppy disk	*/
#define	FLCHDSK		3005		/* change the curr. floppy disk	*/
#define	FLSTAT		3006		/* get status of disk file	*/
#define FLMKDIR		3007		/* make a directory (name)	*/
#define FLMKDIRS	3008		/* make a directory tree	*/
#define FLRMDIR		3009		/* remove a directory (name)	*/
#define FLRMDIRS	3010		/* remove a directory tree	*/

struct	flblk	{			/* file "device" control block	*/
	int	fl_id;			/* file's "device id" in devtab	*/
	int	fl_dev;			/* file is on this disk device	*/
	int	fl_pid;			/* process id accessing the file*/
	struct	fdes	*fl_dent;	/* file's in-core dir. entry	*/
	int	fl_mode;		/* FLREAD, FLWRITE, or both	*/
	IBADDR	fl_iba;			/* address of iblock in fl_iblk	*/
	struct	iblk	fl_iblk;	/* current iblock for file	*/
	int	fl_ipnum;		/* current iptr in fl_iblk	*/
	long	fl_pos;			/* current file position (bytes)*/
	int	fl_ocount;		/* # of opens on this file	*/
	Bool	fl_dch;			/* has fl_buff been changed?	*/
	char	*fl_bptr;		/* ptr to next char in fl_buff	*/
	char	fl_buff[DBUFSIZ];	/* current data block for file	*/
};

#ifdef	Ndf
extern	struct	flblk	fltab[];
#endif
