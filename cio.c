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
 * File: cio.c
 * Usage:
 */


/*
 * puts() - put a string to the console.
 */

puts(s)
register char	*s;
{
	while (*s)
		putchar(*s++);
}


/*
 * printf() - a very much shortened version.
 */


/*VARARGS1*/
printf(fmt,x)
char	*fmt;
int	x;
{
	char	*p;
	register char	*r;

	p = (char *) &x;
	while (*fmt) {
		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
			case '%':
				putchar('%');
				break;
			case 'd':
				printn(*((unsigned int *)p),10);
				p += sizeof(unsigned int);
				break;
			case 'x':
				printn(*((unsigned int *)p),16);
				p += sizeof(unsigned int);
				break;
			case 'D':
				printl(*((unsigned long *)p),10);
				p += sizeof(unsigned long);
				break;
			case 'X':
				printl(*((unsigned long *)p),16);
				p += sizeof(unsigned long);
				break;
			case 'c':
				putchar( (char) *((int *)p) );
				p += sizeof(int);
				break;
			case 's':
				r = (char*) *(( unsigned long *) p );
				p += sizeof(char *);
				while (*r)
					putchar(*r++);
				break;
			default:
				putchar(*fmt);
				break;
			}
		} else
			putchar(*fmt);
		fmt++;
	}
}


/*
 * printn() - print a number in the given base.
 */

printn(n,b)
register unsigned int	n;
register unsigned int	b;
{
	if (n/b != 0)
		printn(n/b,b);
	putchar("0123456789abcdef"[n%b]);
}


/*
 * printl() - print a number in the given base.
 */

printl( n, b )
register unsigned long n;
register unsigned int b;
{
	if (( n / b ) > 0 )
		printl( n / b, b );
	putchar( "0123456789abcdef" [ n % b ] );
}


/*
 * gets() - get a string
 */


gets( s )
register char *s;
{
	register char *r;
	register char c;

	r = s;
	do {
		c = getchar();
		if ( c == '\n' )
			c = '\r';
		switch ( c ) {
		case '\r':
			putchar( c );
			putchar( '\n' );
			*r = '\0';
			break;
		case '\b':
			if ( r == s ) {
				putchar( '\007' );
			} else {
				putchar( '\010' );
				putchar( ' ' );
				putchar( '\010' );
				*--r = ' ';
			}
			break;
		default:
			putchar( c );
			*r++ = c;
			break;
		}
	} while (( c ) != '\r' );
}


/*
 * The ANSI standard rand() and srand() functions.  Ripped off from libc.
 */

unsigned long next;

rand()
{
	return (( next = next * 1103515245 + 12345 ) % 32767 );
}

srand(seed)
unsigned long seed;
{
	next = seed;
}
