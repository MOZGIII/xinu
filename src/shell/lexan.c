/* lexan.c - lexan, shgetc */

#include <conf.h>
#include <kernel.h>
#include <shell.h>

#define NOCHAR  0x100       /* Value that is not ASCII */
#define ctoi(x) ((int)(unsigned char)(x))         /* put this in kernel.h */

/*------------------------------------------------------------------------
 *  lexan  -  ad hoc lexical analyzer to divide command line into tokens
 *------------------------------------------------------------------------
 */
lexan(str, shptr, prompt)
char    *str;
struct  shvars  *shptr;
char    *prompt;
{
    register int    ch, quote, plevel;
    int             toktype;
    char            *endstr;

    for (ch=shptr->shlastc; ch == ' ' || ch == '\t'
                         || ch == 0  ; ) /* skip white space and ctrl B */
        ch= shgetc(shptr, NULLSTR);
    if ((toktype=ch)=='\n' || toktype==EOF) 
        return(toktype);
    if (plevel=(toktype == '('? 1 : 0))
        ch=shgetc(shptr, NULLSTR);
    endstr = &shptr->shstrbuf[SHSTRLEN - 1];
    quote = NOCHAR;
    for ( ; str<endstr && ch != EOF; ch=shgetc(shptr,shptr->shprcon)){
        if (quote != NOCHAR) {          /* In quotes     */
            if (ch != quote || plevel > 0)
                *str++ = ch;
            if (ch == quote)            /* End quote     */
                quote = NOCHAR;
            continue;
        }
        if (plevel > 0) {               /* In parentheses, not in quotes */
            if (ch == '(')
                ++plevel;
            else if (ch == ')' && --plevel <= 0)
                break;
            *str++ = ch;                /* Gather all chars in parens */
        }
        else {                          /* Not in parentheses or quotes */
            if (ch==' ' || ch == '\t' || ch =='\n' || ch == ';' ||
                ch=='&' || ch == '<'  || ch =='>'  || ch == '^' ||
                ch=='(' || ch == ')'  || ch =='|')
                    break;              /* Terminate token */
            toktype = '"';              /* Otherwise token is a string */
        }
        if (ch=='\'' || ch == '"')      /* Honour quotes everywhere */
            quote = ch;
        else if (plevel == 0)
                *str++ = ch;
    }
    if (toktype != '"')
        shgetc(shptr, NULLSTR);
    *str = NULLCH;
    return(toktype);
}

/*------------------------------------------------------------------------
 *  shgetc  -  get a character from the shell's command input device
 *------------------------------------------------------------------------
 */
 shgetc(shptr, prompt)
 struct shvars  *shptr;
 char           *prompt;
 {
    int     len;

    if (shptr->shnextc < shptr->shend)
        return(shptr->shlastc = ctoi(*shptr->shnextc++));
    if (shptr->shlastc == '\n')
        xinu_write(STDOUT, prompt, xinu_strlen(prompt));
    if ((len=xinu_read(shptr->shcmddev, shptr->shbuf, SHBUFLEN)) <= 0)
        return(shptr->shlastc = EOF);
    shptr->shbuf[len] = '\0';
    shptr->shnextc = shptr->shbuf;
    shptr->shend   = &shptr->shbuf[len];
    return(shptr->shlastc = ctoi(*shptr->shnextc++));
}
