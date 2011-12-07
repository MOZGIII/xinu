/* pcscreen.c - putcsrpos, scrollup, putchr */

#include <dos.h>
#include <conf.h>
#include <kernel.h>
#include <window.h>
#include <vidio.h>
#include <ncurses.h>

/*------------------------------------------------------------------------
 *  putcsrpos  --  put the cursor at a given position
 *------------------------------------------------------------------------
 */
putcsrpos(csr,page)
CURSOR csr;
unsigned page;
{
    int ps;
    disable(ps);
    move(csr.row, csr.col);
    refresh();
    restore(ps);
}

/*------------------------------------------------------------------------
 *  scrollup  --  scroll a window up
 *------------------------------------------------------------------------
 */
scrollup(tl,br,lines,attr)
CURSOR tl,br;
unsigned lines;
unsigned char attr;
{

    int ps;
    int y, i, x, r, c;
    int colnum;
    chtype linebuf[81];
    
    disable(ps);

    
    colnum = (int)(attr & (~(3 << 6)));
    
    for (y = 0; y < 81; ++y) linebuf[y] = ' ' | COLOR_PAIR(colnum);
	
    /* lines == 0  means 'clear screen' */
    if (lines == 0)
    {
	int r, c;
	for (r = tl.row; r < br.row+1; ++r)
	{
	    move(r, tl.col);
	    mvaddchnstr(r, tl.col, linebuf, br.col-tl.col+1);
	}
	refresh();
	return;
    }
    
    i = 0, x = 0;
    for (y = tl.row + lines; y < br.row+1; ++y)
    {
	mvinchnstr(y, tl.col, linebuf, br.col-tl.col+1);
	mvaddchstr(tl.row + i, tl.col, linebuf);
	i++;
    }

    for (r = br.row - lines + 1; r <= br.row; ++r)
    {
	for (c = tl.col; c < br.col+1; ++c)
	{
	    move(r,c);
	    addch(' ' | COLOR_PAIR(colnum));	    
	    continue;
	}
    }
    
    refresh();    
    restore(ps);
}

/*------------------------------------------------------------------------
 *  putchr  --  put character at current cursor position
 *------------------------------------------------------------------------
 */
putchr(ch,count,page)
int ch;
unsigned count,page;
{
    int ps;
    int i;    
    disable(ps);
    for (i = 0; i < count; ++i)
    {
	if (ch != (char)'\n')
	{
	    chtype c = inch();
	    int color = PAIR_NUMBER(c);
	    char chr = c & A_CHARTEXT;
	    addch(ch | COLOR_PAIR(color));
	}	
    }	
    refresh();	
    restore(ps);    
}
