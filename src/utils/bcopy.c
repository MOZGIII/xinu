



bcopy(src, dst, length)
	register char *src, *dst;
	register int length;
{
	long *t1, *t2;
	
	if (length && src != dst)
		if ((unsigned)dst < (unsigned)src)
			if (((int)src | (int)dst | length) & 3)
				do	/* copy by bytes */
					*dst++ = *src++;
				while (--length);
			else {
				length >>= 2;
				do	/* copy by longs */
				{
				    long *t1 = (long*)dst;
				    long *t2 = (long*)src;
				    *t1 = *t2;
				    dst = (char*)((long*)dst + 1);
				    src = (char*)((long*)src + 1);
				}
				while (--length);
			}
		else {			/* copy backwards */
			src += length;
			dst += length;
			if (((int)src | (int)dst | length) & 3)
				do	/* copy by bytes */
					*--dst = *--src;
				while (--length);
			else {
				length >>= 2;
				do{	/* copy by shorts */
					dst = (char*)((long*)dst - 1);
					src = (char*)((long*)src - 1);
				
					t1 = (long*)dst;
					t2 = (long*)src;
					*t1 = *t2;
				}while (--length);
			}
		}
	return(0);
}
