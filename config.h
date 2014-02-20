/* config.h.  Generated by hand for Linux/Windows. */
#ifndef CONFIG_H
#define CONFIG_H

/* Define if threads exist (using pthreads or Win32 threads) */
#ifdef WIN32
#    define _CRT_SECURE_NO_WARNINGS
#    define _WIN32_WINNT 0x0400 /* use (at least) WinNT 4.0 API */
#    define WIN32_LEAN_AND_MEAN /* exclude unnecesary headers */
#    include <winsock2.h>
#    include <ws2tcpip.h>
#    include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#    define HAVE_WIN32_THREAD 1
#else /* WIN32 */
#    define HAVE_POSIX_THREAD 1
#    define HAVE_PTHREAD 1
/* #undef HAVE_PTHREAD_CANCEL */
#    define _REENTRANT 1

#ifndef SOCKET
#define SOCKET int
#endif

#endif /* WIN32 */

/* Define if on OSF1 and need special extern "C" around some header files */
/* #undef SPECIAL_OSF1_EXTERN */

/* Define if the strings.h header file exists */
#ifdef WIN32
/* #undef HAVE_STRINGS_H */
#else /* WIN32 */
#    define HAVE_STRINGS_H 1
#    define HAVE_LIBINTL_H 1
#endif /* WIN32 */

/* Define the intXX_t, u_intXX_t, size_t, ssize_t, and socklen_t types */
/* On the Cray J90 there is no 4 byte integer, so we define int32_t
 * but it is 8 bytes, and we leave HAVE_INT32_T undefined. */
#ifdef WIN32
#    define SIZEOF_INT 4
#    define HAVE_U_INT16_T 1
#    define HAVE_INT32_T   1
#    define HAVE_INT64_T   1
#    define HAVE_U_INT32_T 1

#    define int32_t     LONG32
#    define u_int16_t   UINT16
#    define u_int32_t   ULONG32
/* #undef  size_t */
#    define ssize_t int
#endif /* WIN32 */

/* socklen_t usually defined in <sys/socket.h>. Unfortunately it doesn't
 * work on some systems (like DEC OSF/1), so we'll use our own Socklen_t */
#ifdef WIN32
#    define Socklen_t int
#else /* WIN32 */
#    define Socklen_t socklen_t
#endif /* WIN32 */

/* Define if you have these functions. */
//#define HAVE_SNPRINTF 1
#ifdef WIN32
#    define HAVE_USLEEP 1
#else /* WIN32 */
#    define HAVE_VSNPRINTF    1
#    define HAVE_INET_PTON 1
#    define HAVE_INET_NTOP 1
#    define HAVE_GETTIMEOFDAY 1
#    define HAVE_QUAD_SUPPORT 1
#endif /* WIN32 */

/* standard C++, which isn't always... */
#ifdef WIN32
/* #undef bool */
#    define true   1
#    define false  0
#else /* WIN32 */
#    define bool int
/* #define true   1 */
/* #define false  0 */
#endif /* WIN32 */

/* Define if the host is Big Endian (network byte order) */
/* #undef WORDS_BIGENDIAN */

/* Define if multicast support exists */
#ifdef WIN32
//NO HAVE ICCTL
#else
#    define HAVE_MULTICAST 1
#endif

/* Define if all IPv6 headers/structures are present */
#define HAVE_IPV6 1

/* Define if IPv6 multicast support exists */
#ifdef WIN32
//NO HAVE ICCTL
#else
#    ifdef IPV6_ADD_MEMBERSHIP
#        define HAVE_IPV6_MULTICAST 1
#    endif
#endif

/* Define if Debugging of sockets is desired */
/* #undef DBG_MJZ */

/* /usr/include/netinet/tcp.h */
#ifndef TCP_MAXSEG
#define TCP_MAXSEG      2
#endif

#ifndef SCTP_MAXSEG
#ifdef TCP_MAXSEG
#define SCTP_MAXSEG TCP_MAXSEG
#endif
#endif

#ifndef SOL_SCTP
#define SOL_SCTP        132
#endif

/* IPPROTO_SCTP = 132 is a enum /usr/include/netinet/in.h */
#ifdef WIN32
#define IPPROTO_SCTP 132
#endif

#endif /* CONFIG_H */
