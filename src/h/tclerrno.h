/*  errno.h

    Defines the system error variable errno and the error
    numbers set by system calls. Errors which exist in Unix(tm)
    but not MSDOS have value -1.

    Copyright (c) 1987, 1991 by Borland International
    All Rights Reserved.
*/

#ifndef __ERRNO_H
#define __ERRNO_H

/*  Dos Error Codes */

#define EZERO    0      /* Error 0                  */
#define EINVFNC  1      /* Invalid function number  */
#define ENOPATH  3      /* Path not found           */
#define ECONTR   7      /* Memory blocks destroyed  */
#define EINVMEM  9      /* Invalid memory block address */
#define EINVENV 10      /* Invalid environment      */
#define EINVFMT 11      /* Invalid format           */
#define EINVACC 12      /* Invalid access code      */
#define EINVDAT 13      /* Invalid data             */
#define ECURDIR 16      /* Attempt to remove CurDir */
#define ENOTSAM 17      /* Not same device          */
#define ENMFILE 18      /* No more files            */

#define ENOENT   2      /* No such file or directory*/
#define EMFILE   4      /* Too many open files      */
#define EACCES   5      /* Permission denied        */
#define EBADF    6      /* Bad file number          */
#define ENOMEM   8      /* Not enough core          */
#define ENODEV  15      /* No such device           */
#define EINVAL  19      /* Invalid argument         */
#define E2BIG   20      /* Arg list too long        */
#define ENOEXEC 21      /* Exec format error        */
#define EXDEV   22      /* Cross-device link        */
#define EDOM    33      /* Math argument            */
#define ERANGE  34      /* Result too large         */
#define EEXIST  35      /* File already exists      */
#define EDEADLOCK 36    /* Locking violation        */
#define ECHILD  -1      /* Unix/DOS                 */

#ifdef __OS2__

extern  int * _Cdecl __errno(void);
extern  int * _Cdecl __doserrno(void);
#define errno (*__errno())
#define _doserrno (*__doserrno())

#else

#if !defined( _RTLDLL )
extern  int _Cdecl  errno;
extern  int _Cdecl  _doserrno;
#else

#ifdef __cplusplus
extern "C" {
#endif
int far * far _Cdecl __getErrno(void);
int far * far _Cdecl __getDOSErrno(void);
#ifdef __cplusplus
}
#endif
#define errno (*__getErrno())

#if !defined(__STDC__)
    #define _doserrno (*__getDOSErrno())
#endif

#endif

#endif

#if !__STDC__
#define _sys_nerr   35      /* highest defined system error number */
#endif

#endif
