/* snclient.c - snclient */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <snmp.h>

/*------------------------------------------------------------------------
 * snclient - send an SNMP request and wait for the response
 *------------------------------------------------------------------------
 */
int snclient(rqdp, fport/*, xinu_stdout*/)
struct req_desc	*rqdp;
char		*fport;
/*int		xinu_stdout;*/
{
    struct snbentry *bindl;
    u_char  buff[SNMPMAXSZ], reqidsave[10], reqidsavelen;
	int		snmpdev, len;
    int dgmode = DG_DMODE | DG_TMODE;
    
	rqdp->reqidlen = 0;
	rqdp->err_stat = 0;
	rqdp->err_idx = 0;

	if ((len = mksnmp(rqdp, buff, rqdp->reqtype)) == SYSERR)
	    return SCL_BADREQ;
	blkcopy(reqidsave, rqdp->reqid, rqdp->reqidlen);
	reqidsavelen = rqdp->reqidlen;
	/* open the SNMP port and put into data mode */
	if ((snmpdev = xinu_open(UDP, fport, ANYLPORT)) == SYSERR ||
        control(snmpdev, DG_SETMODE, &dgmode) == SYSERR) {
		xinu_close(snmpdev);
		return SCL_OPENF;
	}
	if (xinu_write(snmpdev, buff, len) == SYSERR) {
		xinu_close(snmpdev);
		return SCL_WRITEF;
	}
	/* retry once, on timeout */
	if ((len = xinu_read(snmpdev, buff, SNMPMAXSZ)) == TIMEOUT)
		len = xinu_read(snmpdev, buff, SNMPMAXSZ);
	if (len == TIMEOUT) {
		xinu_close(snmpdev);
		return SCL_NORESP;
	} else if (len == SYSERR) {
		xinu_close(snmpdev);
		return SCL_READF;
	}
    snfreebl(rqdp->bindlf);
    if (snparse(rqdp, buff, len) == SYSERR) {
        xinu_close(snmpdev);
		return SCL_BADRESP;
	}
	if (reqidsavelen != rqdp->reqidlen ||
	    ! blkequ(reqidsave, rqdp->reqid, reqidsavelen)) {
		xinu_close(snmpdev);
		return SCL_BADRESP;
	}
	/* convert the sb_a1str's to objid's and their values */
	if (sna2b(rqdp) == SYSERR) {
		xinu_close(snmpdev);
		return SCL_BADRESP;
	}
	xinu_close(snmpdev);
	return SCL_OK;
}
