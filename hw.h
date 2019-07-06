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
 * File: hw.h
 * Usage:
 */


/*
 * Defines for VHA processor physical address map
 */

#define	ROM		0x000000
#define	RAM		0x040000
#define	MFP		0x080000
#define	MFP2		0x0a0000
#define SW		0x080102


/* 
 * Set processor level define.
 */

#define SPL_HIGH	0x2700
#define SPL_LOW 	0x2000


/*
 * 68901 MFP register map.
 */

struct mc68901 {
	unsigned char		spare1;
	unsigned char	gpip;
	unsigned char		spare2;
	unsigned char	aer;
	unsigned char		spare3;
	unsigned char	ddr;
	unsigned char		spare4;
	unsigned char	iera;
	unsigned char		spare5;
	unsigned char	ierb;
	unsigned char		spare6;
	unsigned char	ipra;
	unsigned char		spare7;
	unsigned char	iprb;
	unsigned char		spare8;
	unsigned char	isra;
	unsigned char		spare9;
	unsigned char	isrb;
	unsigned char		spare10;
	unsigned char	imra;
	unsigned char		spare11;
	unsigned char	imrb;
	unsigned char		spare12;
	unsigned char	vr;
	unsigned char		spare13;
	unsigned char	tacr;
	unsigned char		spare14;
	unsigned char	tbcr;
	unsigned char		spare15;
	unsigned char	tcdcr;
	unsigned char		spare16;
	unsigned char	tadr;
	unsigned char		spare17;
	unsigned char	tbdr;
	unsigned char		spare18;
	unsigned char	tcdr;
	unsigned char		spare19;
	unsigned char	tddr;
	unsigned char		spare20;
	unsigned char	scr;
	unsigned char		spare21;
	unsigned char	ucr;
	unsigned char		spare22;
	unsigned char	rsr;
	unsigned char		spare23;
	unsigned char	tsr;
	unsigned char		spare24;
	unsigned char	udr;
};


/*
 *
 * Defines for bits in 68901 MFP UART registers
 * 
 */

#define GPIP_LED	0x01

#define UCR_DV16	0x80
#define UCR_DV1		0x00
#define UCR_8BITS	0x00
#define UCR_7BITS	0x20
#define UCR_6BITS	0x40
#define UCR_5BITS	0x60
#define	UCR_PARITY	0x04
#define	UCR_NOPARITY	0x00
#define UCR_0STOP	0x00
#define UCR_1STOP	0x08
#define UCR_15STOP	0x10
#define UCR_2STOP	0x18
#define UCR_EVEN	0x02
#define UCR_ODD		0x00

#define RSR_BUF_FULL	0x80
#define RSR_OVERRUN	0x40
#define RSR_PARITY	0x20
#define RSR_FRAME	0x10
#define RSR_BREAK	0x08
#define RSR_RE		0x01

#define TSR_BUF_EMPTY	0x80
#define TSR_UNDERRUN	0x40
#define TSR_AUTOTURN	0x20
#define TSR_EOT		0x10
#define TSR_BREAK	0x08
#define TSR_HIGH	0x04
#define TSR_LOW		0x02
#define TSR_TE		0x01

#define TCDCR_9600	0x11
#define TCDR_9600	2
#define TDDR_9600	2

#define TADR_60HZ	0xd0
#define TACR_60HZ	0x07

#define TBDR_960HZ	0x33
#define TBCR_960HZ	0x04
#define TBDR_1440HZ	0x6c
#define TBCR_1440HZ	0x03

#define IERA_GP		0x80
#define IERA_TA		0x20
#define IERA_RBF	0x10
#define IERA_RE		0x08
#define IERA_TB		0x01


/*
 * Dimming hardware register.
 */

#define DIM_REG0	0x080108
#define DIM_REG1	0x08018e
