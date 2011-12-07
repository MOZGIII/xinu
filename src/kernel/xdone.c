/* xdone.c - xdone */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <io.h>
#include <disk.h>
#include <tty.h>

/*------------------------------------------------------------------------
 *  xdone  --  print system termination message and terminate PC-Xinu
 *------------------------------------------------------------------------
 */
int xdone()
{
	int	kprintf();
	int ps, i, ret;

#ifdef Ndsk
	for ( i=0; i<Ndsk; i++ )
		control(dstab[i].dnum,DSKSYNC);	/* sync the disks	*/
#endif
	sleep(1);			/* let tty output settle	*/
	xdisable(ps);
	scrollup(0,0x184f,0, 0);         /* clear the screen     */
	maprestore();           /* restore interrupt vectors    */
	for ( i=0 ; i<NDEVS ; i++) {
		ret = init(i,0);	/* un-initialize device */
		if (ret != OK)
			kprintf("xdone: error un-initializing device %d\n", i);
	}
	enable();
	kprintf("\n\n-- system halt --\n\n");
	if (numproc==0)
		kprintf("All user processes have completed\n");
	else
        kprintf("PC XINU 7.9 terminated with %d process%s active\n",
			numproc, ((numproc==1) ? "" : "es"));
	kprintf("Returning to DOS . . .\n\n");
	close_keyboard();
	endwin();
	halt();				/* return to caller		*/
}
