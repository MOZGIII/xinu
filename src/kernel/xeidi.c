short pcxflag=1;
/*;-------------------------------------------------------------------------
; _sys_xdisabl  --  return pcxflag & disable context switching
;-------------------------------------------------------------------------*/
int sys_xdisabl()
{
    int oldflag =  pcxflag;
    pcxflag = 0;
    return oldflag;
}

/*;-------------------------------------------------------------------------
; _sys_xrestor  --  restore pcxflag
;-------------------------------------------------------------------------*/
void sys_xrestor(int ps)
{
    pcxflag = ps;
}

/*;-------------------------------------------------------------------------
; _sys_pcxget  --  get the current value of pcxflag
;-------------------------------------------------------------------------*/
int sys_pcxget()
{
    return pcxflag;
}

/*;-------------------------------------------------------------------------
; sys_getstk  --  get the stack parameters for panic printing
;-------------------------------------------------------------------------*/
void sys_getstk(int *sssp, int *spsp)
{

}
