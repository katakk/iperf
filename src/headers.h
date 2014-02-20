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


    // find Probider
    static WSAPROTOCOL_INFO *FindProtocolInfo(int af, int type, int protocol, DWORD flags)
    {
        WSAPROTOCOL_INFO *buf=NULL;
        static WSAPROTOCOL_INFO pinfo;

        DWORD size = 0;
        DWORD num;

        WSAEnumProtocols(NULL, NULL, &size);
        buf = (WSAPROTOCOL_INFO *)LocalAlloc(LPTR, size);
        num = size / sizeof(WSAPROTOCOL_INFO);

        WSAEnumProtocols(NULL, buf, &size);

        for(DWORD i=0; i < num ;i++)
        {
            if ((buf[i].iAddressFamily == af) &&
                (buf[i].iSocketType == type) &&
                (buf[i].iProtocol == protocol))
            {
                if ((buf[i].dwServiceFlags1 & flags) == flags)
                {
                    memcpy(&pinfo, &buf[i], sizeof(WSAPROTOCOL_INFO));
                    LocalFree(buf);
                    return &pinfo;
                }

            }

        }

        LocalFree(buf);
        return NULL;
    }

#ifdef HAVE_IPV6
#define WIN32SocketDomain(setting) (SockAddr_isIPv6( &setting->local ) ? AF_INET6 : AF_INET )
#else
#define WIN32SocketDomain(setting) (SockAddr_isIPv6( &setting->local ) ? AF_INET : AF_INET )
#endif

#define WIN32Socket(setting) \
        WSASocket(FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, \
        FindProtocolInfo( WIN32SocketDomain(setting), \
        (isUDP( setting )  ?  SOCK_DGRAM  : SOCK_STREAM), \
        (isUDP( setting )  ?  IPPROTO_UDP : IPPROTO_TCP), \
        ( setting->mTOS ) ? XP1_QOS_SUPPORTED : 0 ), \
        0, \
        (SockAddr_isMulticast( &setting->local ) ? \
            WSA_FLAG_MULTIPOINT_C_LEAF | WSA_FLAG_MULTIPOINT_D_LEAF : 0 ) \
        )

/* close, read, and write only work on files in Windows.
 * I get away with #defining them because I don't read files. */
    #define close( s )       closesocket( s )

    /* WSA 1.1 */
    /* #define read( s, b, l )  recv( s, (char*) b, l, 0 ) */
    /* #define write( s, b, l ) send( s, (char*) b, l, 0 ) */

    /* WSA 2.2 */
    static int read( SOCKET s, void * b, size_t l )
    {
        WSABUF RecvData;
        DWORD  dwBytesRecv;
        RecvData.buf = (char*) b;
        RecvData.len = (u_long) l;
        WSARecv(s, &RecvData, sizeof(RecvData), &dwBytesRecv, 0, NULL, NULL);
        return dwBytesRecv;
    }

    static int write( SOCKET s, void * b, size_t l )
    {
        WSABUF SendData;
        DWORD  dwBytesSent;
        SendData.buf = (char*) b;
        SendData.len = (u_long) l;
        WSASend(s, &SendData, 1, &dwBytesSent, 0, NULL, NULL);
        return dwBytesSent;
    }
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
#ifndef WIN32
#define _STDINT_HAVE_STDINT_H 1
#include <stdint.h>
#endif

#ifdef WIN32
typedef unsigned __int64 max_size_t;
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






