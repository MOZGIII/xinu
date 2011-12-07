/* name2ip.c - name2ip, gname, resolve */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <mem.h>

extern	char	*index();

/*------------------------------------------------------------------------
 *  name2ip  -  return the IP address for a given DNS name
 *------------------------------------------------------------------------
 */
SYSCALL	name2ip(ip, nam)
IPaddr	ip;
char	*nam;
{
	int	start, end, mid;
	Bool	isnum;
	char	*p, nbuf[128], *index();

	isnum = TRUE;
	for (p=nam; *p; ++p) {
		isnum &= ((*p >= '0' && *p <= '9') || *p == '.');
		if (!isnum)
			break;
	}
	if (isnum) {
		dot2ip(ip, nam);
		return OK;
	}
	return resolve(ip, nam);
}

/*------------------------------------------------------------------------
 *  gname  -  use the DNS to look up the name
 *------------------------------------------------------------------------
 */
int gname(ip, nam)
IPaddr	ip;
char	*nam;
{
	char	tmpstr[64];		/* temporary string buffer	*/
	char	*buf;			/* buffer to hold domain query	*/
	int	dg, i, dnlen;
    int     dgmode = DG_DMODE | DG_TMODE;
    register char   *p, *p2, *p3;
	register struct	dn_mesg *dnptr;

	dnptr = (struct dn_mesg *) (buf = (char *) getmem(DN_MLEN));
	dnptr->dn_id = 0;
	dnptr->dn_opparm = hs2net(DN_RD);
	dnptr->dn_qcount = hs2net(1);
	dnptr->dn_acount = dnptr->dn_ncount = dnptr->dn_dcount = 0;
	p = dnptr->dn_qaaa;

	xinu_strcpy(tmpstr, nam);
	p2 = tmpstr;
	while (p3=index(p2, '.')) {
		*p3 = '\0';
		dn_cat(p, p2);
		p2 = p3+1;
	}
	dn_cat(p, p2);
	*p++ = NULLCH;	/* terminate name */

	/* Add query type and query class fields to name */

	( (struct dn_qsuf *)p )->dn_type = hs2net(DN_QTHA);
	( (struct dn_qsuf *)p )->dn_clas = hs2net(DN_QCIN);
	p += sizeof(struct dn_qsuf);

	/* send query */

    sprintf(tmpstr, "%u.%u.%u.%u:53", BYTE(nif[1].ni_nserver, 0), 
        BYTE(nif[1].ni_nserver, 1), BYTE(nif[1].ni_nserver, 2), 
        BYTE(nif[1].ni_nserver, 3));

    dg = xinu_open(UDP, tmpstr, ANYLPORT);
    control(dg, DG_SETMODE, &dgmode);
	write (dg, buf, p - buf);
	if ( (i = xinu_read(dg, buf, DN_MLEN)) == SYSERR || i == TIMEOUT) {
		xinu_close(dg);
		freemem(buf, DN_MLEN);
		return SYSERR;
	}
	xinu_close(dg);
	if (net2hs(dnptr->dn_opparm) & DN_RESP ||
	    net2hs(dnptr->dn_acount) <= 0) {
		freemem(buf, DN_MLEN);
		return SYSERR;
	}
	/* In answer, skip name and remainder of resource record header	*/

	while (*p != NULLCH)
		if (*p & DN_CMPRS) 	/* compressed section of name	*/
			*++p = NULLCH;
		else
			p += *p + 1;
	p += DN_RLEN + 1;

	/* Copy IP to user */

	for (i=0; i < IP_ALEN; ++i)
		ip[i] = *p++;
	freemem(buf, DN_MLEN);
	return OK;
}

/*------------------------------------------------------------------------
 *  resolve  -  do shorthand DNS name resolution
 *------------------------------------------------------------------------
 */
int resolve(ip, nam)
IPaddr	ip;
char	*nam;
{
	IPaddr	myip;
	char	myname[64];
	char	name_buf[100];
	char	*name_suffix;
	int	i;

	/* if it ends in a 'dot', remove it and try exactly once */
	if (nam[xinu_strlen(nam)-1] == '.') {
		xinu_strcpy(name_buf, nam);
		name_buf[xinu_strlen(name_buf)-1] = NULLCH;
		return gname(ip, name_buf);
	}
	if (getname(myname) == SYSERR)
		return SYSERR;
	name_suffix = myname;

	/* tack on successively smaller suffixes of MY name */
	while (name_suffix = index(++name_suffix, '.')) {
		xinu_sprintf(name_buf, "%s%s", nam, name_suffix);
		if (gname(ip, name_buf) == OK)
			return OK;
	}
	return gname(ip, nam);		/* try the original	*/
}
