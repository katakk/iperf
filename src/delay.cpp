/*---------------------------------------------------------------
 * Copyright (c) 1999,2000,2001,2002,2003,2010
 * The Board of Trustees of the University of Illinois
 * All Rights Reserved.
 *---------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software (Iperf) and associated
 * documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 *
 * Redistributions of source code must retain the above
 * copyright notice, this list of conditions and
 * the following disclaimers.
 *
 *
 * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimers in the documentation and/or other materials
 * provided with the distribution.
 *
 *
 * Neither the names of the University of Illinois, NCSA,
 * nor the names of its contributors may be used to endorse
 * or promote products derived from this Software without
 * specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTIBUTORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ________________________________________________________________
 * National Laboratory for Applied Network Research
 * National Center for Supercomputing Applications
 * University of Illinois at Urbana-Champaign
 * http://www.ncsa.uiuc.edu
 * ________________________________________________________________
 *
 * delay.c
 * by Kirby Zhou <kirbyzhou at gmail.com>
 * -------------------------------------------------------------------
 * accurate microsecond delay
 * ------------------------------------------------------------------- */

#include "Timestamp.hpp"

#include "delay.hpp"

#ifndef WIN32
/* -------------------------------------------------------------------
 * determine the timer resolution of nanosleep
 * ------------------------------------------------------------------- */
static long measure_nanosleep_threshold()
{
	timespec req = {0, 1000};
	Timestamp bg, ed;
	bg.setnow();
	for (int i = 0; i < 20; ++i) {
		nanosleep(&req, NULL);
	}
	ed.setnow();
	long diff = ed.subUsec(bg) / 20;
	if (diff <= 20) {
		diff = 10;
	} else if (diff <= 200 ) {
		diff = 10 * ( (diff+5) / 10 ) - 5;
	} else if (diff <= 600 ) {
		diff = 50 * ( (diff+25) / 50 ) - 50;
	} else if (diff <= 2000 ) {
		diff = 100 * ( (diff+50) / 100 ) - 50;
	} else {
		diff = 500 * ( (diff+250) / 500 ) - 50;
	}
	//printf("thr %ld\n", diff);
	return diff;
}
static long nanosleep_threshold = measure_nanosleep_threshold();

/* -------------------------------------------------------------------
 * A micro-second delay function. This uses gettimeofday (underneith
 * the Timestamp) which has a resolution of upto microseconds. I've
 * found it's good to within about 10 usecs.
 * I used to do calibration, but iperf automatically adjusts itself
 * so that isn't necesary, and it causes some problems if the
 * calibration adjustment is larger than your sleep time.
 * ------------------------------------------------------------------- */

void delay_loop( unsigned long usec ) {
    Timestamp end;
    end.add( usec * 1e-6 );

    Timestamp now;
    long diff;
    while ( (diff = end.subUsec(now)) > 0 ) {
        if (diff >= nanosleep_threshold) {
/* convert to seconds; nanosleep requires 0 <= tv_nsec <= 999999999 */
            timespec req = { usec / 1000000UL,
                            (usec % 1000000UL) * 1000UL };
            nanosleep(&req, NULL);
        } else {
            sched_yield();
        }
        now.setnow();
    }
}
#else
void delay_loop( unsigned long usec ) {

	LARGE_INTEGER freq, start, now;

	if (!QueryPerformanceFrequency(&freq))
	{
		printf("qpfreq err\n");//xxx
	}
	QueryPerformanceCounter(&start);
	for(;;) {
		QueryPerformanceCounter((LARGE_INTEGER*) &now);
		if( ((double)(now.QuadPart - start.QuadPart) / (double)freq.QuadPart)  * 1000000 > usec ) break;
	}
}
#endif
