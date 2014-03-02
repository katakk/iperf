/*---------------------------------------------------------------
 * An abstract class for locking a mutex (mutual exclusion). If
 * threads are not available, this does nothing.
 * ------------------------------------------------------------------- */

#ifndef MUTEX_H
#define MUTEX_H

#include "headers.h"

#if   defined( HAVE_POSIX_THREAD )
    typedef pthread_mutex_t Mutex;
#elif defined( HAVE_WIN32_THREAD )
    typedef HANDLE Mutex;
#else
    typedef int Mutex;
#endif

/* ------------------------------------------------------------------- */
    // initialize mutex
#if   defined( HAVE_POSIX_THREAD )
    #define Mutex_Initialize( MutexPtr ) pthread_mutex_init( MutexPtr, NULL )
#elif defined( HAVE_WIN32_THREAD )
    #define Mutex_Initialize( MutexPtr ) *MutexPtr = CreateMutex( NULL, false, NULL )
#else
    #define Mutex_Initialize( MutexPtr )
#endif

    // lock the mutex variable
#if   defined( HAVE_POSIX_THREAD )
    #define Mutex_Lock( MutexPtr ) pthread_mutex_lock( MutexPtr )
#elif defined( HAVE_WIN32_THREAD )
    #define Mutex_Lock( MutexPtr ) WaitForSingleObject( *MutexPtr, INFINITE )
#else
    #define Mutex_Lock( MutexPtr )
#endif

    // unlock the mutex variable
#if   defined( HAVE_POSIX_THREAD )
    #define Mutex_Unlock( MutexPtr ) pthread_mutex_unlock( MutexPtr )
#elif defined( HAVE_WIN32_THREAD )
    #define Mutex_Unlock( MutexPtr ) ReleaseMutex( *MutexPtr )
#else
    #define Mutex_Unlock( MutexPtr )
#endif

    // destroy, making sure mutex is unlocked
#if   defined( HAVE_POSIX_THREAD )
    #define Mutex_Destroy( MutexPtr )  do {         \
        int rc = pthread_mutex_destroy( MutexPtr ); \
        if ( rc == EBUSY ) {                        \
            Mutex_Unlock( MutexPtr );               \
            pthread_mutex_destroy( MutexPtr );      \
        }                                           \
    } while ( 0 )
#elif defined( HAVE_WIN32_THREAD )
    #define Mutex_Destroy( MutexPtr ) CloseHandle( *MutexPtr )
#else
    #define Mutex_Destroy( MutexPtr )
#endif

#endif // MUTEX_H
