/* getdtime.c - getdtime */

#include <conf.h>
#include <kernel.h>
#include <dos.h>
#include <sys/time.h>

/* 1/100 os seconds per hour */
#define SANS_PER_DAY 24*60*60*100
/*------------------------------------------------------------------------
 *  getdtime  -  get DOS time in 1/100 seconds (past midnight)
 *------------------------------------------------------------------------
 */
SYSCALL getdtime(timvar)
long	*timvar;
{
    int ps;
    struct timeval tv;
    disable(ps);
    gettimeofday(&tv, NULL);
    restore(ps);    
    *timvar = (long)((tv.tv_sec * 100 + tv.tv_usec * 1000) / SANS_PER_DAY);
	return(OK);
}
