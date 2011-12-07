/* x_hdlc.c - x_hdlc */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>
#include <tty.h>
#include <hdlc.h>

/*------------------------------------------------------------------------
 *  x_hdlc  -  (command hdlc) copy STDIN to SL0 using HDLC subset (DC A1)
 *------------------------------------------------------------------------
 */
COMMAND x_hdlc(nargs, args)
int nargs;
char *args[];
{
    struct hdlc *frame;                 /* pointer to a frame */
    int     hdlc_in();                  /* receive process */
    int     len;
    int     stop;

    stop = 0;
    resume(create(hdlc_in, INITSTK, INITPRIO,"hdlcin", 1, xinu_getpid()));
    
    while (stop==0) {
        frame = getbuf(Net.netpool);    /* get buffer from network pool*/
        frame->addr = 'A';
        frame->ctrl = 'I';
        len = xinu_read(STDIN, frame->data, 512);
        if (len <= 0) {
            frame->data[0] = EOFC;
            stop = len = 1;
        }
        xinu_write(SL0, frame, len+2);       /* does a freebuf on completion */
    }
    receive() ;                         /* wait for receiver completion */
    return(OK);
}


PROCESS hdlc_in(pid)                    /* receive process */
int pid;
{
    struct hdlc *frame;
    struct hdlc *ack;
    int    len;

    frame = getbuf(Net.netpool);        /* get a buffer */
    while (TRUE) {
        len=read(SL0, frame , 515);     /* read a frame */
        if (frame->data[0] == EOFC) {
            xinu_fprintf(STDERR,"EOF\n");
            break;
        }
        if (frame->ctrl == 'I') {           /* ack it */
            xinu_write(STDOUT, frame->data, len-2); /* display it */
            ack = getbuf(Net.netpool);
            ack->addr = 'A';
            ack->ctrl = 'R';
            xinu_fprintf(STDERR, "RR\n");
            xinu_write(SL0, ack, 2);
        }
    }
    freebuf(frame);
    send(pid,0);                        /* notify hdlc process */
    return OK;
}
