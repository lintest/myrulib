/*
 * Merge the different 64-bit file API's
 * of Linux, Cygwin, and MinGW.
 */

#if defined(__MINGW32__)
# ifndef __MSVCRT__
#  define __MSVCRT__
# endif
# define __MSVCRT_VERSION__ 0x0601
# define stat64 __stat64
# define __stat64(x, y) _stat64(x, y)
# define fstat64 _fstat64
# define lstat64 _stat64
# define __STDC_FORMAT_MACROS
# define open64 open
#elif defined(__CYGWIN__)
# define __CYGWIN_USE_BIG_TYPES__
# define stat64 stat
# define fstat64 fstat
# define lstat64 lstat
# define off64_t off_t
# define open64 open
# define lseek64 lseek
# define fpos64_t fpos_t
# define fopen64 fopen
# define fdopen64 fdopen
# define ftello64 ftello
# define fseeko64 fseeko
# define fgetpos64 fgetpos
# define fsetpos64 fsetpos
# define tmpfile64 tmpfile
#else
# define _LARGEFILE64_SOURCE
#endif
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/*
 * Now use these:
 *  struct stat64
 *  stat64()
 *  fstat64()
 *  lstat64()
 *  off64_t
 *  open64()
 *  lseek64()
 *  fpos64_t
 *  fopen64()
 *  fdopen64()
 *  ftello64()
 *  fseeko64()
 *  fgetpos64()
 *  fsetpos64()
 *  tmpfile64()
 */

