/* edit.h */

#define COLOR                   /* define COLOR or MONO for Hercules */

typedef struct texels {         /* text elements for direct screen output */
    unsigned char ch;
    unsigned char attr;
}  texel;

typedef texel scr[25][80];   /* screen array */

/* mono cards */

#ifdef MONO
scr *scrp = (scr *) 0xb0000000L;
#endif

/* any color card */

#ifdef COLOR
scr *scrp = (scr *) 0xb8000000L;
#endif

#define screen (*scrp)

#define INS     0xffd2          /* insert       key */
#define DEL     0xffd3          /* delete       key */
#define HOME    0xffc7          /* home         key */
#define END     0xffcf          /* end          key */
#define PGUP    0xffc9          /* page up      key */
#define PGDN    0xffd1          /* page down    key */
#define CSRU    0xffc8          /* cursor up    key */
#define CSRD    0xffd0          /* cursor down  key */
#define CSRL    0xffcb          /* cursor left  key */
#define CSRR    0xffcd          /* cursor right key */

#define CTRLS   0x13            /* control-s show status */
#define CTRLY   0x19            /* control-y delete line */
#define UP      0
#define DOWN    1
#define TABS    4               /* TAB size */

struct  edvars  {               /* editor global variables */
        int     rows;           /* this screen's number of rows */
        int     cols;           /* this screen's number of cols */
        int     dev;            /* editor device */
        int     src;            /* src file */
        int     r;              /* current row in buffer */
        int     c;              /* current col in buffer */
        int     sr;             /* screen row */
        int     cps;            /* chars per screen */
        int     eof;            /* eof position */
        int     bufsize;        /* size of file buffer */

};
