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
 * File: loop.c
 * Usage: Loop executive and generic loop code.
 */


#include "hw.h"
#include "global.h"


/*
 * run_loops() - Look through the table of groups for loops to run.
 */

run_loops()
{
	int mflag;
	register int i;
	register struct group_desc *p;

	mflag = 0;
	if ( mbeat ) {
		--mbeat;
		mflag = 1;
	}

	p = group_table;
	for ( i = 0; i < NUMGROUPS; ++i ) {
		if ( p->loop ) {
			if (( p->flags & LPFG_MUSIC ) && mflag )
				p->var[0] = ( MAXINTV + 1 );
			if ( p->flags & LPFG_VAR )
				p->dim = mlevel / ( MAXMUSIC / MAXDIM );
			switch (( loop_table[ p->loop ].func )( p )) {
			case 0:
			default:
				break;
			case 1:
				return;
				break;
			case -1:
				p->loop = 0;
				break;
			}
		}
		++p;
	}
}


/*
 * How to add new loops:
 *
 *	1.  Add the generic routine here. The only argument is a pointer
 *	to the group context information.
 *	2.  Add the routine name and description to the static loop
 *	table in global.h
 *	3. Var[0] should be the change interval. Ui_run() depends
 *	on this, and so might some other things.
 *	4. Return 0 normally, -1 if you want the loop to be terminated,
 *	or 1 if you wish to override lower priority (higher numbered)
 *	groups.
 */


/*
 * lp_inc() - Basic incrementing pattern.
 */

lp_inc( p )
register struct group_desc *p;
{
	if ( p->mapsize == 0 )
		return -1;
	if ( ++ ( p->var[0] ) > p->speed ) {
		p->var[0] = 0;
		fill_dim( p->map[ p->var[1] ], 0 );
		if ( ++ ( p->var[1] ) >= p->mapsize ) {
			p->var[1] = 0;
		}
		fill_dim( p->map[ p->var[1] ], p->dim );
	}
	return 0;
}


/*
 * lp_flash() - Clears _all_ lights on the first pass and turns on the
 * given group to the given intensity.  A countdown timer is started and
 * after ticks = speed, the loop kills itself.  If speed is zero, the
 * loop must be killed manually.  Good for flash-and-continues and
 * strobe lights.
 */

lp_flash( p )
register struct group_desc *p;
{
	register int j;

	if ( p->var[1] == 0 ) {
		p->var[1] = 1;
		clear_dim();
		for ( j = 0; j < p->mapsize; ++j ) {
			fill_dim( p->map[ j ], p->dim );
		}
	} else {
		if ( p->speed ) {
			if ( ++ ( p->var[0] ) > p->speed ) {
				for ( j = 0; j < p->mapsize; ++j ) {
					fill_dim( p->map[ j ], 0 );
				}
				return -1;
			}
		}
	}
	return 1;	/* override other groups */
}


/*
 * lp_throb() - Basic increasing intensity pattern.
 */

lp_throb( p )
register struct group_desc *p;
{
	register int j;

	if ( ++ ( p->var[0] ) > p->speed ) {
		p->var[0] = 0;
		for ( j = 0; j < p->mapsize; ++j ) {
			fill_dim( p->map[ j ], p->var[1] );
		}
		if ( ++ ( p->var[1] ) > p->dim ) {
			p->var[1] = 0;
		}
	}
	return 0;
}


/*
 * lp_clear() - Clears _all_ lights on the first pass.
 * A countdown timer is started and after ticks = speed,
 * the loop kills itself.  If speed is zero, the loop
 * must be killed manually.
 */

lp_clear( p )
register struct group_desc *p;
{
	if ( p->var[1] == 0 ) {
		p->var[1] = 1;
		clear_dim();
	} else {
		if ( p->speed ) {
			if ( ++ ( p->var[0] ) > p->speed ) {
				return -1;
			}
		}
	}
	return 1;	/* override other groups */
}


/*
 * lp_cylon() - Basic cylon pattern.
 */

lp_cylon( p )
register struct group_desc *p;
{
	if ( p->mapsize < 3 )
		return -1;
	if ( ++ ( p->var[0] ) > p->speed ) {
		p->var[0] = 0;
		fill_dim( p->map[ p->var[1] ], 0 );
		if ( p->var[2] == 0 ) {
			if ( ++ ( p->var[1] ) >= ( p->mapsize - 1 )) {
				p->var[2] = 1;
			}
			fill_dim( p->map[ p->var[1] ], p->dim );
		} else {
			if ( -- ( p->var[1] ) == 0 ) {
				p->var[2] = 0;
			}
			fill_dim( p->map[ p->var[1] ], p->dim );
		}
	}
	return 0;
}


/*
 * lp_dcylon() - Double cylon pattern.
 */

lp_dcylon( p )
register struct group_desc *p;
{
	if ( p->mapsize < 3 )
		return -1;
	if ( ++ ( p->var[0] ) > p->speed ) {
		p->var[0] = 0;
		fill_dim( p->map[ p->var[1] ], 0 );
		fill_dim( p->map[ ( p->mapsize -1 ) - p->var[1] ], 0 );
		if ( p->var[2] == 0 ) {
			if ( ++ ( p->var[1] ) >= ( p->mapsize - 1 )) {
				p->var[2] = 1;
			}
		} else {
			if ( -- ( p->var[1] ) == 0 ) {
				p->var[2] = 0;
			}
		}
		fill_dim( p->map[ p->var[1] ], p->dim );
		fill_dim( p->map[ ( p->mapsize -1 ) - p->var[1] ], p->dim );
	}
	return 0;
}


/*
 * lp_marquee() - Like the lights on a theatre marquee.  Goes in
 * groups of four by default, but can be patched by changing var[2].
 */

lp_marquee( p )
register struct group_desc *p;
{
	register int j;

	if ( p->var[2] == 0 )
		p->var[2] = 4;
	if ( p->mapsize < p->var[2] )
		return -1;
	if ( ++ ( p->var[0] ) > p->speed ) {
		p->var[0] = 0;
		for( j = p->var[1]; j < p->mapsize; j += p->var[2] )
			fill_dim( p->map[j], 0 );
		if ( ++ ( p->var[1] ) >=  p->var[2] )
			p->var[1] = 0;
		for( j = p->var[1]; j < p->mapsize; j += p->var[2] )
			fill_dim( p->map[j], p->dim );
	}
	return 0;
}


/*
 * lp_blink() - Blink a group of lights at the given rate.
 */

lp_blink( p )
register struct group_desc *p;
{
	register int j;

	if ( ++ ( p->var[0] ) > p->speed ) {
		p->var[0] = 0;
		if ( p->var[1] ) {
			for ( j = 0; j < p->mapsize; ++j )
				fill_dim( p->map[j], 0 );
			p->var[1] = 0;
		} else {
			for ( j = 0; j < p->mapsize; ++j )
				fill_dim( p->map[j], p->dim );
			p->var[1] = 1;
		}
	}
	return 0;
}


/*
 * lp_random() - Blink a group of lights in a random manner.
 */

lp_random( p )
register struct group_desc *p;
{
	if ( p->mapsize < 1 )
		return -1;
	if ( ! p->var[1] ) {
		p->var[1] = 1;
		srand( ticks );
	}
	if ( ++ ( p->var[0] ) > p->speed ) {
		p->var[0] = 0;
		fill_dim( p->map[ p->var[ 2 ]], 0 );
		p->var[2] = rand() % p->mapsize;
		fill_dim( p->map[ p->var[ 2 ]], p->dim );
	}
	return 0;
}


/*
 * lp_cycle() - Cycle through intensitys.
 */

lp_cycle( p )
register struct group_desc *p;
{
	register int j;

	if ( ++ ( p->var[0] ) > p->speed ) {
		p->var[0] = 0;
		if ( p->var[2] == 0 ) {
			for ( j = 0; j < p->mapsize; ++j ) {
				fill_dim( p->map[ j ], p->var[1] );
			}
			if ( ++ ( p->var[1] ) >= p->dim ) {
				p->var[2] = 1;
			}
		} else {
			for ( j = 0; j < p->mapsize; ++j ) {
				fill_dim( p->map[ j ], p->var[1] );
			}
			if ( -- ( p->var[1] ) == 0 ) {
				p->var[2] = 0;
			}
		}
	}
	return 0;
}


/*
 * lp_build() - Basic build pattern.
 */

lp_build( p )
register struct group_desc *p;
{
	register int j;

	if ( p->mapsize < 2 )
		return -1;
	if ( ++ ( p->var[0] ) > p->speed ) {
		p->var[0] = 0;
		if ( p->var[2] == 0 ) {
			fill_dim( p->map[ p->var[1] ], p->dim );
			if ( ++ ( p->var[1] ) >= ( p->mapsize )) {
				p->var[2] = 1;
			}
		} else {
			for ( j = 0; j < p->mapsize; ++j )
				fill_dim( p->map[j], 0 );
			p->var[2] = 0;
			p->var[1] = 0;
		}
	}
	return 0;
}


/*
 * lp_build() - Build and break down pattern.
 */

lp_bbuild( p )
register struct group_desc *p;
{
	if ( p->mapsize < 2 )
		return -1;
	if ( ++ ( p->var[0] ) > p->speed ) {
		p->var[0] = 0;
		if ( p->var[2] == 0 ) {
			fill_dim( p->map[ p->var[1] ], p->dim );
			if ( ++ ( p->var[1] ) >= ( p->mapsize )) {
				p->var[2] = 1;
				p->var[1] = 0;
			}
		} else {
			fill_dim( p->map[ p->var[1] ], 0 );
			if ( ++ ( p->var[1] ) >= ( p->mapsize )) {
				p->var[2] = 0;
				p->var[1] = 0;
			}
		}
	}
	return 0;
}


/*
 * lp_alter() - Alternating pattern.
 */

lp_alter( p )
register struct group_desc *p;
{
	if ( p->mapsize < 2 )
		return -1;
	if ( ++ ( p->var[0] ) > p->speed ) {
		p->var[0] = 0;
		if ( p->var[1] & 1 ) {
			fill_dim( p->map[(p->mapsize - 1) - (p->var[1] / 2)],
				0 );
		} else {
			fill_dim( p->map[p->var[1] / 2], 0 );
		}
		if ( ++ ( p->var[1] ) >= p->mapsize ) {
			p->var[1] = 0;
		}
		if ( p->var[1] & 1 ) {
			fill_dim( p->map[(p->mapsize - 1) - (p->var[1] / 2)],
				p->dim );
		} else {
			fill_dim( p->map[p->var[1] / 2], p->dim );
		}
	}
	return 0;
}


/*
 * lp_fade() - Simulate a music fade out.
 */

lp_fade( p )
register struct group_desc *p;
{
	if ( ++ ( p->var[0] ) > p->speed ) {
		p->var[0] = 0;
		if ( mlevel ) {
			--mlevel;
		} else {
			return -1;
		}
	}
	return 0;
}
