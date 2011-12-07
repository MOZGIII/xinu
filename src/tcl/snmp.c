/* 
 * tclSnmp.c --
 *
 *  This file contains the command routines for Tcl SNMP commands
 *
 */

#include <tclInt.h>
#include <network.h>
#include <snmp.h>
#include <asn1.h>
#include <mib.h>
#include <snvars.h>

#define AGENT 1
#define GETN  2
#define SET   3

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SnmpCmd --
 *
 *  This procedure is invoked to process the "snmp" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_SnmpCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;                   /* Number of arguments. */
    char **argv;                /* Argument strings. */
{
    int     cmd;
    int     i;
    IPaddr  agent;
    char    *last;
    char    agentstr[80];
    struct snvars *psv;
    struct req_desc rqd;
        
    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
        "snmp option ?arg ...?", "\"", NULLPTR);
	return TCL_ERROR;
    }
    
    if (!strcmp(argv[1],"next")) cmd = GETN;
    else if (!strcmp(argv[1],"agent")) cmd = AGENT;
    else if (!strcmp(argv[1],"set")) cmd = SET;
    else cmd = 0;

    if (Tcl_GetVar(interp, "last", 0) == NULLPTR)
        Tcl_SetVar(interp, "last", "system", 0);         

    psv = (struct snvars*)xmalloc(sizeof(struct snvars));

    switch (cmd) {
    case GETN:  rqd.reqtype = PDU_GETN;
                if (argc ==  2) {
                    strcpy(psv->buf, Tcl_GetVar(interp, "last", 0));
                    strcat(psv->buf, "\0");
                    goto doget;
                }
                else {
                    *psv->buf = '\0';
                    for (i=2; i<argc; i++){
                        sncmpl(psv->buf, argv[i]);
                        strcat(psv->buf," ");
                    }
                    goto doget;
                }
                
    case AGENT: if (argc == 3) {
                    if (name2ip(agent, argv[2]) == SYSERR) {
                        Tcl_AppendResult(interp, "couldn't resolve name ", 
                        argv[2], NULLPTR);
                        Tcl_UnsetVar(interp, "agent", 0);
                        xfree(psv);
                        return TCL_ERROR;
                    }
                    ip2dot(agentstr, agent);
                    sprintf(agentstr + xinu_strlen(agentstr), ":%d", SNMPPORT);
                    Tcl_SetVar(interp, "agent", agentstr, 0);
                    Tcl_AppendResult(interp, Tcl_GetVar(interp, "agent", 0),
                    NULLPTR);
                    break;
                } else {
                    Tcl_AppendResult(interp, "wrong # args: should be \"",
                    "agent name", "\"", NULLPTR);
                    xfree(psv);
                    return TCL_ERROR;
                }
                
    case SET:   Tcl_AppendResult(interp, "SET not implemented", NULLPTR);
                xfree(psv);
                return TCL_ERROR;
                                
    default:    if (Tcl_GetVar(interp, "agent", 0) == NULLPTR) {
                    Tcl_AppendResult(interp, "no agent", NULLPTR);
                    xfree(psv);
                    return TCL_ERROR;
                }
                *psv->buf = '\0';
                for (i=1; i<argc; i++){
                    sncmpl(psv->buf, argv[i]);
                    strcat(psv->buf," ");
                }
                rqd.reqtype = PDU_GET;
     doget:     rqd.bindle = rqd.bindlf = (struct snbentry*)NULLPTR;

                switch (snmpget(interp, &rqd, psv)) {
                    case SCL_OK:
                        if (rqd.err_stat == SNMP_OK)
                            snappend(interp, rqd.bindlf);
                        else
                            sneappend(interp, &rqd);
                        break;
                    case SCL_NORESP:
                        Tcl_AppendResult(interp, "no response ",
                        "from agent ", Tcl_GetVar(interp, "agent", 0),
                        NULLPTR);
                    }
                snfreebl(rqd.bindlf);
                xfree(psv);
    }
    return TCL_OK;
}
/*---------------------------------------------------------------------------
 * SNMP GET function: GET an SNMP object
 *---------------------------------------------------------------------------
*/ 
int snmpget(interp, rqd, psv)
Tcl_Interp      *interp;
struct req_desc *rqd;
struct snvars   *psv;             /* contains the variables to get */
{
    char    *wrd;
    
    initgetword(psv->buf, psv);
    getword(&wrd, psv);

    if (*psv->buf =='\0')
        return OK;
    if (parseoidlist(rqd, &wrd, psv) == SYSERR) {
        Tcl_AppendResult(interp, "unknown variable", NULLPTR);
        return SYSERR;
    }
    return snclient(rqd, Tcl_GetVar(interp, "agent", 0), 0);
}
/*---------------------------------------------------------------------------
 * SNMP SNAPPEND function: append a result
 *---------------------------------------------------------------------------
*/ 
int snappend(interp, bindl)
Tcl_Interp  *interp;
struct snbentry *bindl;
{
    char buf[128];
    
    /* for each element in bindl, print objid string and value. */
	for (; bindl != (struct snbentry *) NULL; bindl = bindl->sb_next) {
        snmpprint_objname(buf, &bindl->sb_oid);
        Tcl_AppendResult(interp, buf, NULLPTR);
        Tcl_SetVar(interp, "last", buf, 0); /* save object name for "next" */ 
        snmpprint_val(buf, &bindl->sb_val);
        Tcl_AppendResult(interp, buf, " ", NULLPTR);
	}
    return TCL_OK;
}
/*---------------------------------------------------------------------------
 * SNMP SNEAPPEND function: append an SNMP error message
 *---------------------------------------------------------------------------
*/ 
int sneappend(interp, rqd)
Tcl_Interp  *interp;
struct req_desc *rqd;
{
	struct 	snbentry *bindl;
    int     i;
	char	buf[128];
	
    if (sna2b(rqd) == SYSERR)
        return SYSERR;
        
    for (bindl = rqd->bindlf, i = 1; bindl && i != rqd->err_idx;
        bindl = bindl->sb_next, i++) ;
        
    sprintf(buf, "error %d ", rqd->err_stat);
    Tcl_AppendResult(interp, buf, NULLPTR); 
    snmpprint_objid(buf, &bindl->sb_oid);
    Tcl_AppendResult(interp, buf, NULLPTR); 
    return TCL_OK;
}
/*---------------------------------------------------------------------------
 * SNMP SNCMPL function: complete an object name by adding the prefix
 *---------------------------------------------------------------------------
*/ 
int sncmpl(dst,src)
char *dst;
char *src;
{
    int i, n;
    
    for (i=0, n=0; i < xinu_strlen(src); i++) {
        if (src[i] != '.') n++;
        else break;
    }  
    
    for (i=0; i < mib_entries; i++) {
        if (n == xinu_strlen(mib[i].mi_name))
            if (strnequ(mib[i].mi_name, src, n)) {
                strcat(dst, mib[i].mi_prefix);
                strcat(dst, src);
            }
    }
    return OK;
}

