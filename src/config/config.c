/* config.c - config */

#include <stdio.h>
#include <ctype.h>
#include <gettoken.h>
#include <lazy.h>

#define CFILE	"../kernel/conf.c"	/* configuration source file	*/
#define HFILE	"../h/conf.h"		/* configuration header file	*/
#define PFILE	"pcxconf"		/* configuration definition	*/

#define NSZ	14			/* fixed name size		*/
#define MAXW	64			/* max word size		*/
#define	NHASH	97			/* hash table size		*/
#define NSYMS	128			/* max number of i/o routines	*/
#define NIO	11			/* no. of routines per type	*/
#define NVEC	3			/* no. of "vectors" per type	*/
#define DVNMLN	12			/* max chars per device name	*/

#define NL	'\n'
#define TAB	'\t'
static
attr_list(dvtp);


struct symentry {
	struct	symentry	*hlink;
	char	symnam[NSZ];
};
typedef struct symentry SYM;

struct dvclass {
	char dvcnam[NSZ];
	int dvcnum;
	struct dvtyp *dvtlist;
	struct dvclass *dvcnext;
};
typedef struct dvclass DVC;

struct dvtyp {
	char dvtnam[NSZ];
	int dvrefct;
	SYM *ioroutine[NIO];
	char iovec[NVEC][MAXW];
	char name[DVNMLN];
	char *ioblk;
	struct dvtyp *dvtnext;
};
typedef struct dvtyp DVT;

char *iotab[] = { "i","o","c","r","w","s","g","p","n","iint","oint",NULL };
char *addrtab[] = { "port","ivec","ovec",NULL };

#define NEW(typ)	(typ *)( malloc (sizeof (typ)))

static struct lazy tokio = {
	0,
	0,
	gettoken
};

#define	tok		getval(&tokio)
#define	advtok()	getnext(&tokio)

int	ndevs = 0;			/* number of devices		*/
int	nlines = 0;
int	nsyms = 0;			/* number of symbols		*/
SYM	*htab[NHASH];			/* hash table			*/
SYM	symtab[NSYMS];			/* symbol table			*/
DVC	*dvclist = NULL;		/* device class list		*/
SYM	*ioerr;				/* io error routine entry	*/
int	vflag = 0;			/* verbose flag			*/

extern	char	*malloc();

int	ifflag = 0;
FILE	*cfp = NULL;			/* conf.c output file		*/
FILE	*hfp = NULL;			/* conf.h output file		*/

char errstr[128];

/*------------------------------------------------------------------------
 *  config  --  program to configure PC-Xinu
 *------------------------------------------------------------------------
 */
main(argc,argv)
int argc;
char *argv[];
{
	int	i;
	SYM	*enter();

	argc--; argv++;
	while ( argc ) {
		if ( **argv == '-' ) {
			switch (*++*argv) {
			case 'v':
				vflag++;
				break;
			case 'c':
				argc--; argv++;
				if ( (cfp=fopen(*argv,"w")) == NULL ) {
					sprintf(errstr,"can't open %s for output",*argv);
					err(errstr);
				}
				break;
			case 'h':
				argc--; argv++;
				if ( (hfp=fopen(*argv,"w")) == NULL ) {
					sprintf(errstr,"can't open %s for output",*argv);
					err(errstr);
				}
				break;
			case 'f':
				argc--; argv++;
				ifflag = 1;
				if ( strcmp(*argv,"-") == 0 )
					break;
				if ( (freopen(*argv,"r",stdin)) == NULL ) {
					sprintf(errstr,"can't open %s for input",*argv);
					err(errstr);
				}
				break;
			default:
				sprintf(errstr,"'%c': unknown flag",**argv);
				err(errstr);
			}
			argc--; argv++;
			continue;
		} else
			err("usage: config [-v] [-f ifile] [-c cfile] [-h hfile]");
	}
	if ( ifflag == 0 && (freopen(PFILE,"r",stdin)) == NULL ) {
		sprintf(errstr,"can't open %s for input",PFILE);
		err(errstr);
	}
	if ( cfp == NULL && (cfp=fopen(CFILE,"w")) == NULL ) {
		sprintf(errstr,"can't open %s for output",CFILE);
		err(errstr);
	}
	if ( hfp == NULL && (hfp=fopen(HFILE,"w")) == NULL ) {
		sprintf(errstr,"can't open %s for output",HFILE);
		err(errstr);
	}
	for ( i=0 ; i<NHASH ; i++ )
		htab[i] = NULL;
	ioerr = enter("ioerr");
	classes();
	devices();
	cleanup();
	exit(0);
}

strmcpy(dest,src,max)
char *dest,*src;
int max;
{
	char	*srcsav;
	int	maxsav;

	srcsav = src;
	maxsav = max;
	if ( max == 0 )
		err("strmcpy: illegal function call");
	while ( (*dest++ = *src++) != 0 && --max != 0 )
		;
	if ( max == 0 ) {
		*dest=0;
		fprintf(stderr,
			"config: WARNING: string '%s' truncated to %d chars\n",
			srcsav,maxsav);
	}
}

int hash(cp)
char *cp;
{
	int	h;

	for ( h=0 ; *cp ; cp++ )
		h += *cp;
	h %= NHASH;
	while ( h < 0 )
		h += NHASH;
	return(h);
}

/* enter an identifier in the symbol table & return its pointer */
SYM *enter(cp)
char *cp;
{
	int	h;
	SYM	*symp;

	h = hash(cp);
	for ( symp = htab[h] ; symp != NULL ; symp=symp->hlink )
		if ( strncmp(symp->symnam,cp,NSZ-1) == 0 )
			return(symp);
	if ( nsyms >= NSYMS )
		err("symbol table overflow");
	symp = &symtab[nsyms++];
	symp->hlink = htab[h];
	strmcpy(symp->symnam,cp,NSZ);
	htab[h] = symp;
	return(symp);
}

/* lookup string in table - return index, or -1 if not there */
int lookup(tab,cp)
char **tab;
char *cp;
{
	int	i;

	for ( i=0 ; *tab != NULL ; i++,tab++ )
		if ( strcmp(*tab,cp) == 0 )
			return(i);
	return(-1);
}

/* get a new DVC entry and link to dvclist */
DVC *newdvc(nam)
char *nam;
{
	DVC	*dvcp,*dvcpp;

	dvcpp = NULL;
	for ( dvcp=dvclist ; dvcp != NULL ; dvcp=dvcp->dvcnext ) {
		dvcpp = dvcp;
		if ( strcmp(dvcp->dvcnam,nam) == 0 ) {
			sprintf(errstr,"%s: duplicate device class name",nam);
			err(errstr);
		}
	}
	if ( (dvcp=NEW(DVC)) == NULL )
		err("not enough memory");
	dvcp->dvcnum = 0;
	dvcp->dvtlist = NULL;
	strmcpy(dvcp->dvcnam,nam,NSZ);
	dvcp->dvcnext = NULL;
	if ( dvcpp == NULL )
		dvclist = dvcp;
	else
		dvcpp->dvcnext = dvcp;
	return(dvcp);
}

/* get a new DVT entry and link it to device class list dvcp->dvtlist */
DVT *newdvt(dvcp,nam)
DVC *dvcp;
char *nam;
{
	int	i;
	DVT	*dvtp,*dvtpp;

	dvtpp = NULL;
	for ( dvtp=dvcp->dvtlist ; dvtp != NULL ; dvtp=dvtp->dvtnext ) {
		dvtpp = dvtp;
		if ( strcmp(dvtp->dvtnam,nam) == 0 ) {
			sprintf(errstr,"%s: duplicate device type name",nam);
			err(errstr);
		}
	}
	if ( (dvtp=NEW(DVT)) == NULL )
		err("not enough memory");
	strmcpy(dvtp->dvtnam,nam,NSZ);
	for ( i=0 ; i<NIO ; i++ )
		dvtp->ioroutine[i] = ioerr;
	dvtp->ioblk = "NULLPTR";
	for ( i=0 ; i<NVEC ; i++ )
		strcpy(dvtp->iovec[i],"0");
	strcpy(dvtp->name,"\"\"");
	dvtp->dvrefct = 0;
	dvtp->dvtnext = NULL;
	if ( dvcp->dvtlist == NULL )
		dvcp->dvtlist = dvtp;
	else
		dvtpp->dvtnext = dvtp;
	return(dvtp);
}

static
match(t)
int t;
{

	if ( t != tok ) {
		sprintf(errstr,"syntax error - expecting token %d",t);
		err(errstr);
	}
	advtok();
}
		
classes()
{
	DVC	*dvcp;
	DVT	*dvtp;

	dvcp = NULL;
/*	printf("-- class definition section --\n"); */
	while ( tok != DEFBRK_TOK ) {
		match(ID_TOK);
		dvcp = newdvc(tbuf);
		match(COLON_TOK);
		if ( tok == ON_TOK ) {
			do {
				match(ON_TOK);
				match(ID_TOK);
				dvtp = newdvt(dvcp,tbuf);
				attr_list(dvtp);
			} while ( tok == ON_TOK );
		} else {
			dvtp = newdvt(dvcp,"");
			attr_list(dvtp);
		}
	}
	match(DEFBRK_TOK);
}

static
attr_list(dvtp)
DVT *dvtp;
{
	int	i;
	int	t;
	char	*cp;

	while (1) {
		t = tok;
		switch (t) {
		case NOREFCT_TOK:
			match(NOREFCT_TOK);
			dvtp->dvrefct = -1;
			break;
		case ROUTINE_TOK:
			match(ROUTINE_TOK);
	/*		printf("i/o routine: %s",tbuf); */
			if ( (i=lookup(iotab,tbuf)) < 0 ) {
				sprintf(errstr,"%s: unknown i/o routine flag",
					tbuf);
				err(errstr);
			}
			match(ID_TOK);
	/*		printf(" %s\n",tok); */
			dvtp->ioroutine[i] = enter(tbuf);
			break;
		case ADDR_TOK:
			match(ADDR_TOK);
			if ( strcmp("name",tbuf) == 0 ) {
				t = tok;
				if ( t == STRING_TOK || t == ID_TOK ) {
					match(t);
					strmcpy(dvtp->name,tbuf,DVNMLN);
					break;
				}
				sprintf(errstr,"%s: string or id expected",
					tbuf);
				err(errstr);
			}
			if ( strcmp("ioblk",tbuf) == 0 ) {
				t = tok;
				if ( t == STRING_TOK || t == ID_TOK) {
					match(t);
					if ( (cp=malloc(tbuflen+1)) == NULL )
						err("not enough memory");
					strcpy(cp,tbuf);
					dvtp->ioblk = cp;
					break;
				}
				sprintf(errstr,"%s: string or id expected",
					tbuf);
				err(errstr);
			}
			if ( (i=lookup(addrtab,tbuf)) < 0 ) {
				sprintf(errstr,"%s: unknown address key",
					tbuf);
				err(errstr);
			}
			t = tok;
			if ( t == INT_TOK || t == STRING_TOK || t == ID_TOK ) {
				match(t);
				cp = tbuf;
				if ( t == STRING_TOK ) {
					/* kill closing & opening quotes */
					tbuf[tbuflen-1] = 0;
					cp++;
				}
				strmcpy(dvtp->iovec[i],cp,MAXW);
				break;
			}
			sprintf(errstr,"%s: integer, string or id expected",
				tbuf);
			err(errstr);
			break;
		default:
			return;
		}
	}
}

/* look up device class name */
DVC *getdvc(nam)
char *nam;
{
	DVC	*dvcp;

	for ( dvcp=dvclist ; dvcp != NULL ; dvcp=dvcp->dvcnext )
		if ( strcmp(dvcp->dvcnam,nam) == 0 )
			return(dvcp);
	return(NULL);
}

/* look up device type name */
DVT *getdvt(dvcp,nam)
DVC *dvcp;
char *nam;
{
	DVT	*dvtp;

	dvtp=dvcp->dvtlist;
	/* special case of empty name & only one device type in class */
	if ( xinu_strlen(nam) == 0 ) {
		if ( dvtp != NULL && dvtp->dvtnext == NULL )
			return(dvtp);
		return(NULL);
	}
	for ( ; dvtp != NULL ; dvtp=dvtp->dvtnext )
		if ( strcmp(dvtp->dvtnam,nam) == 0 )
			return(dvtp);
	return(NULL);
}

char	confc1[] =
"/* conf.c */\n\
/* (GENERATED FILE: DO NOT EDIT) */\n\
\n\
#include <conf.h>\n";

char	confc2[] =
"\n\
/* device independent I/O switch */\n\
\n\
struct	devsw	devtab[NDEVS] = {\n\
\n\
/*------------------------------------------------------------------------\n\
 * Format of each entry is:\n\
 *\n\
 * dev number, dev name, ref count,\n\
 * init, open, close,\n\
 * read, write, seek,\n\
 * getc, putc, cntl,\n\
 * port addr, dev input vector, dev output vector,\n\
 * input interrupt routine, output interrupt routine,\n\
 * dev i/o block, minor dev number\n\
 *------------------------------------------------------------------------\n\
 */\n";

char confh[] = 
"/* conf.h */\n\
/* (GENERATED FILE; DO NOT EDIT) */\n\
\n\
#define	NULLPTR	(char *)0\n\
\n\
/* Device table declarations */\n\
struct	devsw	{			/* device table entry		*/\n\
	int	dvnum;\n\
	char	dvnam[10];\n\
	int	dvrefct;\n\
	int	(*dvinit)();\n\
	int	(*dvopen)();\n\
	int	(*dvclose)();\n\
	int	(*dvread)();\n\
	int	(*dvwrite)();\n\
	int	(*dvseek)();\n\
	int	(*dvgetc)();\n\
	int	(*dvputc)();\n\
	int	(*dvcntl)();\n\
	int	dvport;\n\
	int	dvivec;\n\
	int	dvovec;\n\
	int	(*dviint)();\n\
	int	(*dvoint)();\n\
	char	*dvioblk;\n\
	int	dvminor;\n\
	};\n";

static	DVT	dvt;			/* copy of entry		*/
char	dvn[MAXW];			/* device name			*/

devices()
{
	int	i;
	DVC	*dvcp;
	DVT	*dvtp;

	fputs(confc1,cfp);
	while ( tok == PP_TOK ) {
		match(PP_TOK);
		fputs(tbuf,cfp);
		putc(NL,cfp);
	}
	fputs(confc2,cfp);
	fputs(confh,hfp);
	fprintf(hfp,"\nextern\tstruct\tdevsw devtab[];");
	fprintf(hfp,"\t\t/* one entry per device\t\t*/\n\n");
	fprintf(hfp,"/* Device name definitions */\n\n");
	while ( tok != DEFBRK_TOK ) {
		match(ID_TOK);
		strmcpy(dvn,tbuf,MAXW);
		match(IS_TOK);
		match(ID_TOK);
		if ( (dvcp=getdvc(tbuf)) == NULL ) {
			sprintf(errstr,"can't find device class %s",tbuf);
			err(errstr);
		}
		if ( tok == ON_TOK ) {
			match(ON_TOK);
			match(ID_TOK);
			dvtp = getdvt(dvcp,tbuf);
		} else
			dvtp = getdvt(dvcp,"");
		if ( dvtp == NULL ) {
			sprintf(errstr,"%s: can't find device type",tbuf);
			err(errstr);
		}
		dvt = *dvtp;		/* make the copy		*/
		attr_list(&dvt);
		if ( ndevs )
			fprintf(cfp,",\n");
		fprintf(cfp,"\n/* %s is %s",dvn,dvcp->dvcnam);
		if ( xinu_strlen(dvt.dvtnam) )
			fprintf(cfp," on %s",dvt.dvtnam);
		fprintf(cfp," */\n%d,%s,%d,",ndevs,dvt.name,dvt.dvrefct);
		for ( i=0 ; i<9 ; i++ )
			fprintf(cfp,"%s%s,",(i%3)?"":"\n",
				dvt.ioroutine[i]->symnam);
		fprintf(cfp,"\n");
		for ( i=0 ; i<NVEC ; i++ )
			fprintf(cfp,"%s,",dvt.iovec[i]);
		fprintf(cfp,"\n");
		for ( i=9 ; i<NIO ; i++ )
			fprintf(cfp,"%s,", dvt.ioroutine[i]->symnam);
		fprintf(cfp,"\n%s,%d",dvt.ioblk,dvcp->dvcnum);
		if ( strcmp(dvn,"GENERIC") != 0 )
			fprintf(hfp,"#define\t%s\t%d\t\t/* class %s\t*/\n",
				dvn,ndevs,dvcp->dvcnam);
		dvcp->dvcnum++;
		ndevs++;
	}
	match(DEFBRK_TOK);
	fprintf(cfp,"\n};\n");
	fprintf(hfp,"\n");
}

cleanup()
{
	int	c,cc;
	int	i;
	DVC	*dvcp;

	fprintf(hfp,"/* Control block sizes */\n\n");
	for ( dvcp=dvclist ; dvcp != NULL ; dvcp = dvcp->dvcnext )
		if ( dvcp->dvcnum > 0 )
			fprintf(hfp,"#define\tN%s\t%d\n",
				dvcp->dvcnam,dvcp->dvcnum);
	fprintf(hfp,"\n#define\tNDEVS\t%d\n\n",ndevs);
	fprintf(hfp,"/* Declarations of I/O routines referenced */\n\n");
	for ( i=0 ; i<nsyms ; i++ )
		fprintf(hfp,"extern\tint\t%s();\n",symtab[i].symnam);
	fprintf(hfp,"\n");
	while ( fgets(lbuf,LBUFSIZ,stdin) != NULL )
		fputs(lbuf,hfp);
}

err(cp)
char *cp;
{
	fprintf(stderr,"config: %s\n",cp);
	exit(1);
}

/* dump the device table list */
dumplist()
{
	DVC	*dvcp;
	DVT	*dvtp;
	int	i;

	for ( i=0 ; i<nsyms ; i++ )
		printf("%s()\n",symtab[i].symnam);
	for ( dvcp=dvclist ; dvcp != NULL ; dvcp = dvcp->dvcnext ) {
		printf("%s:\n",dvcp->dvcnam);
		for ( dvtp=dvcp->dvtlist; dvtp != NULL; dvtp=dvtp->dvtnext ) {
			if ( xinu_strlen(dvtp->dvtnam) )
				printf("\ton %s",dvtp->dvtnam);
			for ( i=0 ; i<NIO ; i++ ) {
				if ( (i%3) == 0 )
					printf("\n\t");
				printf("\t-%s %s",iotab[i],
					dvtp->ioroutine[i]->symnam);
			}
			printf("\n");
		}
	}
}
