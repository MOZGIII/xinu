/* test.c -- test */

#include <conf.h>
#include <kernel.h>

/*-----------------------------------------
 * test -- function test 
 *-----------------------------------------
*/
int x_test(nargs, args)
int nargs;
char *args[];
{
    int i,j, k;
    char buf[128];
    buf[0]='/0';
    if (nargs != 3) return SYSERR;
    xinu_strcpy(buf, args[1]);
    strcat(buf, " ");
    strcat(buf, args[2]);
    j = atoi(args[1]);
    xinu_printf("%d\n",j);
    return OK;
/*    i=sscanf(buf,"%d %d",&j, &k);*/
    xinu_printf("i= %d j=%d k=%d\n",i,j,k);
    return OK;
}
