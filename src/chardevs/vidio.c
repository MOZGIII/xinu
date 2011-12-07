/*;-------------------------------------------------------------------------
; _wtty  --  put a character to the video display using WTTY call
;-------------------------------------------------------------------------*/
#include <stdio.h>
#include <ncurses.h>
#include <signal.h>
int cursesInit = 0;
extern void int_sig_handler(int sig);
int colors[8][8];
const unsigned int color_num = 8;
static short nc_colors[] = {COLOR_BLACK, COLOR_BLUE, COLOR_GREEN, COLOR_CYAN, COLOR_RED, COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE};

void close_vidio()
{
    endwin();
    cursesInit = 0;
}

void init_vidio()
{
    int i = 0, j = 0;
    initscr();
    cbreak();
    noecho();    
    cursesInit = 1;
    
    
    if (start_color() == OK)
    {    
        printf("%d, %d", COLORS, COLOR_PAIRS);

        xinu_signal(SIGINT, int_sig_handler);
	for (i = 0; i < color_num; ++i)
        {
    	    for (j = 0; j < color_num; ++j)
	    {
		if (i*color_num+j < COLOR_PAIRS)
		    init_pair(i*color_num+j, nc_colors[i], nc_colors[j]);
	    }
	}
    }
}

void wtty(char ch)
{
/*    if (!cursesInit)*/
	write(1, &ch, 1);
/*    else
	addch(ch);*/
}
/*;-------------------------------------------------------------------------
; vidint  --  general access to VID interrupts; returns flags
;-------------------------------------------------------------------------*/
int vidint(union REGS *r)
{
    return 0;
}

void get_cur_yx(int *y, int *x)
{
    getyx(curscr, *y, *x);
}
