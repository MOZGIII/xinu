/* dos.h */

struct WORDREGS	{
	unsigned int ax, bx, cx, dx, si, di, cflag, flags;
};

struct BYTEREGS	{
	unsigned char al, ah, bl, bh, cl, ch, dl, dh;
};

union	REGS {
	struct	WORDREGS x;
	struct	BYTEREGS h;
};

struct	SREGS	{
	unsigned int es, cs, ss, ds;
};

#define MK_FP(seg,ofs)  ((void*)(((unsigned long)(seg)<<16) | (unsigned)(ofs)))
#define FP_SEG(fp)      ((unsigned)((unsigned long)(fp) >> 16))
#define FP_OFF(fp)      ((unsigned)(fp))
