/* config.h.  Generated by hand for Linux. */
#ifndef CONFIG_H
#define CONFIG_H

/* Define if threads exist (using pthreads or Win32 threads) */
#define HAVE_POSIX_THREAD 1
/* #undef HAVE_WIN32_THREAD 1 */
#define _REENTRANT 1

/* Define if on OSF1 and need special extern "C" around some header files */
/* #undef SPECIAL_OSF1_EXTERN */

/* #undef HAVE_THREAD */

/* Define if the strings.h header file exists */
#define HAVE_STRINGS_H 1

#define HAVE_LIBINTL_H 1

/* Define the intXX_t, u_intXX_t, size_t, ssize_t, and socklen_t types */
/* On the Cray J90 there is no 4 byte integer, so we define int32_t
 * but it is 8 bytes, and we leave HAVE_INT32_T undefined. */

#undef HAVE_INT32_T

/* socklen_t usually defined in <sys/socket.h>. Unfortunately it doesn't
 * work on some systems (like DEC OSF/1), so we'll use our own Socklen_t */
#define Socklen_t socklen_t

/* Define if you have these functions. */
#define HAVE_SNPRINTF 1
#define HAVE_INET_PTON 1
#define HAVE_INET_NTOP 1
#define HAVE_GETTIMEOFDAY 1
#define HAVE_PTHREAD 1
/* #undef HAVE_PTHREAD_CANCEL */
#define HAVE_QUAD_SUPPORT 1
/* #undef HAVE_PRINTF_QD */

/* standard C++, which isn't always... */
#define bool int
/* #define true   1 */
/* #define false  0 */

/* Define if the host is Big Endian (network byte order) */
/* #undef WORDS_BIGENDIAN */

/* Define if multicast support exists */
#define HAVE_MULTICAST 1

/* Define if all IPv6 headers/structures are present */
#define HAVE_IPV6 1

/* Define if IPv6 multicast support exists */
#ifdef IPV6_ADD_MEMBERSHIP
#define HAVE_IPV6_MULTICAST 1
#endif

/* Define if Debugging of sockets is desired */
/* #undef DBG_MJZ */

#endif /* CONFIG_H */
