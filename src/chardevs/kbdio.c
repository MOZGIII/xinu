#include <termios.h>
#include <unistd.h>
#include "/usr/include/fcntl.h"
#include <sys/select.h>
#include <ncurses.h>

static struct termios initial_settings, new_settings;
static int peek_character = -1;

fd_set pollset;
int dbg_fd;

void init_keyboard()
{
        dbg_fd = open("key.log", O_RDWR|O_CREAT, 00777);
}

void close_keyboard()
{
    close(dbg_fd);
}

int kbhit()
{
    struct timeval tv;
    int ret = 0;
    
    FD_ZERO(&pollset);
    FD_SET(STDIN_FILENO, &pollset);

    tv.tv_usec = 1;
    tv.tv_sec = 0;
    ret = select(1, &pollset, NULL, NULL, &tv);
    return ret;
}

int kbdgetc()
{
    int ch=0;
    int ps = sys_disabl();
    if (kbhit())
    {
	int n = read(STDIN_FILENO, &ch, 4);
	if (n > 0)
	{
	    /* Keylogger :) */
/*	    if (ch != 0)
	    {
		dprintf(dbg_fd, "n=%i Key '%c' (%i)\n", n, (char)ch, ch);
	    }*/
	    
	    sys_restor(ps);

    	    return ch;    	
	}
    }
    sys_restor(ps);    
    return -1;
}

int kbdint(int r)
{
    return 0;
}
