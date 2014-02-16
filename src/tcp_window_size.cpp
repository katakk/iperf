/*---------------------------------------------------------------
 * set/getsockopt
 * ------------------------------------------------------------------- */

#define HEADERS()

#include "headers.h"

#include "util.h"

/* -------------------------------------------------------------------
 * If inTCPWin > 0, set the TCP window size (via the socket buffer
 * sizes) for inSock. Otherwise leave it as the system default.
 *
 * This must be called prior to calling listen() or connect() on
 * the socket, for TCP window sizes > 64 KB to be effective.
 *
 * This now works on UNICOS also, by setting TCP_WINSHIFT.
 * This now works on AIX, by enabling RFC1323.
 * returns -1 on error, 0 on no error.
 * ------------------------------------------------------------------- */

int setsock_tcp_windowsize( int inSock, int inTCPWin, int inSend ) {
#ifdef SO_SNDBUF
    int rc;
    int newTCPWin;

    assert( inSock >= 0 );

    if ( inTCPWin > 0 ) {

#ifdef TCP_WINSHIFT

        /* UNICOS requires setting the winshift explicitly */
        if ( inTCPWin > 65535 ) {
            int winShift = 0;
            int scaledWin = inTCPWin >> 16;
            while ( scaledWin > 0 ) {
                scaledWin >>= 1;
                winShift++;
            }

            /* set TCP window shift */
            rc = setsockopt( inSock, IPPROTO_TCP, TCP_WINSHIFT,
                             (char*) &winShift, sizeof( winShift ));
            if ( rc < 0 ) {
                return rc;
            }

            /* Note: you cannot verify TCP window shift, since it returns
             * a structure and not the same integer we use to set it. (ugh) */
        }
#endif /* TCP_WINSHIFT  */

#ifdef TCP_RFC1323
        /* On AIX, RFC 1323 extensions can be set system-wide,
         * using the 'no' network options command. But we can also set them
         * per-socket, so let's try just in case. */
        if ( inTCPWin > 65535 ) {
            /* enable RFC 1323 */
            int on = 1;
            rc = setsockopt( inSock, IPPROTO_TCP, TCP_RFC1323,
                             (char*) &on, sizeof( on ));
            if ( rc < 0 ) {
                return rc;
            }
        }
#endif /* TCP_RFC1323 */

        if ( !inSend ) {
            /* receive buffer -- set
             * note: results are verified after connect() or listen(),
             * since some OS's don't show the corrected value until then. */
            newTCPWin = inTCPWin;
            rc = setsockopt( inSock, SOL_SOCKET, SO_RCVBUF,
                             (char*) &newTCPWin, sizeof( newTCPWin ));
        } else {
            /* send buffer -- set
             * note: results are verified after connect() or listen(),
             * since some OS's don't show the corrected value until then. */
            newTCPWin = inTCPWin;
            rc = setsockopt( inSock, SOL_SOCKET, SO_SNDBUF,
                             (char*) &newTCPWin, sizeof( newTCPWin ));
        }
        if ( rc < 0 ) {
            return rc;
        }
    }
#endif /* SO_SNDBUF */

    return 0;
} /* end setsock_tcp_windowsize */

/* -------------------------------------------------------------------
 * returns the TCP window size (on the sending buffer, SO_SNDBUF),
 * or -1 on error.
 * ------------------------------------------------------------------- */

int getsock_tcp_windowsize( int inSock, int inSend ) {
    int theTCPWin = 0;

#ifdef SO_SNDBUF
    int rc;
    Socklen_t len;

    /* send buffer -- query for buffer size */
    len = sizeof( theTCPWin );
    if ( inSend ) {
        rc = getsockopt( inSock, SOL_SOCKET, SO_SNDBUF,
                         (char*) &theTCPWin, &len );
    } else {
        rc = getsockopt( inSock, SOL_SOCKET, SO_RCVBUF,
                         (char*) &theTCPWin, &len );
    }
    if ( rc < 0 ) {
        return rc;
    }

#endif

    return theTCPWin;
} /* end getsock_tcp_windowsize */

