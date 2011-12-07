/* x_dump.c    x_dump */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <tty.h>
#include <dos.h>

/*--------------------------------------------------------------------------
 * x_dump  -  dumps one paragraph in hex starting at <address>
 *--------------------------------------------------------------------------
 */

COMMAND x_dump(nargs,args)
int     nargs;
char    *args[];
{
	int     i,len;
	char    *addr;
	unsigned int    addr_seg;
	unsigned int    addr_off;
	char    ch;
    char    buf[80];

for ( ; TRUE ; ){

    xinu_printf("\naddr: ");
    if ((len=read(STDIN,buf,80))==0)
        len=read(STDIN,buf,80);
	if (buf[0]=='q')   break;           /* quit */
    buf[len-1]=NULLCH;
    i=sscanf(buf,"%x %x",&addr_seg,&addr_off);
	if (i==-1) addr_off=addr_off+16;
    xinu_printf("%04x:%04x ",addr_seg,addr_off);
	addr=MK_FP(addr_seg,addr_off);
	for (i=16 ; i != 0 ; i-- ) {
        xinu_printf("%02x ",(*addr & 0x00FF));
	    addr++;
	}
	addr=MK_FP(addr_seg,addr_off);
	for (i=16 ; i != 0 ; i-- ) {
        if (*addr < 0x20) xinu_printf(".");
	    else
        xinu_printf("%c",(*addr & 0x00FF));
	    addr++;
	}

 }
	return(OK);
}
