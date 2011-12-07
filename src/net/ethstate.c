/* ethstate.c -- ethstate */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <pc.h>
#include <ec.h>

static unsigned char rv[34];      /* this will be in control block later */

#define MSK_PG2 (0x80 | MSK_RD2) /* put this in ec.h later */

/*------------------------------------------------------------------------
 *  ethstate  --  save/restore controller status (3C503 Version)
 *------------------------------------------------------------------------
 */	
ethstate(devptr, flag)
struct devsw	*devptr;
int flag;
{
    int    i, j;

    if (flag) {
        
        /* save 3C503 GA settings */
    
        for (i=0; i < 16; i++) {
            rv[i] = inportb(PSTR+i);
        }
        
        /* save 8390 page 0 registers */
        
        rv[i++] = inportb(CMDR);
        outportb(CMDR, MSK_PG2);            /* select Page 2 */
        rv[i++] = inportb(PSTART);
        rv[i++] = inportb(PSTOP);
        outportb(CMDR, MSK_PG0);            /* select Page 0 */
        rv[i++] = inportb(BNRY);
        outportb(CMDR, MSK_PG2);            /* select Page 2 */
        rv[i++] = inportb(TPSR);
        i += 7;
        rv[i++] = inportb(RCR);
        rv[i++] = inportb(TCR);
        rv[i++] = inportb(DCR);
        rv[i++] = inportb(IMR);
        outportb(CMDR, MSK_PG1);            /* select Page 1 */
        rv[i++] = inportb(CURR);
        outportb(CMDR, MSK_PG0);            /* select Page 0 */
        
        /* save 8259 interrupt mask */
        
        rv[i++] = inportb(IC_MASK);
        
/*        kprintf("3C503 saved settings:");
        for (j=0; j < 32; j++) {
            if ((j % 8) == 0) kprintf("\n");
            kprintf("%02x ",rv[j]);
        }
        kprintf("\n%02x %02x\n",rv[j], rv[j+1]);
*/
    } else {
        
        /* restore 3C503 GA registers (ints must be disabled) */
        
        outportb(CTRL, XSEL);     /* stop any DMA */ 

        outportb(PSTR, rv[0]);
        outportb(PSPR, rv[1]);
        outportb(DQTR, rv[2]);
        outportb(GACFR, rv[5]);
        outportb(CTRL, rv[6]);
        outportb(IDCFR, rv[8]);
        outportb(DAMSB, rv[9]);
        outportb(DALSB, rv[10]);
                  
        /* restore 8390 Page 0 and Page 1 */
        
        for (i=17; i < 21; i++)
            outportb(CMDR+i-16, rv[i]);
            
        for (i=28; i<32 ; i++)
            outportb(CMDR+i-16, rv[i]);
        
        outportb(CMDR, MSK_PG1);
        outportb(CURR, rv[i++]);
        outportb(CMDR, rv[16]);
        outportb(IC_MASK,rv[i++]);
    }
    return OK;
}
