/* lazy.c - getval(), getnext() */

#include <lazy.h>

/* function to return the current item in stream */
int getval(lp)
struct lazy *lp;
{
	if (lp->lazy_flag)
		return(lp->lazy_val);
	lp->lazy_flag=1;
	return(lp->lazy_val=(*lp->lazy_next)());
}

/* function to advance to the next item in stream */
getnext(lp)
struct lazy *lp;
{
	if (lp->lazy_flag==0) {
		lp->lazy_val=(*lp->lazy_next)();
		return;
	}
	lp->lazy_flag=0;
}
