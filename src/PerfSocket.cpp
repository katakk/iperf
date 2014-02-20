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

void SetSocketOptions( thread_Settings *inSettings )
{

#ifdef WIN32
    DWORD dwBytesSent;
#endif /* WIN32 */

/* ------------------> TCP/UDP window size ------------------------> */
    /* set the TCP window size (socket buffer sizes)
     * also the UDP buffer size
     * must occur before call to accept() for large window sizes
     */
    setsock_tcp_windowsize( inSettings->mSock, inSettings->mTCPWin,
                            (inSettings->mThreadMode == kMode_Client ? 1 : 0) );
/* <------------------ TCP window size <------------------------ */

/* ------------------> TCP_CONGESTION ------------------------> */
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
/* <------------------ TCP_CONGESTION <------------------------ */

/* ------------------> TTL ------------------------> */
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

/* <------------------ TTL <------------------------ */

/* ------------------> IP_TOS ------------------------> */
    // set IP TOS (type-of-service) field
    if ( inSettings->mTOS > 0 ) {
        int  tos = inSettings->mTOS;
        Socklen_t len = sizeof(tos);

#ifdef WIN32
//now testing...
//= "0x60 SERVICETYPE_CONTROLLEDLOAD  Controlled Load" 2
//= "0xA0 SERVICETYPE_GUARANTEED      Guaranteed"      3
        inSettings->mQOS.SendingFlowspec.TokenRate = 1;                             /* In Bytes/sec */
        inSettings->mQOS.SendingFlowspec.TokenBucketSize = 1;                       /* In Bytes */
        inSettings->mQOS.SendingFlowspec.PeakBandwidth = 1;                         /* In Bytes/sec */
        inSettings->mQOS.SendingFlowspec.Latency = QOS_NOT_SPECIFIED;               /* In microseconds */
        inSettings->mQOS.SendingFlowspec.DelayVariation = QOS_NOT_SPECIFIED;        /* In microseconds */
        inSettings->mQOS.SendingFlowspec.ServiceType = inSettings->mTOS;
        inSettings->mQOS.SendingFlowspec.MaxSduSize = QOS_NOT_SPECIFIED;            /* In Bytes */
        inSettings->mQOS.SendingFlowspec.MinimumPolicedSize = QOS_NOT_SPECIFIED;    /* In Bytes */

        inSettings->mQOS.ReceivingFlowspec.TokenRate = 1;                           /* In Bytes/sec */
        inSettings->mQOS.ReceivingFlowspec.TokenBucketSize = 1;                     /* In Bytes */
        inSettings->mQOS.ReceivingFlowspec.PeakBandwidth = 1;                       /* In Bytes/sec */
        inSettings->mQOS.ReceivingFlowspec.Latency = QOS_NOT_SPECIFIED;             /* In microseconds */
        inSettings->mQOS.ReceivingFlowspec.DelayVariation = QOS_NOT_SPECIFIED;      /* In microseconds */
        inSettings->mQOS.ReceivingFlowspec.ServiceType = inSettings->mTOS;
        inSettings->mQOS.ReceivingFlowspec.MaxSduSize = QOS_NOT_SPECIFIED;          /* In Bytes */
        inSettings->mQOS.ReceivingFlowspec.MinimumPolicedSize = QOS_NOT_SPECIFIED;  /* In Bytes */

        inSettings->mQOS.ProviderSpecific.buf = NULL;
        inSettings->mQOS.ProviderSpecific.len = 0;

        WSAIoctl(inSettings->mSock, SIO_SET_QOS, &inSettings->mQOS, sizeof(QOS), NULL, 0, &dwBytesSent, NULL, NULL);

#else /* WIN32 */
#    ifdef IP_TOS
       if ( !isIPV6( inSettings ) )
       {
          // for IPv4
          int rc = setsockopt( inSettings->mSock, IPPROTO_IP, IP_TOS,
                               (char*) &tos, len );
          WARN_errno( rc == SOCKET_ERROR, "setsockopt IP_TOS" );
       }
       else
       {
#    ifndef IPV6_TCLASS
#        define IPV6_TCLASS 67
#    endif
          // for IPv6 (traffic class)
          int rc = setsockopt( inSettings->mSock, IPPROTO_IPV6, IPV6_TCLASS,
                               (char*) &tos, len );
          WARN_errno( rc == SOCKET_ERROR, "setsockopt IPV6_TCLASS" );
       }
#    endif /* IP_TOS */
#endif /* WIN32 */
    }
/* <------------------ IP_TOS <------------------------ */

/* ------------------> SO_PRIORITY ------------------------> */
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
/* <------------------ SO_PRIORITY <------------------------ */

/* ------------------> TCP_MAXSEG ------------------------> */
/* ------------------> TCP_NODELAY ------------------------> */

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
/* <------------------ TCP_MAXSEG <------------------------ */
/* <------------------ TCP_NODELAY <------------------------ */
}
// end SetSocketOptions
