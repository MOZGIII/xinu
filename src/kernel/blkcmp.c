/* blkcmp.c - blkcmp */

/*------------------------------------------------------------------------
 * blkcmp - do a lexical compare of a block of len bytes
 *------------------------------------------------------------------------
 */
int blkcmp(s1, s2, len)
unsigned char	*s1, *s2;
int		len;
{
	int 	i, rv = 0;

	for (i = 0; i < len; ++i, ++s1, ++s2)
		if (rv = *s1 - *s2)
			break;
	return rv;
}
