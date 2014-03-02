/*---------------------------------------------------------------
 *   Incorporates class declarations for fetching data from files
 * ------------------------------------------------------------------- */

#ifndef PERFSOCKET_H
#define PERFSOCKET_H

#include "Mutex.h"
#include "Settings.h"

    void SetSocketOptions( thread_Settings *inSettings );

    // handle interupts
    void Sig_Interupt( int inSigno );

    extern int sInterupted;
    extern int groupID;
    extern Mutex groupCond;

#endif // PERFSOCKET_H
