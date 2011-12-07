/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 */

#define NULLPTR (char *) 0

char *
index(sp, c)
register char *sp, c;
{
	do {
		if (*sp == c)
			return(sp);
	} while (*sp++);
    return(NULLPTR);
}
