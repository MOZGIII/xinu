/*	stdio.h

	Definitions for stream input/output.

        Copyright (c) Borland International 1987,1988,1990
	All Rights Reserved.
    
******* modified for XINU ********
*/

#ifndef __STDIO_H
#define __STDIO_H

#if __STDC__
#define _Cdecl
#else
#define _Cdecl	cdecl
#endif

#ifndef __PAS__
#define _CType _Cdecl
#else
#define _CType pascal
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned size_t;
#endif

/*#ifndef NULL
#if defined(__TINY__) || defined(__SMALL__) || defined(__MEDIUM__)
#define	NULL	0
#else
#define	NULL	0L
#endif
#endif*/

/* Definition of the file position type
*/
typedef long	fpos_t;

/* Definition of the control structure for streams
*/
typedef struct	{
        short       	level;          /* fill/empty level of buffer */
        unsigned        flags;          /* File status flags          */
        char            fd;             /* File descriptor            */
        unsigned char   hold;           /* Ungetc char if no buffer   */
        short           bsize;          /* Buffer size                */
        unsigned char   *buffer;        /* Data transfer buffer       */
        unsigned char   *curp;          /* Current active pointer     */
        unsigned        istemp;         /* Temporary file indicator   */
        short           token;          /* Used for validity checking */
}       FILE;                           /* This is the FILE object    */

/* Bufferisation type to be used as 3rd argument for "setvbuf" function
*/
#define _IOFBF	0
#define _IOLBF	1
#define _IONBF	2

/*	"flags" bits definitions
*/
#define _F_RDWR 0x0003                  /* Read/write flag       */
#define _F_READ 0x0001                  /* Read only file        */
#define _F_WRIT 0x0002                  /* Write only file       */
#define _F_BUF	0x0004			/* Malloc'ed Buffer data */
#define _F_LBUF 0x0008                  /* line-buffered file    */
#define _F_ERR  0x0010                  /* Error indicator       */
#define _F_EOF  0x0020                  /* EOF indicator         */
#define _F_BIN	0x0040			/* Binary file indicator */
#define _F_IN   0x0080                  /* Data is incoming      */
#define _F_OUT  0x0100                  /* Data is outgoing      */
#define _F_TERM 0x0200                  /* File is a terminal    */

/* End-of-file constant definition
*/
/* XINU
#define EOF	(-1)			/* End of file indicator */

/* Number of files that can be open simultaneously
*/
#if __STDC__
#define FOPEN_MAX 18        /* Able to have 18 files (20 - stdaux & stdprn) */
#else
#define FOPEN_MAX 20        /* Able to have 20 files */
#define SYS_OPEN  20
#endif

#define FILENAME_MAX 80

/* Default buffer size use by "setbuf" function
*/
#define BUFSIZ	512			/* Buffer size for stdio */

/* Size of an arry large enough to hold a temporary file name string
*/
#define L_ctermid	5		/* CON: plus null byte */
#define L_tmpnam	13		/* tmpnam buffer size */

/* Constants to be used as 3rd argument for "fseek" function
*/
#define SEEK_CUR	1
#define SEEK_END	2
#define SEEK_SET	0

/* Number of unique file names that shall be generated by "tmpnam" function
*/
#define TMP_MAX 	0xFFFF

/* Standard I/O predefined streams
*/
extern	FILE	_Cdecl _streams[];

#define xinu_stdin	(&_streams[0])
#define xinu_stdout	(&_streams[1])
#define xinu_stderr	(&_streams[2])

#if !__STDC__
#define stdaux	(&_streams[3])
#define stdprn	(&_streams[4])
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* XINU

void    _Cdecl clearerr (FILE *__stream);
int	    _Cdecl fclose   (FILE *__stream);
int	    _Cdecl fflush	(FILE *__stream);
int	    _Cdecl fgetc	(FILE *__stream);
int	    _Cdecl fgetpos  (FILE *__stream, fpos_t *__pos);
char   *_Cdecl fgets	(char *__s, int __n, FILE *__stream);
FILE   *_Cdecl fopen	(const char *__path, const char *__mode);
int	    _Cdecl fprintf  (FILE *__stream, const char *__format, ...);
int	    _Cdecl fputc	(int __c, FILE *__stream);
int	    _Cdecl fputs	(const char *__s, FILE *__stream);
size_t	_Cdecl fread	(void *__ptr, size_t __size, size_t __n, FILE *__stream);
FILE   *_Cdecl freopen  (const char *__path, const char *__mode, FILE *__stream);
int	    _Cdecl fscanf	(FILE *__stream, const char *__format, ...);
int	    _Cdecl fseek	(FILE *__stream, long __offset, int __whence);
int	    _Cdecl fsetpos  (FILE *__stream, const fpos_t *__pos);
long	_Cdecl ftell	(FILE *__stream);
size_t	_Cdecl fwrite	(const void *__ptr, size_t __size, size_t __n,
		            	 FILE *__stream);
char   *_Cdecl gets	    (char *__s);
void    _Cdecl perror	(const char *__s);
int	    _Cdecl printf	(const char *__format, ...);
int	    _Cdecl puts	    (const char *__s);
int	    _Cdecl rename	(const char *__oldname, const char *__newname);
void    _Cdecl rewind   (FILE *__stream);
int	    _Cdecl scanf	(const char *__format, ...);
void	_Cdecl setbuf	(FILE *__stream, char *__buf);
int	    _Cdecl setvbuf  (FILE *__stream, char *__buf, int __type, size_t __size);
int	    _Cdecl sprintf  (char *__buffer, const char *__format, ...);
int	    _Cdecl sscanf	(const char *__buffer, const char *__format, ...);
char   *_Cdecl strerror (int __errnum);
FILE   *_Cdecl tmpfile  (void);
char   *_Cdecl tmpnam	(char *__s);
int	    _Cdecl ungetc	(int __c, FILE *__stream);
int	    _Cdecl vfprintf (FILE *__stream, const char *__format, void *__arglist);
int	    _Cdecl vfscanf  (FILE *__stream, const char *__format, void *__arglist);
int     _CType vprintf  (const char *__format, void *__arglist);
int	    _Cdecl vscanf   (const char *__format, void *__arglist);
int	    _Cdecl vsprintf (char *__buffer, const char *__format, void *__arglist);
int	    _Cdecl vsscanf  (const char *__buffer, const char *__format, void *__arglist);
int	    _CType unlink	(const char *__path);
int     _Cdecl getc     (FILE *__fp);
int     _Cdecl getchar  (void);
int     _Cdecl putc     (const int __c, FILE *__fp);
int     _Cdecl putchar  (const int __c);
int     _Cdecl feof     (FILE *__fp);
int     _Cdecl ferror   (FILE *__fp);


#if !__STDC__
int	    _Cdecl fcloseall(void);
FILE   *_Cdecl fdopen	(int __handle, char *__type);
int	    _Cdecl fgetchar (void);
int	    _Cdecl flushall (void);
int	    _Cdecl fputchar (int __c);
int	    _Cdecl getw	    (FILE *__stream);
int	    _Cdecl putw	    (int __w, FILE *__stream);
char   *_Cdecl _strerror(const char *__s);
#define fileno(f)	    ((f)->fd)
#endif
*/
int      _Cdecl _fgetc   (FILE *__stream);              /* used by getc() macro */
int      _Cdecl _fputc   (char __c, FILE *__stream);    /* used by putc() macro */
#ifdef  __cplusplus
}
#endif

/*	The following macros provide for common functions */

#define ferror(f)	((f)->flags & _F_ERR)
#define feof(f) 	((f)->flags & _F_EOF)

#ifdef  __cplusplus
inline int remove(const char *__path) { return unlink(__path); }
#else
int     _Cdecl remove   (const char *__c);
#define remove(path)	unlink(path)
#endif

/* XINU

#define getc(f) \
  ((--((f)->level) >= 0) ? (unsigned char)(*(f)->curp++) : \
	_fgetc (f))

#define putc(c,f) \
  ((++((f)->level) < 0) ? (unsigned char)(*(f)->curp++=(c)) : \
	_fputc ((c),f))

#define getchar()  getc(xinu_stdin)
#define putchar(c) putc((c), xinu_stdout)
*/
#define ungetc(c,f)	ungetc((c),f)	/* traditionally a macro */

#endif

