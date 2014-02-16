/*---------------------------------------------------------------
 * main does initialization and creates the various objects that will
 * actually run the iperf program, then waits in the Joinall().
 * ------------------------------------------------------------------- */

#define HEADERS()

#include "headers.h"

#include "Settings.hpp"
#include "PerfSocket.hpp"
#include "Locale.h"
#include "Condition.h"
#include "Timestamp.hpp"
#include "Listener.hpp"
#include "List.h"
#include "util.h"

#ifdef WIN32
#include "service.h"
#endif

/* -------------------------------------------------------------------
 * prototypes
 * ------------------------------------------------------------------- */
// Function called at exit to clean up as much as possible
void cleanup( void );

/* -------------------------------------------------------------------
 * global variables
 * ------------------------------------------------------------------- */
// Global flag to signal a user interrupt
int sInterupted = 0;
// Global ID that we increment to be used
// as identifier for SUM reports
int groupID = 0;
// Mutex to protect access to the above ID
Mutex groupCond;
// Condition used to signify advances of the current
// records being accessed in a report and also to
// serialize modification of the report list
Condition ReportCond;
Condition ReportDoneCond;

// global variables only accessed within this file

// Thread that received the SIGTERM or SIGINT signal
// Used to ensure that if multiple threads receive the
// signal we do not prematurely exit
nthread_t sThread;
// The main thread uses this function to wait
// for all other threads to complete
void waitUntilQuit( void );

/* -------------------------------------------------------------------
 * main()
 *      Entry point into Iperf
 *
 * sets up signal handlers
 * initialize global locks and conditions
 * parses settings from environment and command line
 * starts up server or client thread
 * waits for all threads to complete
 * ------------------------------------------------------------------- */
int main( int argc, char **argv ) {

    // Set SIGTERM and SIGINT to call our user interrupt function
    my_signal( SIGTERM, Sig_Interupt );
    my_signal( SIGINT,  Sig_Interupt );
#ifndef WIN32 // SIGALRM=14, _NSIG=3...
    my_signal( SIGALRM,  Sig_Interupt );

    // Ignore broken pipes
    signal(SIGPIPE,SIG_IGN);
#else
    // Start winsock
    WSADATA wsaData;
    int rc = WSAStartup( 0x202, &wsaData );
    WARN_errno( rc == SOCKET_ERROR, "WSAStartup" );
    if (rc == SOCKET_ERROR)
        return 0;

    // Tell windows we want to handle our own signals
    SetConsoleCtrlHandler( sig_dispatcher, true );
#endif

    // Initialize global mutexes and conditions
    Condition_Initialize ( &ReportCond );
    Condition_Initialize ( &ReportDoneCond );
    Mutex_Initialize( &groupCond );
    Mutex_Initialize( &clients_mutex );

    // Initialize the thread subsystem
    thread_init( );

    // Initialize the interrupt handling thread to 0
    sThread = thread_zeroid();

    // perform any cleanup when quitting Iperf
    atexit( cleanup );

    // Allocate the "global" settings
    thread_Settings* ext_gSettings = new thread_Settings;

    // Initialize settings to defaults
    Settings_Initialize( ext_gSettings );
    // read settings from command-line parameters
    Settings_ParseCommandLine( argc, argv, ext_gSettings );

    // Check for either having specified client or server
    if ( ext_gSettings->mThreadMode == kMode_Client
         || ext_gSettings->mThreadMode == kMode_Listener ) {
#ifdef WIN32
        // Start the server as a daemon
        // Daemon mode for non-windows in handled
        // in the listener_spawn function
        if ( isDaemon( ext_gSettings ) ) {
            CmdInstallService(argc, argv);
            return 0;
        }

        // Remove the Windows service if requested
        if ( isRemoveService( ext_gSettings ) ) {
            // remove the service
            if ( CmdRemoveService() ) {
                fprintf(stderr, "IPerf Service is removed.\n");

                return 0;
            }
        }
#endif
        // initialize client(s)
        if ( ext_gSettings->mThreadMode == kMode_Client ) {
            client_init( ext_gSettings );
        }

#ifdef HAVE_THREAD
        // start up the reporter and client(s) or listener
        {
            thread_Settings *into = NULL;
            // Create the settings structure for the reporter thread
            Settings_Copy( ext_gSettings, &into );
            into->mThreadMode = kMode_Reporter;

            // Have the reporter launch the client or listener
            into->runNow = ext_gSettings;

            // Start all the threads that are ready to go
            thread_start( into );
        }
#else
        // No need to make a reporter thread because we don't have threads
        thread_start( ext_gSettings );
#endif
    } else {
        // neither server nor client mode was specified
        // print usage and exit

#ifdef WIN32
        // In Win32 we also attempt to start a previously defined service
        // Starting in 2.0 to restart a previously defined service
        // you must call iperf with "iperf -D" or using the environment variable
        SERVICE_TABLE_ENTRY dispatchTable[] =
        {
            { TEXT(SZSERVICENAME), (LPSERVICE_MAIN_FUNCTION)service_main},
            { NULL, NULL}
        };

        // Only attempt to start the service if "-D" was specified
        if ( !isDaemon(ext_gSettings) ||
             // starting the service by SCM, there is no arguments will be passed in.
             // the arguments will pass into Service_Main entry.
             !StartServiceCtrlDispatcher(dispatchTable) )
            // If the service failed to start then print usage
#endif
        fprintf( stderr, usage_short, argv[0], argv[0] );

        return 0;
    }

    // wait for other (client, server) threads to complete
    thread_joinall();

    // all done!
    return 0;
} // end main

/* -------------------------------------------------------------------
 * Signal handler sets the sInterupted flag, so the object can
 * respond appropriately.. [static]
 * ------------------------------------------------------------------- */

void Sig_Interupt( int inSigno ) {
#ifdef HAVE_THREAD
    // We try to not allow a single interrupt handled by multiple threads
    // to completely kill the app so we save off the first thread ID
    // then that is the only thread that can supply the next interrupt
    if ( thread_equalid( sThread, thread_zeroid() ) ) {
        sThread = thread_getid();
    } else if ( thread_equalid( sThread, thread_getid() ) ) {
        sig_exit( inSigno );
    }

    // global variable used by threads to see if they were interrupted
    sInterupted = 1;

    // with threads, stop waiting for non-terminating threads
    // (ie Listener Thread)
    thread_release_nonterm( 1 );

#else
    // without threads, just exit quietly, same as sig_exit()
    sig_exit( inSigno );
#endif
}

/* -------------------------------------------------------------------
 * Any necesary cleanup before Iperf quits. Called at program exit,
 * either by exit() or terminating main().
 * ------------------------------------------------------------------- */

void cleanup( void ) {
#ifdef WIN32
    // Shutdown Winsock
    WSACleanup();
#endif
    // clean up the list of clients
    Iperf_destroy ( &clients );

    // shutdown the thread subsystem
    thread_destroy( );
} // end cleanup






