/* snprint.c - snmpprint snmpprint_val snmpprint_objid snmpprint_objname */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ctype.h>
#include <snmp.h>
#include <mib.h>
#include <asn1.h>

/* --------------------------------------------------------------------
 * snmpprint - print out each binding in the bindings list.
 * --------------------------------------------------------------------
 */
snmpprint(xinu_stdout, bindl)
int xinu_stdout;
struct snbentry *bindl;
{
    char buf[256];
    
    /* for each element in bindl, print objid string and value. */

    for (; bindl != (struct snbentry *) NULL; bindl = bindl->sb_next) {
        snmpprint_objname(buf, &bindl->sb_oid);
        xinu_write(xinu_stdout, buf, xinu_strlen(buf));
        snmpprint_val(buf, &bindl->sb_val);
        xinu_write(xinu_stdout, buf, xinu_strlen(buf));
		write(xinu_stdout, "\n", 1);
	}
    return OK;
}

/* --------------------------------------------------------------------
 * snmpprint_val - print out the value in the snval structure.
 * --------------------------------------------------------------------
 */
snmpprint_val(buf, val)
char *buf;
struct snval *val;
{
	int 	i;
    char buff[80];

	switch(val->sv_type) {
	    case ASN1_INT :
        xinu_strcpy(buff, " {Integer} ");
		break;
	    case ASN1_COUNTER :
        xinu_strcpy(buff, " {Counter} ");
		break;
	    case ASN1_GAUGE :
        xinu_strcpy(buff, " {Gauge} ");
		break;
	    case ASN1_TIMETICKS :
        xinu_strcpy(buff, " {Timeticks} ");
		break;
	    case ASN1_OCTSTR :
        xinu_strcpy(buff, " {Octet Str} ");
		break;
	    case ASN1_NULL :
		break;
	    case ASN1_OBJID :
        xinu_strcpy(buff, " {Object ID} ");
		break;
	    case ASN1_IPADDR :
        xinu_strcpy(buff, " {IP Address} ");
		break;
	}
	if (val->sv_type != ASN1_NULL)
        xinu_strcpy(buf, buff);
    else buf[0] = '\0';
		
	switch(val->sv_type) {
	    case ASN1_INT :
	    case ASN1_COUNTER :
	    case ASN1_GAUGE :
	    case ASN1_TIMETICKS :
        xinu_sprintf(buff, "%D", val->sv_val.sv_int);   /* was %d */
        strcat(buf, buff);
		break;
	    case ASN1_OCTSTR :
		for (i = 0; i < val->sv_val.sv_str.sv_len; i++)
		    if (! isprint(val->sv_val.sv_str.sv_str[i]))
			break;
		/* if something wasn't printable */
        if (i < val->sv_val.sv_str.sv_len) {
            for (i = 0; i < val->sv_val.sv_str.sv_len; i++) {
                xinu_sprintf(buff, " 0x%02x",
				    (u_char) val->sv_val.sv_str.sv_str[i]);
                if (i==0) *buff = '{';    
                strcat(buf, buff);
		    }
            if (i != 0) strcat(buf,"}");
        }
        else {
            blkcopy(buff, val->sv_val.sv_str.sv_str, 
                          val->sv_val.sv_str.sv_len);
            buff[val->sv_val.sv_str.sv_len] = '\0';
            strcat(buf,"{");               
            strcat(buf, buff);
            strcat(buf,"}");
        }
		break;
	    case ASN1_NULL :
        strcat(buf, "Null");
		break;
	    case ASN1_OBJID :
        snmpprint_objid(buff, &(val->sv_val.sv_oid));
        strcat(buf, buff);
		break;
	    case ASN1_IPADDR :
		for (i = 0; i < IP_ALEN - 1; i++) {
			xinu_sprintf(buff, "%d.",
				(u_char) val->sv_val.sv_ipaddr[i]);
            strcat(buf, buff);
		}
		xinu_sprintf(buff, "%d", (u_char) val->sv_val.sv_ipaddr[i]);
        strcat(buf, buff);
		break;
	    default :
		break;
	}
    return OK;
}	
	
/* --------------------------------------------------------------------
 * snmpprint_objid - print out an object identifier.
 * --------------------------------------------------------------------
 */
snmpprint_objid(buf, objidp)
char *buf;
struct oid *objidp;
{
	int 	i;
    char    buff[80];

    buf[0] = '\0';
    for (i = 0; i < objidp->len - 1; i++) {
        xinu_sprintf(buff, "%d.", objidp->id[i]);
        strcat(buf, buff);
	}
	xinu_sprintf(buff, "%d", objidp->id[i]);
    strcat(buf, buff);
    return OK;
}

/* --------------------------------------------------------------------
 * snmpprint_objname - print out the name corresponding to an objid.
 * --------------------------------------------------------------------
 */
snmpprint_objname(buf, oip)
char *buf;
struct oid *oip;
{
	char	buff[80];
	struct	mib_info *np;
	struct	mib_info *getmib();
	struct 	oid	tmpobj;
    unsigned short *tp;
	int	i;
	
    if ((char *) (np = getmib(oip)) == NULL) {
       snmpprint_objid(buf, oip);
    }
	else {
        xinu_strcpy(buf, np->mi_prefix);
        strcat(buf, np->mi_name);
        if (np->mi_varleaf == NLEAF) {          /* this var is not a leaf */
            strcat(buf, ".");
			tmpobj.len = oip->len - np->mi_objid.len;
			tp = tmpobj.id;
			for (i = np->mi_objid.len; i < oip->len; i++)
			    *tp++ = oip->id[i];
            buff[0]='\0';    
            snmpprint_objid(buff, &tmpobj);
            strcat(buf, buff);
		}
	}
    return OK;
}
