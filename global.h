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
 * File: global.h
 * Usage:
 */


#ifdef DEFINE_GLOBALS
#include "hw.h"
#define GLOBAL	 
#else
#define GLOBAL extern
#endif


/*
 * Defines for Configuration.
 */

#define HERTZ		60	/* number of clock ticks per second */
#define	BUFSIZ		80	/* serial input buffer size */
#define DIMSTEPS	24	/* dimming steps per 60Hz wave */
#define NUMLIGHTS	32	/* number of ligths we can handle */
#define LPTICKS		3	/* number of ticks between loop runs */
#define NUMLOOPS	32	/* number of generic loops */
#define NUMVAR		4	/* number of temporary variables per group */
#define NUMGROUPS	16	/* number of light groups */
#define MAXDIM		16	/* maximum dimming level */
#define MAXMUSIC	64	/* number of music intensity levels */
#define MAXINTV		20000	/* maximim interval for loop runs */
#define NUMCONF		1	/* number of stored configurations */
#define CONFNAME	16	/* maximum length of configuration name */


/*
 * Loop control flags.
 */

#define LPFG_MUSIC	0x0001
#define LPFG_VAR	0x0002


/*
 * Music response information.
 */

#define MR_BEAT		63
#define MR_IBASE	64


/*
 * Global Variables.
 */

GLOBAL char rb_buffer[BUFSIZ];		/* character buffer */
GLOBAL int rb_count;			/* number of characters in buffer */
GLOBAL char *rb_first;			/* pointer to head */
GLOBAL char *rb_last;			/* pointer to tail */

GLOBAL unsigned long ticks;		/* number of 60Hz ticks since reset */
GLOBAL unsigned long lasttime;		/* last time clock_proc() was done */
GLOBAL unsigned long seconds;		/* number of seconds since startup */
GLOBAL unsigned short stick;		/* 60Hz -> 1 second counter */
GLOBAL unsigned short ttick;		/* 60Hz -> 1/10 second counter */

GLOBAL int gpip;			/* state of the blinking LED */

GLOBAL int guard;			/* flag to protect interrupt service */

GLOBAL unsigned long full_count;	/* full counts by 960Hz clock */
GLOBAL unsigned long guard_hits;	/* # of times guard protected us */
GLOBAL short last_step;			/* huh? */

GLOBAL unsigned short mlevel;		/* current music level */
GLOBAL unsigned short mbeat;		/* number of beats queued */

GLOBAL short dim_step;				/* current step */
GLOBAL unsigned long dim_table[DIMSTEPS];	/* the table */
GLOBAL unsigned long flip_mask;			/* some lights need inverting */

GLOBAL char ibuffer[ BUFSIZ * 2 ];		/* a _big_ input buffer */
GLOBAL char obuffer[ BUFSIZ * 2 ];		/* for saving cmd lines */


/*
 * Generic loop table stuff.
 */

struct loop_desc {
	char *name;
	int (*func)();
};

#ifdef DEFINE_GLOBALS

	extern int lp_inc();
	extern int lp_throb();
	extern int lp_clear();
	extern int lp_flash();
	extern int lp_cylon();
	extern int lp_dcylon();
	extern int lp_marquee();
	extern int lp_blink();
	extern int lp_random();
	extern int lp_cycle();
	extern int lp_build();
	extern int lp_bbuild();
	extern int lp_alter();
	extern int lp_fade();

	struct loop_desc loop_table[NUMLOOPS] = {
		{ (char *) 0, /* (func *) */ 0 },
		{ "move incrementing", lp_inc },
		{ "build", lp_build },
		{ "build and break down", lp_bbuild },
		{ "cylon", lp_cylon },
		{ "double cylon", lp_dcylon },
		{ "increasing intensity", lp_throb },
		{ "cycle intensity", lp_cycle },
		{ "marquee", lp_marquee },
		{ "blink", lp_blink },
		{ "random", lp_random },
		{ "alternate", lp_alter },
		{ "simulate fade out", lp_fade },
		{ "clear and continue (s)", lp_clear },
		{ "flash and continue (s)", lp_flash },
		{ (char *) 0, /* (func *) */ 0 }
	};

#else

	GLOBAL struct loop_desc loop_table[NUMLOOPS];

#endif

GLOBAL struct group_desc {
	int loop;
	int flags;
	int speed;
	int dim;
	int var[NUMVAR];
	int mapsize;
	unsigned char map[NUMLIGHTS];
} group_table[NUMGROUPS];


/*
 * Configuration table info
 */

struct conf_ent {
	char name[CONFNAME];
	struct conf_grp {
		int mapsize;
		unsigned char map[NUMLIGHTS];
	} group[NUMGROUPS];
	unsigned char pad;
	unsigned char checksum;
};


/*
 * Rom configuration table (for systems without NVRAM)
 */

#ifdef DEFINE_GLOBALS

	struct conf_ent rom_conf = {
		{ 'd', 'e', 'f', 'a', 'u', 'l', 't', 0 },
		{
			{ 32, {
				0, 1, 2, 3, 4, 5, 6, 7,
				8, 9, 10, 11, 12, 13, 14, 15,
				16, 17, 18, 19, 20, 21, 22, 23,
				24, 25, 26, 27, 28, 29, 30, 31
				}},
			{ 1, { 18 }},
			{ 8, { 0, 1, 2, 3, 4, 5, 6, 7 }},
			{ 6, { 12, 13, 14, 15, 16, 17 }}
		},
		0, 0
	};

#else

	GLOBAL struct conf_ent rom_conf;

#endif
