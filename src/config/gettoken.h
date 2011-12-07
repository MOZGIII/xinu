/* gt.h */

/* return token values */

#define	ERROR_TOK	-1
#define EOF_TOK		0
#define	DEFBRK_TOK	1
#define	STRING_TOK	2
#define	ID_TOK		3
#define	ROUTINE_TOK	4
#define	ADDR_TOK	5
#define	MINUS_TOK	6
#define INT_TOK		7
#define	COLON_TOK	8
#define	PP_TOK		9
#define	ON_TOK		10
#define IS_TOK		11
#define NOREFCT_TOK	12

/* externals */

#define TBUFSIZ		512
#define LBUFSIZ		512

extern	char	tbuf[TBUFSIZ];
extern	int	tbuflen;
extern	int	gettoken();
extern	char	lbuf[LBUFSIZ];
