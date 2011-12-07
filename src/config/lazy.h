/* lazy.h - lazy I/O package */

struct lazy {
	int	lazy_flag;	/* =1 if item is avail, =0 otherwise	*/
	int	lazy_val;	/* value returned if item is avail	*/
	int	(*lazy_next)();	/* function to return next value	*/
};

extern	getval();
extern	getnext();
