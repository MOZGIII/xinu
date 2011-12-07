/* fs.h */

#include <conf.h>
#include <kernel.h>       		/*** Xinu include files ***/
#include <network.h>
#include <io.h>

/* Defines for miscellaneous constants */

#define	MAXNFILES	15		/* maximum number of open files	*/
#define	NOFILE		MAXNFILES	/* last file entry is not a file*/
#define	FHDRLEN		sizeof(struct fphdr)/* request/reply header only*/

/* Declaration of file table entry format */

struct	filent	{			/* entry in table of files	*/
	char	name[RNAMLEN];		/* file's name			*/
	int	fd;			/* XINU file descriptor to use	*/
	int	mode;			/* mode of file			*/
	long	currpos;		/* current offset for this file	*/
	int	prev;			/* link to previously opened fl	*/
	int	next;			/* link to next file opened	*/
};

extern	struct	filent	fcache[];	/* table of open files		*/
extern	int	freelst;		/* first free entry in fcache	*/
#define	NEWEST  fcache[NOFILE].prev	/* youngest file in file table	*/
#define OLDEST  fcache[NOFILE].next	/* oldest file in file table	*/

/* Declaration of the request-to-handler mapping */

struct	reqmap	{			/* request-to-procedure mapping	*/
	int	req;			/* request (e.g., FS_OPEN)	*/
	int	(*reqproc)();		/* procedure to handle request	*/
};

extern	struct	reqmap	rq[];		/* array of request mappings	*/
extern	int	fsopen(), fsclose(), fsread(), fswrite(), fsremove(),
		fsrename(),fsrmdir(),fsmkdir(),fsaccess(),fstrans();
extern	int	port;			/* UDP port number to use or -1	*/
extern	char	*pname;			/* UDP port name to use		*/
