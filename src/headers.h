/*---------------------------------------------------------------
 * All system headers required by iperf.
 * This could be processed to form a single precompiled header,
 * to avoid overhead of compiling it multiple times.
 * This also verifies a few things are defined, for portability.
 * ------------------------------------------------------------------- */

#ifndef HEADERS_H
#define HEADERS_H

#include "version.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32
#include "config.h"

#define _WIN32_WINNT 0x0400 /* use (at least) WinNT 4.0 API */
#define WIN32_LEAN_AND_MEAN /* exclude unnecesary headers */
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <tchar.h>

#endif

/* turn off assert debugging */
#ifndef NDEBUG
#define NDEBUG
#endif

/* standard C headers */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <math.h>

#ifdef WIN32
	struct itimerval { struct timeval it_interval; struct timeval it_value; };
	#define IFNAMSIZ 32
	#define ENOBUFS 55

/* define EINTR, just to help compile; it isn't useful */
    #ifndef EINTR
        #define EINTR  WSAEINTR
    #endif // EINTR

/* Visual C++ has INT64, but not 'long long'.
 * Metrowerks has 'long long', but INT64 doesn't work. */
    #ifdef __MWERKS__
        #define int64_t  long long
    #else
        #define int64_t  INT64
    #endif // __MWERKS__

/* Visual C++ has _snprintf instead of snprintf */
    #ifndef __MWERKS__
        #define snprintf _snprintf
    #endif // __MWERKS__

/* close, read, and write only work on files in Windows.
 * I get away with #defining them because I don't read files. */
    #define close( s )       closesocket( s )
    #define read( s, b, l )  recv( s, (char*) b, l, 0 )
    #define write( s, b, l ) send( s, (char*) b, l, 0 )

#else /* not defined WIN32 */

/* required on AIX for FD_SET (requires bzero).
 * often this is the same as <string.h> */
    #ifdef HAVE_STRINGS_H
        #include <strings.h>
    #endif // HAVE_STRINGS_H

/* unix headers */
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <sys/time.h>
    #include <signal.h>
    #include <unistd.h>

/** Added for daemonizing the process */
    #include <syslog.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>   /* netinet/in.h must be before this on SunOS */
    #include <net/if.h>

#ifdef HAVE_POSIX_THREAD
    #include <pthread.h>
#endif // HAVE_POSIX_THREAD

/* used in Win32 for error checking,
 * rather than checking rc < 0 as unix usually does */
    #define SOCKET_ERROR   -1
    #define INVALID_SOCKET -1

#endif /* not defined WIN32 */

#ifndef INET6_ADDRSTRLEN
#    define INET6_ADDRSTRLEN 40
#endif
#ifndef INET_ADDRSTRLEN
#    define INET_ADDRSTRLEN 15
#endif

#ifdef HAVE_IPV6
#    define REPORT_ADDRLEN (INET6_ADDRSTRLEN + 1)
typedef struct sockaddr_storage iperf_sockaddr;
#else
#    define REPORT_ADDRLEN (INET_ADDRSTRLEN + 1)
typedef struct sockaddr_in iperf_sockaddr;
#endif

// Rationalize stdint definitions and sizeof, thanks to ac_create_stdint_h.m4
// from the gnu archive

#include "iperf-int.h"
#ifdef WIN32
typedef u_int32_t max_size_t;
#else
typedef uint64_t max_size_t;
#endif

/* in case the OS doesn't have these, we provide our own implementations */
#include "gettimeofday.h"
#include "inet_aton.h"
#include "snprintf.h"

#ifndef SHUT_RD
    #define SHUT_RD   0
    #define SHUT_WR   1
    #define SHUT_RDWR 2
#endif // SHUT_RD

#endif /* HEADERS_H */






