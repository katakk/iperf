/*---------------------------------------------------------------
 * An abstract class for waiting on a condition variable. If
 * threads are not available, this does nothing.
 * ------------------------------------------------------------------- */

#ifndef CONDITION_H
#define CONDITION_H

#include "headers.h"
#include "Mutex.h"
#include "util.h"

#if   defined( HAVE_POSIX_THREAD )
typedef struct Condition {
    pthread_cond_t mCondition;
    pthread_mutex_t mMutex;
} Condition;
#elif defined( HAVE_WIN32_THREAD )
typedef struct Condition {
    HANDLE mCondition;
    HANDLE mMutex;
} Condition;
#else
typedef struct Condition {
    int mCondition;
    int mMutex;
} Condition;
#endif

#define Condition_Lock( Cond ) Mutex_Lock( &Cond.mMutex )

#define Condition_Unlock( Cond ) Mutex_Unlock( &Cond.mMutex )

    // initialize condition
#if   defined( HAVE_POSIX_THREAD )
    #define Condition_Initialize( Cond ) do {             \
        Mutex_Initialize( &(Cond)->mMutex );              \
        pthread_cond_init( &(Cond)->mCondition, NULL );   \
    } while ( 0 )
#elif defined( HAVE_WIN32_THREAD )
    // set all conditions to be broadcast
    // unfortunately in Win32 you have to know at creation
    // whether the signal is broadcast or not.
    #define Condition_Initialize( Cond ) do {                         \
        Mutex_Initialize( &(Cond)->mMutex );                          \
        (Cond)->mCondition = CreateEvent( NULL, true, false, NULL );  \
    } while ( 0 )
#else
    #define Condition_Initialize( Cond )
#endif

    // destroy condition
#if   defined( HAVE_POSIX_THREAD )
    #define Condition_Destroy( Cond ) do {            \
        pthread_cond_destroy( &(Cond)->mCondition );  \
        Mutex_Destroy( &(Cond)->mMutex );             \
    } while ( 0 )
#elif defined( HAVE_WIN32_THREAD )
    #define Condition_Destroy( Cond ) do {            \
        CloseHandle( (Cond)->mCondition );            \
        Mutex_Destroy( &(Cond)->mMutex );             \
    } while ( 0 )
#else
    #define Condition_Destroy( Cond )
#endif

    // sleep this thread, waiting for condition signal
#if   defined( HAVE_POSIX_THREAD )
    #define Condition_Wait( Cond ) pthread_cond_wait( &(Cond)->mCondition, &(Cond)->mMutex )
    #define Condition_Wait_Event( Cond ) do { \
        Mutex_Lock( &(Cond)->mMutex ); \
        pthread_cond_wait( &(Cond)->mCondition, &(Cond)->mMutex ); \
        Mutex_Unlock( &(Cond)->mMutex ); \
    } while( 0 )
#elif defined( HAVE_WIN32_THREAD )
    // atomically release mutex and wait on condition,
    // then re-acquire the mutex
    #define Condition_Wait( Cond ) do {                                         \
        SignalObjectAndWait( (Cond)->mMutex, (Cond)->mCondition, INFINITE, false ); \
        Mutex_Lock( &(Cond)->mMutex );                          \
    } while ( 0 )
    #define Condition_Wait_Event( Cond ) do { \
        Mutex_Lock( &(Cond)->mMutex );                          \
        SignalObjectAndWait( (Cond)->mMutex, (Cond)->mCondition, INFINITE, false ); \
    } while ( 0 )
#else
    #define Condition_Wait( Cond )
#endif

    // sleep this thread, waiting for condition signal,
    // but bound sleep time by the relative time inSeconds.
#if   defined( HAVE_POSIX_THREAD )
    #define Condition_TimedWait( Cond, inSeconds ) do {                         \
        struct timespec absTimeout;                                             \
        absTimeout.tv_sec  = time( NULL ) + inSeconds;                          \
        absTimeout.tv_nsec = 0;                                                 \
       pthread_cond_timedwait( &(Cond)->mCondition, &(Cond)->mMutex, &absTimeout ); \
    } while ( 0 )
#elif defined( HAVE_WIN32_THREAD )
    // atomically release mutex and wait on condition,
    // then re-acquire the mutex
    #define Condition_TimedWait( Cond, inSeconds ) do {                         \
        SignalObjectAndWait( (Cond)->mMutex, (Cond)->mCondition, inSeconds*1000, false ); \
        Mutex_Lock( &(Cond)->mMutex );                          \
    } while ( 0 )
#else
    #define Condition_TimedWait( Cond, inSeconds )
#endif

    // send a condition signal to wake one thread waiting on condition
    // in Win32, this actually wakes up all threads, same as Broadcast
    // use PulseEvent to auto-reset the signal after waking all threads
#if   defined( HAVE_POSIX_THREAD )
    #define Condition_Signal( Cond ) pthread_cond_signal( &(Cond)->mCondition )
#elif defined( HAVE_WIN32_THREAD )
    #define Condition_Signal( Cond ) PulseEvent( (Cond)->mCondition )
#else
    #define Condition_Signal( Cond )
#endif

    // send a condition signal to wake all threads waiting on condition
#if   defined( HAVE_POSIX_THREAD )
    #define Condition_Broadcast( Cond ) pthread_cond_broadcast( &(Cond)->mCondition )
#elif defined( HAVE_WIN32_THREAD )
    #define Condition_Broadcast( Cond ) PulseEvent( (Cond)->mCondition )
#else
    #define Condition_Broadcast( Cond )
#endif

#endif // CONDITION_H
