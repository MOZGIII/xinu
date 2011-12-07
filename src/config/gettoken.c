/* gettoken.c - finite state machine for tokens */

#include <ctype.h>
#include <stdio.h>
#include <lazy.h>		/* lazy I/O declarations		*/
#include <gettoken.h>

/* special ASCII characters */
#define	SPACE	' '
#define	TAB	'\t'
#define NL	'\n'

static
panic(cp);

static int s_init(),s_string(),s_cmt(),
	s_inth(),s_intd(),s_id(),s_idskip(),s_pp(),s_routine();

typedef int (*state_fcn)();

/* state values */
#define EXIT_STATE	-1

static state_fcn state_tbl[] = {
#define	INIT_STATE	0
	s_init,
#define	STRING_STATE	1
	s_string,
#define	CMT_STATE	2
	s_cmt,
#define INTH_STATE	3
	s_inth,
#define	INTD_STATE	4
	s_intd,
#define	ID_STATE	5
	s_id,
#define IDSKIP_STATE	6
	s_idskip,
#define	PP_STATE	7
	s_pp,
#define ROUTINE_STATE	8
	s_routine
#define LAST_STATE	9


};

/* structure for lookup table entry */
struct ttab {
	char *tstr;
	int tval;
};

/* keyword lookup table */
static struct ttab ttab[] = {
	"on",		ON_TOK,
	"is",		IS_TOK,
	"norefct",	NOREFCT_TOK,
	NULL,		ID_TOK
};

char lbuf[LBUFSIZ] = { 0 };
static char *lbufp = lbuf;

char tbuf[512];
int tbuflen;
int tbuferr;
static int tokval;

static
stuffbuf(c)
char c;
{
	if (tbuflen < TBUFSIZ-1)
		tbuf[tbuflen++] = c;
	else
		tbuferr++;
}

static
lookup(ttp,cp)
struct ttab *ttp;
char *cp;
{
	for ( ; ttp->tstr != NULL ; ttp++ )
		if ( strcmp(ttp->tstr,cp) == 0 )
			return(ttp->tval);
	return(ttp->tval);
}

static
int gc()
{
	int	c;
	static	eofflag = 0;

	if ( eofflag )
		return(EOF);
	if ( *lbufp == 0 ) {
		if ( fgets(lbuf,512,stdin) == NULL ) {
			eofflag = 1;
			return(EOF);
		}
/*		printf("%s",lbuf); */
		lbufp = lbuf;
	}
	return(*lbufp++);
}

static struct lazy chario = {
	0,
	0,
	gc
};

#define ch	getval(&chario)
#define advch()	getnext(&chario)

gettoken()
{
	int	state;		/* finite state machine state		*/

	tbuflen = 0;		/* clear out the token buffer		*/
	tbuferr = 0;		/* reset error state			*/
	state=INIT_STATE;
	while ( state != EXIT_STATE ) {	/* process current state	*/
		if ( state < LAST_STATE )
			state=(*state_tbl[state])();
		else
			panic("Improper state");
	}
	tbuf[tbuflen] = 0;
	if (tbuferr == 0)
		return(tokval);
	else
		return(ERROR_TOK);
		
}

/* state entered upon initialization of finite state machine */
static
s_init()
{
	int	c;

	c = ch;
	if ( isalpha(c) || c == '_' ) {
		stuffbuf(c);
		advch();
		return(ID_STATE);
	}
	if ( isdigit(c) && c > '0' ) {
		stuffbuf(c);
		advch();
		return(INTD_STATE);
	}
	switch(c) {
		case SPACE:
		case TAB:
		case NL:
			advch();
			return(INIT_STATE);
		case EOF:
			tokval=EOF_TOK;
			return(EXIT_STATE);
		case '0':
			stuffbuf(c);
			advch();
			c = ch;
			if ( c == 'x' || c == 'X' ) {
				stuffbuf(c);
				advch();
				if ( ishex(ch) )
					return(INTH_STATE);
				tokval=ERROR_TOK;
				return(EXIT_STATE);
			}
			return(INTD_STATE);
		case '#':
			stuffbuf(c);
			advch();
			return(PP_STATE);
		case '-':
			advch();
			c = ch;
			if ( isalpha(c) || c == '_' ) {
				stuffbuf(c);
				advch();
				return(ROUTINE_STATE);
			}
			tokval=ERROR_TOK;
			break;
		case '%':
			tokval=DEFBRK_TOK;
			break;
		case ':':
			tokval=COLON_TOK;
			break;
		case '"':
			stuffbuf(c);
			advch();
			return(STRING_STATE);
		case '/':
			advch();
			if ( ch == '*' ) {
				advch();
				return(CMT_STATE);
			}
			stuffbuf(c);
			tokval=ERROR_TOK;
			return(EXIT_STATE);
		default:
			tokval=ERROR_TOK;
	}
	stuffbuf(c);
	advch();
	return(EXIT_STATE);
}

/* string state processing */
static
s_string()
{
	int	c;

	c = ch;
	if ( c == EOF || c == NL ) {
		tokval = ERROR_TOK;
		return(EXIT_STATE);
	}
	if ( c == '"' ) {
		stuffbuf(c);
		advch();
		tokval = STRING_TOK;
		return(EXIT_STATE);
	}
	advch();
	stuffbuf(c);
	return(STRING_STATE);
}

ishex(c)
int c;
{
	c = tolower(c);
	return( isdigit(c) || ( c>='a' && c<='f' ) );
}

static
s_inth()
{
	int	c;

	c = ch;
	if ( ishex(c) ) {
		stuffbuf(c);
		advch();
		return(INTH_STATE);
	}
	tokval = INT_TOK;
	return(EXIT_STATE);
}

static
s_intd()
{
	int	c;

	c = ch;
	if ( isdigit(c) ) {
		stuffbuf(c);
		advch();
		return(INTD_STATE);
	}
	tokval = INT_TOK;
	return(EXIT_STATE);
}

/* comment processing */
static
s_cmt()
{
	if ( ch == '*' ) {
		advch();
		if ( ch == '/' ) {
			advch();
			return(INIT_STATE);
		}
		return(CMT_STATE);
	}
	advch();
	return(CMT_STATE);
}

/* identifier processing */
static
s_id()
{
	int	c;

	c = ch;
	if ( isalpha(c) || isdigit(c) || c == '_' ) {
		stuffbuf(c);
		advch();
		return(ID_STATE);
	}
	return(IDSKIP_STATE);
}

s_idskip()
{
	int	c;

	c = ch;
	if ( c == '=' ) {
		advch();
		tokval = ADDR_TOK;
		return(EXIT_STATE);
	}
	if ( c == TAB || c == SPACE || c == NL ) {
		advch();
		return(IDSKIP_STATE);
	}
	tbuf[tbuflen] = 0;
	tokval = lookup(ttab,tbuf);
	return(EXIT_STATE);
}

/* routine identifier processing */
static
s_routine()
{
	int	c;

	c = ch;
	if ( isalpha(c) || isdigit(c) || c == '_' ) {
		stuffbuf(c);
		advch();
		return(ROUTINE_STATE);
	}
	tokval = ROUTINE_TOK;
	return(EXIT_STATE);
}

/* preprocessor statements */
static
s_pp()
{
	int	c;

	c = ch;
	if ( c == EOF ) {
		tokval = ERROR_TOK;
		return(EXIT_STATE);
	}
	advch();
	if ( c == NL ) {
		tokval = PP_TOK;
		return(EXIT_STATE);
	}
	stuffbuf(c);
	return(PP_STATE);
}

static
panic(cp)
char *cp;
{
	printf("\n\npanic: %s - program aborted\n",cp);
	exit(1);
}
