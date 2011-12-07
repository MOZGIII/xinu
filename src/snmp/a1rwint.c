/* a1rwint.c - a1readint, a1writeint */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <snmp.h>
#include <asn1.h>

#define h2asn 	blkcopy

/*------------------------------------------------------------------------
 * a1readint - convert an ASN.1 encoded integer into a machine integer
 *------------------------------------------------------------------------
 */
long a1readint(pack, len)
u_char	*pack;
int	len;
{
    register long    tot;
	u_char 		neg;
	int		tlen;
    if ((tlen = len) > sizeof(long))
		return 0;
    tot = *pack & ~CHAR_HIBIT;
	neg = *pack & CHAR_HIBIT;
	for (tlen--, pack++ ; tlen > 0 ; tlen--, pack++)
		tot = (tot << CHAR_BITS) | (int) *pack;
	if (neg)
		tot -= (1 << ((len * CHAR_BITS) - 1));
    return tot;
}

/*------------------------------------------------------------------------
 * a1writeint - convert an integer into its ASN.1 encoded form
 *------------------------------------------------------------------------
 */
int a1writeint(val, buffp, a1type)
long val;
u_char	*buffp;
int	a1type;
{
    unsigned long   tmp;
    unsigned numbytes;
	register u_char	*bp;
    int     i;

	bp = buffp;
	tmp = val;
	if (A1_SIGNED(a1type) && val < 0)
		tmp = -val;
    if (tmp < (unsigned long) CHAR_HIBIT)
		*bp++ = numbytes = (u_char) 1;
    else if (tmp < (unsigned long) BYTE2_HIBIT)
		*bp++ = numbytes = (u_char) 2;
    else if (tmp < (unsigned long) BYTE3_HIBIT)
		*bp++ = numbytes = (u_char) 3;
    else if (tmp < (unsigned long) BYTE4_HIBIT)
		*bp++ = numbytes = (u_char) 4;
	else {	/* 5 bytes for unsigned with high bit set */
		*bp++ = (u_char) 5;	/* length */
		*bp++ = (u_char) 0;
		numbytes = 4;
	}
    for (i=numbytes-1; i>=0; i--)       /* copy backwards for 8086 */
    *bp++=*((char *) &val+i);
/*    h2asn(bp, ((char *) &val) + (sizeof(int) - numbytes), numbytes); */
	bp += numbytes;
    return bp - buffp;
}
