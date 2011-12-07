#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
int intflag = 1; /* Flag for enabling/disabling interrupts */
/*;-------------------------------------------------------------------------
  ; _sys_disabl  --  return interrupt status and disable interrupts
  ;-------------------------------------------------------------------------*/
extern void vt_sig_handler(int sig);
int sys_disabl()
{
    int tmp = intflag;
    intflag = 0;
    return intflag;
}


/*;-------------------------------------------------------------------------
; _sys_enabl  --  enable interrupts unconditionally
;-------------------------------------------------------------------------*/
void sys_enabl()
{
    intflag = 1;
}

/*;-------------------------------------------------------------------------
; _sys_restor  --  restore interrupt status
;-------------------------------------------------------------------------*/

void sys_restor(int ps)
{
    if (ps == 1)
        sys_enabl();
    else sys_disabl();
    intflag = ps;
}

int sys_get_int_flag()
{
    return intflag;
}

/*;-------------------------------------------------------------------------
; _sys_wait  --  wait for interrupt
;-------------------------------------------------------------------------*/
void sys_wait()
{
    sys_enabl();
    pause();
}

void sys_hlt()
{
    close_keyboard();
    exit(0);
}
