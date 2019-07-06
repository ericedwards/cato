/*
 * Copyright (c) 1991 Eric A. Edwards
 *
 * This file is part of Cato, an implemenation of D.A.M.M.I.T.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * File: main.c
 * Usage:
 */


#include "hw.h"
#include "global.h"


/*
 * main() - Startup and calling of the main loop.
 */

main()
{
	gpip = 0x0f;
	clear_dim();
	printf( "\r\n\r\nStarting...\r\n" );
	printf( "Loading default configuration... " );
	load_conf( 0 );
	printf( "done.\r\n" );
	ui();
}


/*
 * clock_proc() - Called about once per second.  Things that don't need
 * to be done too often can be called form here ( sanity checks, pretty
 * blinking lights, etc. ).
 */

clock_proc()
{
	register struct mc68901 *mfp = (struct mc68901 *) MFP;

	/*
	 * Blink led at ~1/2 Hz.
	 */

	if ( gpip & GPIP_LED ) {
		gpip &= ~GPIP_LED;
	} else {
		gpip |= GPIP_LED;
	}
	mfp->gpip = gpip;


	/*
	 * Sanity check #1 - See if the 960Hz clock interrupt has
	 * ever missed making 15 interrupts within the 60Hz frame.
	 *
	 * A minimum number of 60 cycles missed has been added,
	 * it seems a few are missed just after startup... they
	 * may have been due to pending 60Hz interrupts.  If we
	 * print the message, reset full_count to ticks.
	 */

	if (( ticks - full_count )  > 60 ) {
		puts( "sanity: 960Hz clock ticks missing.\r\n" );
		full_count = ticks;
	}

}
