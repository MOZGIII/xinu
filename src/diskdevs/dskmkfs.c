/* dskmkfs.c - dskmkfs */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>
#include <mark.h>
#include <bufpool.h>
LOCAL dskmkil(int diskdev, int niblks);
LOCAL dskmkdl(int diskdev, DBADDR firstfb, DBADDR lastfb);

/*------------------------------------------------------------------------
 *  dskmkfs  --  make a fresh file system on a disk
 *------------------------------------------------------------------------
 */
dskmkfs(diskdev, niblks, nblocks, diskid)
int diskdev;
int niblks;
unsigned nblocks;
int diskid;
{
	DBADDR	firstdb, lastdb;
	struct	dir	*dirptr;

	xinu_printf("Making a file system - device=%d\n",diskdev);
	xinu_printf(" iblocks=%d dblocks=%d diskid=%d\n",niblks,nblocks,diskid);
	xinu_printf("Clearing directory block...\n");
	dirptr = dsdirec(diskdev);
	firstdb = (DBADDR) (ibtodb(niblks-1) + 1);
	lastdb = (DBADDR) (nblocks - 1);
	dirptr->d_iblks = niblks;
	dirptr->d_fblst = firstdb;
	dirptr->d_filst = (IBADDR) niblks-1;
	dirptr->d_id = diskid;
	dirptr->d_nfiles = 0;
	write(diskdev, dskbcpy(dirptr), DIRBLK);
	dskmkil(diskdev, niblks);
	dskmkdl(diskdev, firstdb, lastdb);	
	xinu_printf("Done initializing disk...\n");
	return(OK);
}

/*------------------------------------------------------------------------
 *  dskmkdl  --  link all data blocks into free list to init. file system
 *------------------------------------------------------------------------
 */
LOCAL
dskmkdl(diskdev, firstfb, lastfb)
int	diskdev;
DBADDR	firstfb, lastfb;
{
	struct	freeblk	*buff;
	DBADDR	dba;
	char	*bptr;
	int	i;

	xinu_printf("Linking %u free blocks from %u through %u...\n",
		lastfb - firstfb + (DBADDR)1, firstfb, lastfb);
	buff = (struct freeblk *) (bptr = getbuf(dskdbp));
	for (i=0 ; i<DBUFSIZ ; i++)
		*bptr++ = '\000';
	for (dba=firstfb ; dba < lastfb ; dba++) {
		buff->fbnext = (DBADDR) (dba+1);
		write(diskdev, dskbcpy(buff), dba);
		if ( dba%100 == 0 )
			xinu_printf("%u",dba);
		else if ( dba%10 == 0 )
			xinu_printf(".");
	}
	xinu_printf("\n");
	buff->fbnext = (DBADDR) DIRBLK;
	write(diskdev, buff, dba);
	xinu_printf("\nDone linking data blocks...\n");
	return(OK);
}

/*------------------------------------------------------------------------
 *  dskmkil  --  link ALL i-blocks onto a free list
 *------------------------------------------------------------------------
 */
LOCAL
dskmkil(diskdev, niblks)
int	diskdev;
int	niblks;
{
	struct	iblk	iblock;
	IBADDR	ibnum;
	int	i;

	xinu_printf("Linking %d i-blocks together...\n", niblks);
	ibclear(&iblock, 0L);
	for (ibnum=niblks-1 ; ibnum>0 ; ibnum--) {
		iblock.ib_next = ibnum-1;
		ibput(diskdev, ibnum, &iblock);
	}
	iblock.ib_next = IBNULL;
	ibput(diskdev, 0, &iblock);
	xinu_printf("Done linking i-blocks...\n");
	return(OK);
}
