/*---------------------------------------------------------------
 * Has routines the Client and Server classes use in common for
 * performance testing the network.
 * ------------------------------------------------------------------- */

#define HEADERS()

#include "headers.h"

#include "PerfSocket.hpp"
#include "SocketAddr.h"
#include "util.h"

/* -------------------------------------------------------------------
 * Set socket options before the listen() or connect() calls.
 * These are optional performance tuning factors.
 * ------------------------------------------------------------------- */

void SetSocketOptions( thread_Settings *inSettings ) {
    // set the TCP window size (socket buffer sizes)
    // also the UDP buffer size
    // must occur before call to accept() for large window sizes
    setsock_tcp_windowsize( inSettings->mSock, inSettings->mTCPWin,
                            (inSettings->mThreadMode == kMode_Client ? 1 : 0) );

    if ( isCongestionControl( inSettings ) ) {
#ifdef TCP_CONGESTION
    Socklen_t len = strlen( inSettings->mCongestion ) + 1;
    int rc = setsockopt( inSettings->mSock, IPPROTO_TCP, TCP_CONGESTION,
                 inSettings->mCongestion, len);
    if (rc == SOCKET_ERROR ) {
        fprintf(stderr, "Attempt to set '%s' congestion control failed: %s\n",
            inSettings->mCongestion, strerror(errno));
        exit(1);
    }
#else
    fprintf( stderr, "The -Z option is not available on this operating system\n");
#endif
    }

    // check if we're sending multicast, and set TTL
    if ( isMulticast( inSettings ) && ( inSettings->mTTL > 0 ) ) {
    int val = inSettings->mTTL;
#ifdef HAVE_MULTICAST
    if ( !SockAddr_isIPv6( &inSettings->local ) ) {
        int rc = setsockopt( inSettings->mSock, IPPROTO_IP, IP_MULTICAST_TTL,
            (const char*) &val, (Socklen_t) sizeof(val));

        WARN_errno( rc == SOCKET_ERROR, "multicast ttl" );
        if(isCustInterface ( inSettings ) ) {
            int fd = socket(AF_INET, SOCK_DGRAM, 0);
            struct ifreq ifr;
            ifr.ifr_addr.sa_family = AF_INET;
            strncpy(ifr.ifr_name, inSettings->mCustInterface, IFNAMSIZ-1);
            if(ioctl(fd, SIOCGIFADDR, &ifr) != -1)
            {
                struct sockaddr_in *int_addr = (struct sockaddr_in *)&ifr.ifr_addr;
                setsockopt(inSettings->mSock, IPPROTO_IP, IP_MULTICAST_IF, &int_addr->sin_addr, sizeof(int_addr->sin_addr));
            }
            close(fd);
        }
    }
#ifdef HAVE_IPV6_MULTICAST
    else {
        int rc = setsockopt( inSettings->mSock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS,
            (const void*) &val, (Socklen_t) sizeof(val));
        WARN_errno( rc == SOCKET_ERROR, "multicast ttl" );
        
        if(isCustInterface ( inSettings ) ) {
            int fd = socket(AF_INET6, SOCK_DGRAM, 0);
            struct ifreq ifr;
            ifr.ifr_addr.sa_family = AF_INET6;
            strncpy(ifr.ifr_name, inSettings->mCustInterface, IFNAMSIZ-1);
            if(ioctl(fd, SIOCGIFADDR, &ifr) == 0)
            {
                struct sockaddr_in6 *int_addr = (struct sockaddr_in6 *)&ifr.ifr_addr;
                setsockopt(inSettings->mSock, IPPROTO_IPV6, IPV6_MULTICAST_IF, &int_addr->sin6_addr, sizeof(int_addr->sin6_addr));
            }
            close(fd);
        }
    }
#endif
#endif
    }


#ifdef IP_TOS

    // set IP TOS (type-of-service) field
    if ( inSettings->mTOS > 0 ) {
       int  tos = inSettings->mTOS;
       Socklen_t len = sizeof(tos);
       if ( !isIPV6( inSettings ) )
       {
          // for IPv4
          int rc = setsockopt( inSettings->mSock, IPPROTO_IP, IP_TOS,
                               (char*) &tos, len );
          WARN_errno( rc == SOCKET_ERROR, "setsockopt IP_TOS" );
       }
       else
       {
#ifndef IPV6_TCLASS
#define IPV6_TCLASS 67
#endif
          // for IPv6 (traffic class)
          int rc = setsockopt( inSettings->mSock, IPPROTO_IPV6, IPV6_TCLASS,
                               (char*) &tos, len );
          WARN_errno( rc == SOCKET_ERROR, "setsockopt IPV6_TCLASS" );
       }
    }

#endif

#ifdef SO_PRIORITY
    /* From socket(7): "Set the protocol-defined priority for all
     * packets to be sent on this socket. Linux uses this value to order
     * the networking queues: packets with a higher priority may be
     * processed first depending on the selected device queueing
     * discipline. For ip(7), this also sets the IP type-of-service (TOS)
     * field for outgoing packets. Setting a priority outside the range 0
     * to 6 requires the CAP_NET_ADMIN capability." */
    if ( inSettings->mPriority > 0 ) {
        int  priority = inSettings->mPriority;
        Socklen_t len = sizeof(priority);
        int rc = setsockopt( inSettings->mSock, SOL_SOCKET, SO_PRIORITY,
                             (char*) &priority, len );
        WARN_errno( rc == SOCKET_ERROR, "setsockopt SO_PRIORITY" );
    }
#endif

    if ( !isUDP( inSettings ) ) {
        // set the TCP maximum segment size
        setsock_tcp_mss( inSettings->mSock, inSettings->mMSS );

#ifdef TCP_NODELAY

        // set TCP nodelay option
        if ( isNoDelay( inSettings ) ) {
            int nodelay = 1;
            Socklen_t len = sizeof(nodelay);
            int rc = setsockopt( inSettings->mSock, IPPROTO_TCP, TCP_NODELAY,
                                 (char*) &nodelay, len );
            WARN_errno( rc == SOCKET_ERROR, "setsockopt TCP_NODELAY" );
        }
#endif
    }
}
// end SetSocketOptions
