/* lexan.c - lexan, shgetc */

#include <conf.h>
#include <kernel.h>
#include <shell.h>
#include <cmd.h>

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

// Does all the magic on autocompletion
char* calculate_autocomplete(char * buff, int len) {
  int com, matched_command;

  // Find command in commands list
  matched_command = -1;
  for (com = 0 ; com < ncmds ; com++) {
    if (strncmp(cmds[com].cmdnam, buff, len)==0) {
      // Only one match allowed!
      if(matched_command > -1)
        return NULL;
      // Save matched command index
      matched_command = com;
    }
  }

  // No matches found!
  if(matched_command == -1)
    return NULL;

  // Matched command found!
  // Shift it's name to len...
  return cmds[matched_command].cmdnam + len;
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
    int     i;
    char    *ac;

    if (shptr->shnextc < shptr->shend)
        return(shptr->shlastc = ctoi(*shptr->shnextc++));
    if (shptr->shlastc == '\n')
        xinu_write(STDOUT, prompt, xinu_strlen(prompt));
    if ((len=xinu_read(shptr->shcmddev, shptr->shbuf, SHBUFLEN)) <= 0)
        return(shptr->shlastc = EOF);

    /* Fetch tabs */

    if (xinu_strlen(shptr->shbuf) > 1 && shptr->shbuf[xinu_strlen(shptr->shbuf)-2] == '\t') {
      // Lets see if we can autocomplete command...
      ac = calculate_autocomplete(shptr->shbuf, len-2);
      if (ac) {
        // Very durty code here... but who cares...
        // it's not even a serious task...

        xinu_write(STDOUT, prompt, xinu_strlen(prompt));
        len -= 2;
        xinu_write(STDOUT, shptr->shbuf, len);
        xinu_write(STDOUT, ac, xinu_strlen(ac));
        xinu_write(STDOUT, " ", xinu_strlen(" "));
        strcpy(shptr->shbuf + len, ac);
        len += xinu_strlen(ac);
        strcpy(shptr->shbuf + len, " ");
        len += 1;
      } // else do nothing!
    }


    shptr->shbuf[len] = '\0';
    shptr->shnextc = shptr->shbuf;
    shptr->shend   = &shptr->shbuf[len];

    return(shptr->shlastc = ctoi(*shptr->shnextc++));
}
