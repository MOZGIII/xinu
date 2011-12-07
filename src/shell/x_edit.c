/* x_edit.c - x_edit */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>
#include <mem.h>
#include <disk.h>
#include <file.h>
#include <mffile.h>
#include <edit.h>

#define STATUS getdev("status")

#define  Rows ep->rows
#define  Cols ep->cols
#define  Dev  ep->dev
#define  Src  ep->src
#define  R    ep->r
#define  C    ep->c
#define  Sr   ep->sr
#define  Cps  ep->cps
#define  Eof  ep->eof
#define  Bufsize ep->bufsize

LOCAL insertc(char* buf, int bi, int ch, struct edvars *ep);
LOCAL deletec(char* buf, int bi, struct edvars *ep);
LOCAL insertl(char* buf, int bi,struct edvars *ep);
LOCAL save(char* buf, char *filename, struct edvars *ep);
LOCAL pos(int r,int c, struct edvars *ep);
/*------------------------------------------------------------------------
 *  x_edit  -  (command edit) edit a small text file
 *------------------------------------------------------------------------
 */
COMMAND x_edit(nargs, args)
int	nargs;
char    *args[];
{
    struct  edvars *ep;
    int    ch;
    char    *buf;
    char    filename[80];
    int     i, j, tl, insert, modified;
    int     show();
    int     pos();
    struct  tty *iptr;
    struct  mfblk *mfptr;

    if (nargs > 2) {
        xinu_fprintf(STDERR,"usage: edit [filename]\n");
        return(SYSERR);
    }
    if ((ep=(struct edvars*)xmalloc((word) sizeof(struct edvars)))==NULL) {
        xinu_fprintf(STDERR,"edit: not enough memory\n");
		return(SYSERR);
	}
    if (isatty(Dev=getstdo()) && isatty(getstdi())) {
        iptr = &tty[Dev];
        Rows=iptr->rowsiz;
        Cols=iptr->colsiz;
        Cps=Rows*Cols;
    }
    else {
        xinu_fprintf(STDERR,"STDIN and STDOUT must be TTY devices\n");
        return OK;
    }
    Src=SYSERR;
    insert = TRUE;
    modified = FALSE;
    Eof = FALSE;
    Bufsize = 12*1024;         /* increase if desired */
    if (nargs == 2) {
        xinu_strcpy(filename, args[1]);
        while ((Src = xinu_fopen(filename, "rw")) == SYSERR) {
            xinu_fprintf(STDERR,"file %s not found\n",args[1]);
            xinu_printf("filename: ");
            scanf("%s",filename);
            if (filename[0] == '\n') return OK;
            Src = xinu_fopen(filename,"w");
            if (Src < 0) xinu_printf("couldn't open %s\n",filename);
        }
    }
    if ((buf=xmalloc(Bufsize)) == NULL) {
        xinu_fprintf(STDERR,"edit: not enough memory\n");
        return(SYSERR);
    }
    for (i=0; i<Bufsize; i++) buf[i]=' ';   /* clear buf */
    if (Src > 0) {
        for (i=0 ; i<Bufsize ; i++) {       /* normalize */
            ch = getc(Src);
            switch (ch) {
                case '\n':  i=(i/Cols+1)*Cols;
                case '\r':  i-=1;
                            break;
                case TAB:   i=i+TABS-i%TABS-1;
                            break;
                case EOF:   ch = EOFC;
                            Eof=i;
                default:    buf[i] = ch;
                            if (ch==EOFC) goto edit;
            }
        }
        if (!Eof) {
            xinu_fprintf(STDERR,"file %s too large\n",filename);
            return OK;
            }
        }
    else {                                  /* must be a new file */
            buf[0]=EOFC;
            Eof = 0;
        }
        
/* editing loop: r is row within buf, sr is screen row */
edit:
    control(Dev,TCMODER);               /* put console in raw mode */
    R=C=Sr=tl=0;                /* clear location markers  */
    control(Dev,TCCLEAR);               /* clear the window        */
    show(buf, R, Rows);             /* display a screen full   */
    while ((ch=getc(Dev)) != ESC) {
        if (ch > 0) {
            switch (ch) {
            case '\r':  if (insert) {insertl(buf,R*Cols+C,ep);modified = TRUE;}
                        C = 0;
                        if (((R+1)*Cols) > Eof) break;
                        if (Sr == Rows-1) {scrolld(buf,R++,ep);tl++;}
                        else {pos(++Sr,C,ep);R+=1;show(&buf[tl*Cols],0,Rows);}
                        break;
            case '\t':  i=(R*Cols+C)%TABS;
                        do {
                        if (insert && (C < Cols-1)) 
                            {insertc(buf,R*Cols+C,' ',ep);modified=1;}
                        C = ((C==Cols-1) ? C : C+1);
                        } while (++i != TABS);
                        show(&buf[tl*Cols], Sr, 1);    
                        pos(Sr,C,ep);
                        break;
            case CTRLS: xinu_fprintf(STATUS,"\n Filename: %s %c [%d:%d] mem %d",
                                filename,(insert ? 'I':'A'),C,R,Bufsize-Eof);
                        break;
            case CTRLY: if (R != (Eof/Cols))
                            for (i=R*Cols; i<R*Cols+Cols; i++) buf[i]=' ';
                        C=0;
                        deletec(buf,R*Cols,ep);
                        modified=TRUE;          /* keep going into case \b */
            case '\b':  if (C!=0) {deletec(buf,R*Cols+C-1,ep);modified=1;}
                        show(&buf[tl*Cols], Sr, Rows-Sr);
                        C=(C==0 ? C : C-1);
                        pos(Sr,C,ep);
                        break;
            default:    if (insert) {if (insertc(buf,R*Cols+C,ch,ep)) C=C+1;}
                        else if((R*Cols+C) < Eof)
                            {buf[R*Cols+C]=ch;C=(C==Cols-1 ? C:C+1);}
                        show(&buf[tl*Cols], Sr, 1);
                        pos(Sr,C,ep);
                        modified = TRUE;
            }
        }
        else {
              switch (ch) {
              case HOME:    C=0;
                            break;
              case END:     for (C=Cols-1;C>=0;C--)
                                if (buf[R*Cols+C] != ' ') break;
                            if (C==(Cols-1)) break;
                            if (C==0) break;
                            C += 1;
                            break;
              case INS:     insert = !insert;
                            break;
              case DEL:     if ((R*Cols+C) >= Eof) break;
                            if (C == (Cols-1)) buf[R*Cols+C] = ' ';
                            else deletec(buf, R*Cols+C,ep);
                            show(&buf[tl*Cols],Sr,Rows-Sr);
                            modified = TRUE;
                            break; 
              case PGUP:    for (i=0;(i<Rows)&&(tl>0);i++) {R--; tl--;}
                            show(&buf[tl*Cols],0,Rows);
                            if (i<Rows) { Sr=0; R=0; }
                            break;
              case PGDN:    for (i=0;(i<Rows)&&(R<Eof/Cols);i++) {R++; tl++;}
                            show(&buf[tl*Cols],0,Rows);
                            if(R==(Eof/Cols)) C=Eof%Cols;
                            break;
              case CSRU:    if (R==0) break;
                            R -= 1;
                            if (Sr==0) {show(&buf[R*Cols],0,Rows);tl--; }
                            else Sr -= 1;
                            break;
              case CSRD:    if ((R+1)*Cols+C >= Eof) C = Eof%Cols;
                            if ((R*Cols+C)==Eof) break;
                            if (Sr==Rows-1) { scrolld(buf,R,ep); tl++;}
                            else Sr=Sr+1;
                            R=R+1; 
                            break;
              case CSRL:    if (C==0) break;
                            C -= 1;
                            break;
              case CSRR:    if (R*Cols+C >= Eof) break;
                            if (C==Cols-1) break;
                            C += 1;
                            break;
             }
             pos(Sr,C,ep);
        }
    }
    if (!Eof) buf[(R*Cols-1)+C+1] = EOFC;
    control(Dev,TCCLEAR);
    control(Dev,TCMODEC);
    if (modified) {
        xinu_printf("save file ? (Y/N): ");
        ch = getc(Dev);
        if (ch == 'n') return OK;
        while (Src < 0) {
            xinu_printf("filename: ");
            scanf("%s",filename);
            if (filename[0] == '\n') return OK;
            xinu_close(Src);
            Src = xinu_fopen(filename, "w");
            if (Src < 0) xinu_printf("couldn't open %s\n",filename);
        }
        xinu_close(Src);
        control(DOS,FLREMOVE,filename);
        Src = xinu_fopen(filename, "w");
        save(buf,filename,ep);
    }        
    return OK;
}

int scrolld(buf, r, ep)
char *buf;
int  r;
struct edvars *ep;
{
    int i;
    
    i = r*Cols-(Rows-2)*Cols;
    show(&buf[i],0,Rows);
    return OK;
}
    
int   show(buf, r, n)       /* buffer pointer, starting row, no. of rows */
char    *buf;
int     r, n;               /* r=logical row, n=number of lines to write */
{
    int c, bi, cols, dev, ps;
    int oldy, oldx;
    struct tty *iptr;

    if (isatty(dev=getstdo()) ) {
        iptr = &tty[dev];
        cols=iptr->colsiz;
    } else return SYSERR;
    
    disable(ps);
    bi = r*cols;
    r = r+iptr->topleft.row;        /* border adjustment already done */
    n = r + n;                      /* r and n are now physical */
    
    get_cur_yx(&oldy, &oldx);
    for (; r<n; r++) 
        for (c=iptr->topleft.col; c<cols ; c++) 
	{
	    mvaddch(r,c, buf[bi++] | COLOR_PAIR(7));
/*	    SLsmg_gotorc(r,c);
	    SLsmg_write_char(buf[bi++]);
	    SLsmg_refresh();*/

	}
    move(oldy, oldx);
    refresh();    
    restore(ps);
    return OK;        
}
LOCAL pos(r,c,ep)
int     r,c;
struct edvars *ep;
{
    char    curp[16]="0,0";

    xinu_sprintf(curp,"%d,%d",c,r);
    if (control(Dev,TCCURPOS,curp) == SYSERR) {
        xinu_fprintf(STDERR,"pos: could not position cursor at %s\n",curp);
		return(SYSERR);
	}
	return(OK);
}
LOCAL save(buf, filename, ep)
char    *buf;
char    *filename;
struct edvars *ep;
{
    int     r, c, i;
    char    temp[129];
       
    seek(Src, 0L);

    for (r=0; r < Bufsize/Cols ; r++) {
        c = Cols-1;
        while ((buf[r*Cols+c]) == ' ') c -= 1;
        for (i = 0 ; i <= c ; i++) temp[i] = buf[r*Cols+i];
        if (temp[i-1] == EOFC) {
            i -=1;
            temp[i] = '\n';
            if (i%Cols != 0) xinu_write(Src, temp, i+1);
            break;
        } else {
            temp[i] = '\n';
            xinu_write(Src,temp,i+1);
        }
    }
    xinu_fprintf(STDERR,"wrote %d lines to %s\n", r, filename);    
    return OK;
}
LOCAL insertc(buf, bi, ch, ep)
char *buf;
int ch, bi;
struct edvars *ep;
{
    int i, c;
    
    if (bi%Cols==Cols-1) {putc(STDOUT,BELL); return 0;} /* can't insert */
    i = (bi/Cols+1)*Cols-1;             /* index to end of current line */
    while (buf[i] == ' ') i -= 1;       /* back up to last char (or EOFC) */
    if (i==(bi/Cols+1)*Cols-1) {putc(STDOUT,BELL); return 0;}/* can't insert */
    for (c=i+1; c > bi; c--) buf[c] = buf[c-1];
    buf[bi] = ch;
    if (Eof/Cols == bi/Cols) Eof += 1; /* incr eof if on same line as bi */
    return TRUE;
} 
LOCAL deletec(buf,bi, ep)
char *buf;
int bi;
struct edvars *ep;
{
    int i,j,c, empty;
    
    empty=TRUE;                         /* empty line is checked later */    
    i = bi-bi%Cols+Cols-1;              /* index to end of current line */
    if (bi == Eof) return;              /* can't delete eof */
    for (j=i-Cols+1; j<=i; j++) if (buf[j]!=' ') empty=FALSE;
    if ((bi%Cols) != 0) empty=FALSE;
    if (empty) {                        /* delete whole line */
        for (c=bi; c<=Eof; c++) {buf[c]=buf[c+Cols];buf[c+Cols]=' ';}
        Eof=Eof-Cols;
        return;
    }
    for (c=bi; c < i; c++) {
        buf[c] = buf[c+1];
        if (c%Cols==(Cols-2)) buf[c+1]=' ';  /* don't drag last char along */
    }
    if (Eof/Cols == bi/Cols) Eof -= 1; /* decr eof if on same line as bi */
    return;
} 
LOCAL insertl(buf,bi,ep)
char *buf;
int  bi;
struct edvars *ep;
{
    int i, j;
    
    if(Eof >= (Bufsize-Cols)) {putc(STDOUT,BELL); return;} /* can't insert */
    i = bi-bi%Cols+Cols;   /* index to start of line after insertion point */
    if (bi==Eof) {buf[bi]=' ';buf[i]=EOFC; Eof=i;return;}
    for (j=Eof;j>=i;j--) buf[j+Cols]=buf[j]; /* move text down 1 line */
    Eof=Eof+Cols;                            /* eof has moved */
    for (j=i; j<i+Cols-1; j++) buf[j]=' ';   /* clear the inserted line */
    for (j=bi; j < bi-bi%Cols+Cols ;j++) {   /* copy to next line */
        buf[i++]=buf[j];
        buf[j]=' ';
    }
    while ((bi % Cols) != 0) buf[bi++]=' ';  /* clear rest of old line */
    return;
}
