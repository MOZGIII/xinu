/* nullwrit.c - nullwrit */

#include <conf.h>
#include <kernel.h>




int nullwrit(devptr,buf,count)
struct devsw *devptr;
char *buf;
int  count;
{
	return(count);
}
