/* x_snmp.c - x_snmp */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <tty.h>
#include <ctype.h>
#include <snmp.h>
#include <asn1.h>
#include <mib.h>
#include <mem.h>
#include <snvars.h>

/*------------------------------------------------------------------------
 * x_snmp - SNMP shell that does MIB object name completion
 *------------------------------------------------------------------------
 */
x_snmp(nargs, args)
int     nargs;
char	*args[];
{
	char	ch, snmpservstr[BUFSIZE];
	struct	tty	*ttyp;
    int     len, ct, i, mode;
	IPaddr	destmach;
    struct  snvars *psv;

	if (nargs != 2) {
        xinu_fprintf(STDERR, "usage: snmp agent\n");
		return OK;
	}
	args++;  nargs--;
    sninit();
	if (name2ip(destmach, *args) == SYSERR) {
        xinu_fprintf(STDOUT,"snmp: couldn't resolve name %s\n", *args);
        return SYSERR;
	}
    if ((psv = (struct snvars *) xmalloc(sizeof(struct snvars))) == (struct snvars *)NULLPTR) {
        xinu_fprintf(STDERR,"snmp: not enough memory\n");
		return(SYSERR);
	}
    psv->lastoidset = FALSE;
    for (i=0; i<BUFSIZE; i++) {
        psv->str2compl[i] = '\0';
    }

    ip2dot(snmpservstr, destmach);
	xinu_sprintf(snmpservstr + xinu_strlen(snmpservstr), ":%d", SNMPPORT);
    ttyp = &tty[getpdev(xinu_getpid(),STDIN)];
	ct = 0;
	mode = M_CHARIN;
    psv->next_completion = psv->num_completions = 0;
    control(STDIN, TCMODER);    /* put STDIN into raw mode */
    ttyp->evis = FALSE;
    ttyp->ieback = TRUE;
    ttyp->ierase = TRUE; 
    xinu_write(STDIN, PROMPT, xinu_strlen(PROMPT));  /* print the prompt */

    while (TRUE) {
        if ((ch = getc(STDIN)) == EOF) {
            xinu_write(STDIN, EXITSTR, xinu_strlen(EXITSTR));
            putc(STDIN, '\n');
            control(STDIN, TCMODEC);
            ttyp->evis = TRUE;
            return OK;
		}
		if (ch == COMPLETE) {
			if (mode == M_CHARIN) {
				mode = M_COMPL;
				/* find beginning of word */
                for (i=ct-1; i >= 0 && psv->buf[i] != BLANK;
						i--)
					/* empty */;
                psv->s2clen = ct - ++i;
                strncpy(psv->str2compl, psv->buf + i, psv->s2clen);
                find_completions(psv);
			}
            if (psv->num_completions == 0) {
                putc(STDIN, BELL);
				mode = M_CHARIN;
			} else
                print_completion(ttyp, &ct, psv);
			continue;
		}
		if (ch == KILLWORD && mode == M_COMPL) {
            /* kill word in compl. mode goes back to
			   original string to complete. */
            eraseword(ttyp, &ct, psv);
            strncpy(psv->buf + ct, psv->str2compl, psv->s2clen);
            xinu_write(STDIN, psv->buf + ct, psv->s2clen);
            ct += psv->s2clen;
			mode = M_CHARIN;
            psv->next_completion = psv->num_completions = 0;
			continue;
		}
		if (mode == M_COMPL) {	/* && ch != KILLWORD */
			mode = M_CHARIN;
            psv->str2compl[(psv->s2clen = 0)] = '\0';
            psv->num_completions = psv->next_completion = 0;
		}
		if (ch == KILLWORD) {	/* && mode != M_COMPL */
            eraseword(ttyp, &ct, psv);
			continue;
		}
		if (ch == ttyp->ikillc && ttyp->ikill) {
            eraseall(ttyp, ct, psv);
			ct = 0;
			continue;
		}
        if (ch == BACKSP) {
			if (ct > 0) 
                erase1(ttyp, --ct, psv);
			continue;
		}
		if (ch == RETURN) {
            echoch(ch, ttyp);
            psv->buf[ct] = '\0';
            if (strequ(psv->buf, EXITSTR)) {
                control(STDIN, TCMODEC);
                ttyp->evis = TRUE;
				return OK;
			}
            if (!strncmp(psv->buf, AGENTSTR, 5)) {
                if (name2ip(destmach, &psv->buf[6]) == SYSERR) {
                    xinu_fprintf(STDOUT,"snmp: couldn't resolve %s\n", &psv->buf[6]);
                    control(STDIN, TCMODEC);
                    ttyp->evis = TRUE;
                    return OK;
                } 
                else {
                    ip2dot(snmpservstr, destmach);
                    xinu_sprintf(snmpservstr + xinu_strlen(snmpservstr), ":%d", SNMPPORT);
                    for (i = 0; i < BUFSIZE; i++)
                        psv->buf[i] = '\0';
                    xinu_write(STDIN, PROMPT, xinu_strlen(PROMPT));
                    ct = 0;
                    continue;
                }
            }
            sendquery(snmpservstr, psv);
			for (i = 0; i < BUFSIZE; i++)
                psv->buf[i] = '\0';
            xinu_write(STDIN, PROMPT, xinu_strlen(PROMPT));
			ct = 0;
			continue;
		}
        if (ch == NEXT) {
            echoch(RETURN, ttyp);
            xinu_strcpy(psv->buf,"next");
            ct = ct + 4;
            psv->buf[ct] = '\0';
            sendquery(snmpservstr, psv);
			for (i = 0; i < BUFSIZE; i++)
                psv->buf[i] = '\0';
            xinu_write(STDIN, PROMPT, xinu_strlen(PROMPT));
			ct = 0;
			continue;
        }
        /* all non-special characters */
		if (ct == BUFSIZE - 1) 
            putc(STDIN, BELL);
		else {
            echoch(ch, ttyp);
            psv->buf[ct++] = ch;
		}
	}
}

/*------------------------------------------------------------------------
 * print_completion - write the next name completion to STDOUT
 *------------------------------------------------------------------------
 */
print_completion(ttyp, ct, psv)
struct tty *ttyp;
int	*ct;
struct snvars *psv;
{
	int	i;
	
    if (psv->next_completion >= psv->num_completions) {
        putc(STDIN, BELL);
        return OK;
	}
    eraseword(ttyp, ct, psv);
    xinu_strcpy(psv->buf + *ct, 
        mib[psv->completions[psv->next_completion]].mi_prefix);
    strcat(psv->buf + *ct, 
        mib[psv->completions[psv->next_completion++]].mi_name);
    xinu_write(STDIN, psv->buf + *ct, xinu_strlen(psv->buf) - *ct);
    *ct = xinu_strlen(psv->buf);
    return OK;
}

/*------------------------------------------------------------------------
 * find_completions - find all the completions in the mib for string
 * 		      str2compl.  Put the indices of the completions
 * 		      into the completions array.
 *------------------------------------------------------------------------
 */
find_completions(psv)
struct snvars *psv;
{
	int 	i;
	
	for (i = 0; i < mib_entries; i++) 
        if (strnequ(mib[i].mi_name, psv->str2compl, psv->s2clen))
            psv->completions[psv->num_completions++] = i;
    psv->next_completion = 0;
    return OK;
}

/*------------------------------------------------------------------------
 * parsevalue - parse the type and value of variable to set
 *------------------------------------------------------------------------
 */
parsevalue(wrd, bl, psv)
char        **wrd;
struct snbentry	*bl;
struct snvars *psv;
{

extern long atol(char *);
    
    if (strequ(*wrd, "int"))
		SVTYPE(bl) = ASN1_INT;
    else if (strequ(*wrd, "counter"))
		SVTYPE(bl) = ASN1_COUNTER;
    else if (strequ(*wrd, "gauge"))
		SVTYPE(bl) = ASN1_GAUGE;
    else if (strequ(*wrd, "timeticks"))
		SVTYPE(bl) = ASN1_TIMETICKS;
    else if (strequ(*wrd, "str"))
		SVTYPE(bl) = ASN1_OCTSTR;
    else if (strequ(*wrd, "objid"))
		SVTYPE(bl) = ASN1_OBJID;
    else if (strequ(*wrd, "ipaddr"))
		SVTYPE(bl) = ASN1_IPADDR;
	else
		return SYSERR;

    getword(wrd, psv);
    if (**wrd == NULL)
		return SYSERR;

	switch (SVTYPE(bl)) {
	case ASN1_INT:
	case ASN1_COUNTER:
	case ASN1_GAUGE:
	case ASN1_TIMETICKS:
        SVINT(bl) = atol(*wrd);
		return OK;
	case ASN1_OCTSTR:
        SVSTRLEN(bl) = xinu_strlen(*wrd);
		SVSTR(bl) = (char *) getmem(SVSTRLEN(bl));
        blkcopy(SVSTR(bl), *wrd, SVSTRLEN(bl));
		return OK;
	case ASN1_OBJID:
        SVOIDLEN(bl) = dot2oid(SVOID(bl), *wrd);
		return OK;
	case ASN1_IPADDR:
        dot2ip(SVIPADDR(bl), *wrd);
    default: return OK;
	}
}

/*------------------------------------------------------------------------
 *  echoch  --  echo a character with visual and ocrlf options
 *------------------------------------------------------------------------
 */
LOCAL echoch(ch, ttyp)
char   ch;
struct tty      *ttyp;
{
	if ((ch == NEWLINE || ch == RETURN) && ttyp->ecrlf) {
        putc(STDIN, RETURN);
        putc(STDIN, NEWLINE);
	} else if ((ch < BLANK || ch == 0177) && ttyp->evis) {
        putc(STDIN, UPARROW);
        putc(STDIN, ch + 0100);    /* make it printable    */
	} else
        putc(STDIN, ch);
    return OK;
}

/*------------------------------------------------------------------------
 *  erase1  --  erase one character honoring erasing backspace
 *------------------------------------------------------------------------
 */
LOCAL erase1(ttyp, ct, psv)
struct tty	*ttyp;
int 		ct;
struct snvars *psv;
{
	char	ch;

    ch = psv->buf[ct];
    if (ch < BLANK || ch == 0177) {
		if (ttyp->evis)	{
            putc(STDIN, BACKSP);
			if (ttyp->ieback) {
                putc(STDIN, BLANK);
                putc(STDIN, BACKSP);
			}
		}
        putc(STDIN, BACKSP);
		if (ttyp->ieback) {
            putc(STDIN, BLANK);
            putc(STDIN, BACKSP);
		}
	} else {
        putc(STDIN, BACKSP);
		if (ttyp->ieback) {
            putc(STDIN, BLANK);
            putc(STDIN, BACKSP);
		}
	}
    return OK;
}

/*------------------------------------------------------------------------
 *  eraseall  --  erase all characters honoring erasing backspace
 *------------------------------------------------------------------------
 */
LOCAL eraseall(ttyp, ct, psv)
struct tty	*ttyp;
int		ct;
struct snvars *psv;
{
	for (--ct; ct >= 0; ct--) 
        erase1(ttyp, ct, psv);
    return OK;
}

/*------------------------------------------------------------------------
 *  eraseword  --  erase the previous word
 *------------------------------------------------------------------------
 */
LOCAL eraseword(ttyp, ct, psv)
struct tty	*ttyp;
int		*ct;
struct snvars *psv;
{
	/* if first character behind cursor is a blank, eat it up */
    if (psv->buf[*ct - 1] == BLANK)
        erase1(ttyp, --(*ct), psv);
    for (--(*ct); *ct >= 0 && psv->buf[*ct] != BLANK; (*ct)--) 
        erase1(ttyp, *ct, psv);
	(*ct)++;
    return OK;
}

/*------------------------------------------------------------------------
 * strnequ - if s1 and s2 have same length and same value, return TRUE
 *------------------------------------------------------------------------
 */
strnequ(s1, s2, n)
char	*s1, *s2;
int	n;
{
	if (xinu_strlen(s1) < n)
		return 0;
	return ! strncmp(s1, s2, n);
}

/*------------------------------------------------------------------------
 * initgetword - initialize the routines to get words off the input
 *------------------------------------------------------------------------
 */
initgetword(buf, psv)
char	*buf;
struct snvars *psv;
{
    psv->nextchar = buf;     /* nextchar at beginnning of buffer */
   return OK;
}

/*------------------------------------------------------------------------
 * getword - get the next word from the input line
 *------------------------------------------------------------------------
 */
getword(wrd, psv)
char    **wrd;
struct snvars *psv;
{
    while (*psv->nextchar == BLANK)  /* skip blanks */
        psv->nextchar++;
    *wrd = psv->nextchar;
    while ((*psv->nextchar != '\0') && (*psv->nextchar != BLANK))
        psv->nextchar++;
    if (*psv->nextchar != '\0')
        *psv->nextchar++ = '\0';
    return OK;
}

/*------------------------------------------------------------------------
 * objd2oid - take an object string and parse it into an object id
 *------------------------------------------------------------------------
 */
objd2oid(str, oip)
char		*str;
struct oid	*oip;
{
	register char	*cp;
	struct mib_info	*onp;
    int         idlen, len, i;
	char		tmp[BUFSIZE];

	idlen = 0;
	cp = str;
	if (isalpha(*str)) {
		/*
		 * The string at least starts out with object descriptors,
		 * and may have object ids at the end. So, first, find the
		 * end of the descriptors part.
		 */
		for (; *cp != '\0' && (isalpha(*cp) || *cp == '.'); cp++)
			 /* empty */;
		len = cp - str;
		if (*cp != '\0') 
			len--;

		for (i = 0; i < mib_entries; i++) {
			onp = &mib[i];
			xinu_strcpy(tmp, onp->mi_prefix);
			strcat(tmp, onp->mi_name);
			if (xinu_strlen(tmp) == len && blkequ(tmp, str, len))
				break;
		}
		if (i >= mib_entries)
			return SYSERR;

		idlen = oip->len = onp->mi_objid.len;
		blkcopy(oip->id, onp->mi_objid.id, idlen*2);

		/* remove possible trailing ".0" */
		if (strequ(cp, "0"))
			return OK;
	}
	/* rest of variable name is object sub-id's */
	if (isdigit(*cp)) 
		oip->len = idlen + dot2oid(&oip->id[idlen], cp);
	return OK;
}

/*------------------------------------------------------------------------
 * dot2oid - convert a character representation of an object id into
 * 	     an array of sub-identifiers
 *------------------------------------------------------------------------
 */
int dot2oid(id, str)
unsigned short id[];   /* array of id's */
char	*str;
{
	register int totval;
	int 	numids = 0;
	
	while (TRUE) {
		totval = 0;
		while (isdigit(*str)) 
		    totval = totval * 10 + (*str++ - '0');
        id[numids++] = (unsigned short) totval;
		
		if (*str == '.')
		    str++;
		else
		    break;
	}
	return numids;
}

/*------------------------------------------------------------------------
 * parseoidlist - read a list of oid's from the input line
 *------------------------------------------------------------------------
 */
parseoidlist(rqdp, wrd, psv)
struct req_desc	*rqdp;
char            **wrd;
struct snvars   *psv;
{
	struct	snbentry *parseoid(), *bl;

    while (**wrd != '\0') {
        if ((bl = parseoid(wrd, psv)) == (struct snbentry *) SYSERR) {
            snfreebl(rqdp->bindlf); 
            /* free the good portions of binding list too */
           return SYSERR;
        }
		if (rqdp->bindlf == NULL)
			rqdp->bindlf = rqdp->bindle = bl;
		else {
			bl->sb_prev = rqdp->bindle;
			rqdp->bindle = rqdp->bindle->sb_next = bl;
		}
	}
	return OK;
}

/*------------------------------------------------------------------------
 * parseoid - read an oid from the input line
 *------------------------------------------------------------------------
 */
struct snbentry *parseoid(wrd, psv)
char        **wrd;
struct snvars *psv;
{
	struct	snbentry *bl, *getnewbl();
	
	bl = getnewbl();
    if (objd2oid(*wrd, &bl->sb_oid) == SYSERR) {
        freemem((char*)bl, sizeof(struct snbentry));
		return (struct snbentry *) SYSERR;
	} else
		SVTYPE(bl) = ASN1_NULL;
    getword(wrd, psv);
    return bl;
}

/*------------------------------------------------------------------------
 * sendquery - parse the input line and send the query.  Input has
 * one of the following forms:
 * 		 [object-name]+
 * 		 next [object-name]+ 
 * 		 set [object-name type value]+
 *------------------------------------------------------------------------
 */
sendquery(server, psv)
char	*server;
struct snvars *psv;
{
	struct req_desc	rqd;
	struct snbentry	*bl, *getnewbl();
    char        *wrd;
	int 		repl;
    int i;

    initgetword(psv->buf, psv);
	rqd.reqtype = PDU_GET;		/* by default */
	rqd.bindle = rqd.bindlf = NULL;
    getword(&wrd, psv);
    if (*wrd == '\0')
		return OK;

    if (strequ(wrd, "next")) {
		rqd.reqtype = PDU_GETN;
        getword(&wrd, psv);
        if (parseoidlist(&rqd, &wrd, psv) == SYSERR) {
            xinu_fprintf(STDOUT, "unknown variable\n");
			return SYSERR;
		}
		if ((bl = rqd.bindlf) == NULL) {
            if (psv->lastoidset) { /* no oids so use last one */
				rqd.bindlf = rqd.bindle = bl = getnewbl();
                bl->sb_oid.len = psv->lastobjid.len;
                blkcopy(bl->sb_oid.id, psv->lastobjid.id,
                    psv->lastobjid.len*2);
				SVTYPE(bl) = ASN1_NULL;
			} else {
                xinu_fprintf(STDOUT, "bad syntax\n");
				return SYSERR;
			}
		}
    } else if (strequ(wrd, "set")) {
		rqd.reqtype = PDU_SET;
        if (parseset(&rqd, psv) == SYSERR)
			return SYSERR;
    } else if (parseoidlist(&rqd, &wrd, psv) == SYSERR) {
                xinu_fprintf(STDOUT, "unknown variable\n");
                return SYSERR;
            }
    repl = snclient(&rqd, server, STDOUT);
    
    /* the following code should be restructured */
    
    switch (repl) {
	case SCL_OK:
		if (rqd.err_stat == SNMP_OK)
            snmpprint(STDOUT,rqd.bindlf);
		else
            snerr(STDOUT, &rqd);
		break;
	case SCL_OPENF:
        xinu_fprintf(STDOUT, "snmp: open failed\n");
		break;
	case SCL_WRITEF:
        xinu_fprintf(STDOUT, "snmp: write failed\n");
		break;
	case SCL_NORESP:
        xinu_fprintf(STDOUT, "snmp: No response from server %s\n", server);
        break;
	case SCL_READF:
        xinu_fprintf(STDOUT, "snmp: read failed\n");
		break;
	case SCL_BADRESP:
        xinu_fprintf(STDOUT, "snmp: received bad response\n");
		break;
	}
	
	/* save this object for use with the "next" operation */
    psv->lastobjid.len = rqd.bindlf->sb_oid.len;
    blkcopy(psv->lastobjid.id, rqd.bindlf->sb_oid.id,
		rqd.bindlf->sb_oid.len*2);
    psv->lastoidset = TRUE;
	snfreebl(rqd.bindlf);
	return OK;
}

/*------------------------------------------------------------------------
 * getnewbl - get a new bindlist node from memory and initialize it
 *------------------------------------------------------------------------
 */
struct snbentry *getnewbl()
{
     	struct snbentry *blp;

	blp = (struct snbentry *) getmem(sizeof(struct snbentry));
    blp->sb_next = blp->sb_prev = NULL;
	blp->sb_a1slen = 0;
	return blp;
}

/*------------------------------------------------------------------------
 * parseset - parse the words from the input line for a set operation
 *------------------------------------------------------------------------
 */
parseset(rqdp, psv)
struct req_desc	*rqdp;
struct snvars *psv;
{
	struct snbentry	*bl, *parseoid();
    char        *wrd;

    for (getword(&wrd, psv); *wrd != NULL; getword(&wrd, psv)) {
        if ((bl = parseoid(&wrd, psv)) == (struct snbentry *) SYSERR)
			return SYSERR;
		if (rqdp->bindlf) {
			bl->sb_prev = rqdp->bindle;
			rqdp->bindle = rqdp->bindle->sb_next = bl;
		} else
			rqdp->bindlf = rqdp->bindle = bl;
        if (parsevalue(&wrd, bl, psv) == SYSERR) {
            xinu_fprintf(STDOUT, "bad syntax\n");
			return SYSERR;
		}
	}
	return OK;
}
