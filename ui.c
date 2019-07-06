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
 * File: ui.c
 * Usage: User interface for standalone mode.
 */


#include "hw.h"
#include "global.h"


#define is_dec( x ) 	(( (x) >= '0' ) && ( (x) <= '9' ))
#define is_hex( x )	((( (x) >= '0' ) && ( (x) <= '9' )) || \
			(( (x) >= 'a' ) && ( (x) <= 'f' )))


/*
 * ui() - The user interface for the standalone mode.  Get a single
 * letter command name followed by possible options.  Pass it off to
 * the correct routine for processing.  Now it's split into ui()
 * and ui_parse() to allow multiple commands per line and command
 * repeat... maybe even looping!
 */

ui()
{
	register char *s, *r, *e;
	char c;
	int x;

	lasttime = seconds;

	group_table[2].loop = 0;	/* kill existing loop */
	for ( x = 0; x < NUMVAR; ++x )	/* clear the variables */
		group_table[2].var[x] = 0;
	group_table[2].speed = 1;	/* set the speed */
	group_table[2].flags = 0;	/* set the flags */
	group_table[2].dim = 15;	/* set the dimming level */
	group_table[2].loop = 4;	/* start the loop... */

	for ( ;; ) {
		puts( "-> " );
		r = ibuffer;
		s = ibuffer;
		e = ibuffer;
		gets( s );
		if ( *s == '!' ) {	/* ! at beginning, repeat last cmd */
			strcpy( ibuffer, obuffer );
		} else {		/* save for later repeat */
			strcpy( obuffer, ibuffer );
		}
		while ( *e != '\0' )	/* e points to end of cmd line */
			++e;
		while ( r != e ) {
			++r;
			while (( *r != '\0' ) && ( *r != ';' )) {
				if ( *r == '!' ) {
					if ( rb_count ) {
						c = getchar();
#ifdef LINT
						c = c;
#endif
						++r;
						s = r;
					} else {
						r = ibuffer;	
						s = ibuffer;
						strcpy( ibuffer, obuffer );
					}
				} else {
					++r;
				}
			}
			*r = '\0';
			ui_parse( s );
			s = r + 1;
		}
	}
}

ui_parse( s )
register char *s;
{
	while (( *s == ' ' ) || ( *s == '\t' ))
		++s;
	switch( *s++ ) {
	case 'r':		/* run a loop */
		ui_run( s );
		break;
	case 's':		/* show what is running */
		ui_show();
		break;
	case 'k':		/* kill a loop */
		ui_kill( s );
		break;
	case 'l':		/* list possible loops */
		ui_list();
		break;
	case 'c':		/* configuration options */
		ui_conf( s );
		break;
	case 'd':		/* dump dimming table */
		ui_dump();
		break;
	case 'v':		/* view variables */
		ui_view();
		break;
	case 'f':		/* force a light to a level */
		ui_force( s );
		break;
	case 'h':		/* print help screen */
		ui_help();
		break;
	case 'm':		/* simulate music level */
		ui_mlevel( s );
		break;
	case 'i':		/* load light inversion mask */
		ui_invmask( s );
		break;
	case 'b':		/* simulate music beat */
		mbeat++;
		break;
	case '@':		/* clear those lights */
		clear_dim();
		break;
#ifdef VMEBUG
	case '%':		/* escape to VMEbug */
		goto_bug();
		break;
#endif /* VMEBUG */
	case ':':		/* colon commands */
		ui_colon( s );
		break;
	case 'n':		/* take a nap */
		ui_nap( s );
		break;
	case '\0':		/* null command */
		break;
	default:		/* anything else is invalid */
		ura_knob();
		break;
	}
}



/*
 * ura_knob() - Some kind of input error.
 */

ura_knob()
{
	puts( "You knob.\r\n" );
}


/*
 * rd_long() - Read a long hex number in with some error checking.
 */

char *
rd_long( s, v )
register char *s;
unsigned long *v;
{
	register unsigned long u;
	register int flag;

	u = 0;
	flag = 0;
	while (( *s == '\t' ) || ( *s == ' ' ))
		++s;
	while ( is_hex( *s ) ) {
		++flag;
		if ( *s >= 'a' ) {
			u = ( u * 16 ) + (( *s - 'a' ) + 10 );
		} else {
			u = ( u * 16 ) + ( *s - '0' );
		}
		++s;
	}
	if ( flag ) {
		*v = u;
		return s;
	} else {
		*v = 0;
		return (char *) 0;
	}
}


/*
 * rd_dec() - Read a short decimal number in with some error checking.
 */

char *
rd_dec( s, v )
register char *s;
int *v;
{
	register int u;
	register int flag;

	u = 0;
	flag = 0;
	while (( *s == '\t' ) || ( *s == ' ' ))
		++s;
	while ( is_dec( *s ) ) {
		++flag;
		u = ( u * 10 ) + ( *s - '0' );
		++s;
	}
	if ( flag ) {
		*v = u;
		return s;
	} else {
		*v = 0;
		return (char *) 0;
	}
}


/*
 * ui_help() - Print the help screen.
 */

ui_help()
{
	printf( "simulate music beat    b\r\n" );
	printf( "configure group add    c a <group> [light] ... [light]\r\n" );
	printf( "configure group clear  c c <group>\r\n" );
	printf( "configure group        c g <group> [light0] ... [lightn]\r\n" );
	printf( "load configuration     c l <num>\r\n" );
	printf( "print configuration    c p\r\n" );
#ifdef NVRAM
	printf( "save configuration     c s <num>\r\n" );
#endif /* NVRAM */
	printf( "dump dimming table     d\r\n" );
	printf( "force light to level   f <light> <level>\r\n" );
	printf( "help                   h\r\n" );
	printf( "light inversion mask   i <mask>\r\n" );
	printf( "kill a loop            k <group>|<a>\r\n" );
	printf( "list the loops         l\r\n" );
	printf( "simulate music level   m <mlevel>\r\n" );
	printf( "take a nap             n <length>\r\n" );
	printf( "run a loop             r <group> <loop> <interval> [m] [i] [f <level>]\r\n" );
	printf( "show running loops     s\r\n" );
	printf( "view internal info     v\r\n" );
	printf( "clear all lights       @\r\n" );
	printf( "repeat or loop         !\r\n" );
#ifdef VMEBUG
	printf( "escape to VMEbug       %%\r\n" );
#endif /* VMEBUG */
	printf( "system commands        :<command>\r\n" );
}


/*
 * ui_colon() - System commands.
 */

ui_colon( s )
char *s;
{
	switch( *s ) {
	case 'r':
		start();
		break;
	default:
		ura_knob();
		break;
	}
}


/*
 * ui_dump() - Dump the current contents of the dimming table.
 */

ui_dump()
{
	int i;

	for ( i = 0; i < DIMSTEPS; ++i ) {
		printf( "%X\r\n", dim_table[i] );
	}
}


/*
 * ui_run() - Run a new loop in the given group.
 */

ui_run( s )
char *s;
{
	int g, l, p, d, x, f;

	d = MAXDIM - 1 ;	/* dimming level defaults to maximum */
	f = 0;			/* flags default to none */

	s = rd_dec( s, &g );		/* get group number */
	if ( s == (char *) 0 ) {
		ura_knob();
		return;
	}

	s = rd_dec( s, &l );		/* get loop number */
	if ( s == (char *) 0 ) {
		ura_knob();
		return;
	}

	s = rd_dec( s, &p );		/* get speed */
	if ( s == (char *) 0 ) {
		ura_knob();
		return;
	}

	do {
		while (( *s == ' ' ) || ( *s == '\t' ))
			++s;
		switch ( *s ) {
		case 'f':		/* fixed dimming level option */
			++s;
			s = rd_dec( s, &d );		/* get dim level */
			if ( s == (char *) 0 ) {
				ura_knob();
				return;
			}
			break;
		case 'm':		/* music option */
			++s;
			f |= LPFG_MUSIC;
			break;
		case 'i':		/* variable intensity option */
			++s;
			f |= LPFG_VAR;
			break;
		case '\0':		/* done with possible options */
			break;
		default:
			ura_knob();
			return;
		};
	} while ( *s != '\0' );

	if ( g >= NUMGROUPS ) {
		puts( "Group number out of range.\r\n" );
		return;
	}

	if (( l >= NUMLOOPS ) || ( loop_table[l].name == (char*) 0 )) {
		puts( "Loop number out of range.\r\n" );
		return;
	}

	if (( p < 0 ) || ( p > MAXINTV )) {
		puts( "Invalid speed.\r\n" );
		return;
	}

	if (( d < 0 ) || ( d >= MAXDIM )) {
		puts( "Fixed dimming level out of range.\r\n" );
		return;
	}

	group_table[g].loop = 0;	/* kill existing loop */
	for ( x = 0; x < NUMVAR; ++x )	/* clear the variables */
		group_table[g].var[x] = 0;
	group_table[g].speed = p;	/* set the speed */
	group_table[g].flags = f;	/* set the flags */
	group_table[g].dim = d;		/* set the dimming level */
	group_table[g].loop = l;	/* start the loop... */
}


/*
 * ui_show() - Show what each group is running.
 */

ui_show()
{
	register struct group_desc *p;
	register int i;

	p = group_table;
	for ( i = 0; i < NUMGROUPS; ++i ) {
		if ( p->loop ) {
			printf( "%d: %s %d %d", i, loop_table[ p->loop ].name,
				p->speed, p->dim );
			if ( p->flags & LPFG_VAR )
				printf( " i" );
			if ( p->flags & LPFG_MUSIC )
				printf( " m" );
			printf( "\r\n" );
		}
		++p;
	}
}


/*
 * ui_kill() - Kill a loop running in the given group.
 */

ui_kill( s )
char *s;
{
	int g, i;

	while (( *s == ' ' ) || ( *s == '\t' ))
		++s;

	if ( *s == 'a' ) {		/* kill all running loops */
		for ( i = 0; i < NUMGROUPS; ++i ) {
			group_table[i].loop = 0;
		}
		clear_dim();		/* mop up afterwards */
		return;
	}

	s = rd_dec( s, &g );		/* get the group number */
	if ( s == (char *) 0 ) {
		ura_knob();
		return;
	}

	if ( g >= NUMGROUPS ) {
		puts( "Group number out of range.\r\n" );
		return;
	}

	if ( group_table[g].loop ) {
		group_table[g].loop = 0;	/* kill it */
		for ( i = 0; i < group_table[g].mapsize; ++i ) {
			fill_dim( group_table[g].map[i], 0 );
		}
	}
}


/*
 * ui_list() - List the available loops to run.
 */

ui_list()
{
	int i;

	for ( i = 0; i < NUMLOOPS; ++i ) {
		if ( loop_table[i].name != (char *) 0 ) {
			printf( "%d: %s\r\n", i, loop_table[i].name );
		}
	}
}


/*
 * ui_mlevel() - Simulate a music level.
 */

ui_mlevel( s )
char *s;
{
	int l;

	s = rd_dec( s, &l );
	if ( s == (char *) 0 ) {
		ura_knob();
		return;
	}

	if ( l >= MAXMUSIC ) {
		puts( "Music level out of range.\r\n" );
		return;
	}

	mlevel = l;
}


/*
 * ui_force() - Force a light to a given dimming level
 */

ui_force( s )
char *s;
{
	int l, d;

	s = rd_dec( s, &l );		/* get light number */
	if ( s == (char *) 0 ) {
		ura_knob();
		return;
	}

	s = rd_dec( s, &d );		/* read in dimming level */
	if ( s == (char *) 0 ) {
		ura_knob();
		return;
	}

	if ( l >= NUMLIGHTS ) {
		puts( "Light number out of range.\r\n" );
		return;
	}

	if (( d < 0 ) || ( d >= MAXDIM )) {
		puts( "Dimming level out of range.\r\n" );
		return;
	}

	fill_dim( (unsigned char) l, d );
}


#ifdef NVRAM
/*
 * calc_xor() - Calculate an xor checksum over a given dataset.
 */

unsigned char
calc_xor( p, n )
unsigned char *p;
int n;
{
	unsigned char c;

	c = 0;
	while ( n-- ) {
		c ^= *p++;
	}
	return c;
}
#endif /* NVRAM */


/*
 * ui_conf() - Configuration options.
 */

ui_conf( s )
char *s;
{
	while (( *s == ' ' ) || ( *s == '\t' ))
		++s;
	switch( *s++ ) {
	case 'g':
		ui_cgroup( s, 1 );
		break;
	case 'p':
		ui_cprint();
		break;
	case 'c':
		ui_cclear( s );
		break;
	case 'a':
		ui_cgroup( s, 0 );
		break;
	case 'l':
		ui_cload( s );
		break;
#ifdef NVRAM
	case 's':
		ui_csave( s );
		break;
#endif
	default:
		ura_knob();
		return;
		break;
	};
}


/*
 * ui_cload() - Load a configuration.
 */

ui_cload( s )
char *s;
{
	int c;

	s = rd_dec( s, &c );		/* get configuration number */
	if ( s == (char *) 0 ) {
		ura_knob();
		return;
	}

	if ( c >= NUMCONF ) {
		puts( "Configuration number out of range.\r\n" );
		return;
	}

	load_conf( c );
}


/*
 * load_conf() - Load the actual configuration information.
 */

/*ARGSUSED*/
load_conf( c )
int c;
{
	int g, l;
	struct conf_ent *p;

#ifdef NVRAM
	p = (struct conf_ent *) ( NVRAM + ( c * sizeof( struct conf_ent )));

	if ( calc_xor((unsigned char *) p, sizeof( struct conf_ent )) ) {
		puts( "Configuration checksum is invalid.\r\n" );
		return;
	}
#else
	p = &rom_conf;
#endif /* NVRAM */

	for ( g = 0; g < NUMGROUPS; ++g ) {
		group_table[g].mapsize = p->group[g].mapsize;
		for ( l = 0; l < NUMLIGHTS; ++l ) {
			group_table[g].map[l] = p->group[g].map[l];
		}
	}
}

#ifdef NVRAM
/*
 * ui_csave() - Save a configuration.
 */

ui_csave( s )
char *s;
{
	int c, g, l;
	struct conf_ent *p;

	s = rd_dec( s, &c );		/* get configuration number */
	if ( s == (char *) 0 ) {
		ura_knob();
		return;
	}

	if ( c >= NUMCONF ) {
		puts( "Configuration number out of range.\r\n" );
		return;
	}

	p = (struct conf_ent *) ( NVRAM + ( c * sizeof( struct conf_ent )));


	for ( g = 0; g < NUMGROUPS; ++g ) {
		p->group[g].mapsize = group_table[g].mapsize;
		for ( l = 0; l < NUMLIGHTS; ++l ) {
			p->group[g].map[l] = group_table[g].map[l];
		}
	}

	p->checksum = 0;
	p->checksum = calc_xor((unsigned char *) p, sizeof( struct conf_ent ));

	if ( calc_xor((unsigned char *) p, sizeof( struct conf_ent )) ) {
		puts( "Configuration checksum is invalid.\r\n" );
		return;
	}
}
#endif /* NVRAM */


/*
 * ui_cgroup() - Configure a group.
 */

ui_cgroup( s, c )
char *s;
int c;
{
	int g, i;

	s = rd_dec( s, &g );		/* get group number */
	if ( s == (char *) 0 ) {
		ura_knob();
		return;
	}

	if ( g >= NUMGROUPS ) {
		puts( "Group number out of range.\r\n" );
		return;
	}

	if ( c )				/* clear table if required */
		group_table[g].mapsize = 0;

	do {
		s = rd_dec( s, &i );		/* get next light number */
		if ( s != (char *) 0 ) {
			if ( i >= NUMLIGHTS ) {
				puts( "Light number out of range.\r\n" );
				return;
			}
			if ( group_table[g].mapsize == NUMLIGHTS ) {
				group_table[g].mapsize = 0;
				puts( "Too many lights in group.\r\n" );
				return;
			}
			group_table[g].map[ group_table[g].mapsize++ ] = i;
		}
	} while ( s != (char *) 0 );
}


/*
 * ui_cclear() - Clear a group's configuration.
 */

ui_cclear( s )
char *s;
{
	int g;

	s = rd_dec( s, &g );		/* get group number */
	if ( s == (char *) 0 ) {
		ura_knob();
		return;
	}

	if ( g >= NUMGROUPS ) {
		puts( "Group number out of range.\r\n" );
		return;
	}

	group_table[g].mapsize = 0;
}


/*
 * ui_cprint() - Show the current configuration.
 */

ui_cprint()
{
	int i, j;

	for  ( i = 0; i < NUMGROUPS; ++i ) {
		if ( group_table[i].mapsize == 0 )	/* skip if empty */
			continue;
		printf( "%d: ", i );
		for ( j = 0; (( j < NUMLIGHTS ) &&
			( j < group_table[i].mapsize )); ++j ) {
				printf( "%d ", group_table[i].map[j] );
			}
		printf( "\r\n" );
	}
}


/*
 * ui_view() - Show some internal variables.
 */

ui_view()
{
	printf( "ticks: %D\r\n", ticks );
	printf( "seconds: %D\r\n", seconds );
	printf( "full counts: %D\r\n", full_count );
	printf( "guard hits: %D\r\n", guard_hits );
	printf( "last step: %d\r\n", last_step );
	printf( "music level: %d\r\n", mlevel );
}


/*
 * ui_nap() - Sleep for a while.
 */

ui_nap( s )
char *s;
{
	int t;
	unsigned long future;

	s = rd_dec( s, &t );		/* get length of time */
	if ( s == (char *) 0 ) {
		ura_knob();
		return;
	}

	future = ticks + (unsigned long) t;
	while ( ticks <= future ) {
		idle();
	}
}


/*
 * ui_invmask() - Read light inversion mask.
 */

ui_invmask( s )
char *s;
{
	unsigned long m;

	s = rd_long( s, &m );		/* get the mask */
	if ( s == (char *) 0 ) {
		ura_knob();
		return;
	}

	flip_mask = m;
}
