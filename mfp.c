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
 * File: mfp.c
 * Usage:
 */


#include "hw.h"
#include "global.h"


/*
 * mfp_init() - Initialize the multi-function peripheral chips.
 */

void mfp_init()
{
	register struct mc68901 *mfp = (struct mc68901 *) MFP;
	register struct mc68901 *mfp2 = (struct mc68901 *) MFP2;

	rb_count = 0;
	rb_first = rb_buffer;
	rb_last = rb_buffer;
	
	mfp->tacr = 0x00;	/* stop timer a */
	mfp->tbcr = 0x00;	/* stop timer b */
	mfp->tcdcr = 0x00;	/* stop baud rate timers */

	mfp->aer = 0x00;	/* skip the active edge register */
	mfp->ddr = 0x0f;	/* set the ddr as given */
	mfp->gpip = 0x0f;	/* we set the general i/o */

	mfp->iera = 0x00;	/* turn off interrupts */
	mfp->ierb = 0x00;
	mfp->imra = 0x00;
	mfp->imrb = 0x00;
	mfp->vr = 0x40;		/* set the vector base to 0x40 */

	mfp->tcdr = TCDR_9600;		/* serial port paramenters */
	mfp->tddr = TDDR_9600;
	mfp->tcdcr = TCDCR_9600;
	mfp->ucr = UCR_DV16 | UCR_8BITS | UCR_NOPARITY | UCR_1STOP;
	mfp->rsr = RSR_RE;
	mfp->tsr = TSR_HIGH;
	mfp->tsr = TSR_TE | TSR_HIGH;

	mfp->ipra = 0x00;	/* get rid of pending interrupts */
	mfp->isra = 0x00;	/* get rid of in-service interrupts */

	mfp->iera = IERA_RBF|IERA_RE|IERA_GP|IERA_TB;	/* enable interrupts */
	mfp->ierb = 0x00;

	mfp->imra = IERA_RBF|IERA_RE|IERA_GP|IERA_TB;	/* unmask interrupts */
	mfp->imrb = 0x00;

	/* MFP 2 */

	mfp2->tacr = 0x00;	/* stop timer a */
	mfp2->tbcr = 0x00;	/* stop timer b */
	mfp2->tcdcr = 0x00;	/* stop baud rate timers */

	mfp2->aer = 0x00;	/* skip the active edge register */
	mfp2->ddr = 0x00;	/* set the ddr as given */
	mfp2->gpip = 0x00;	/* we set the general i/o */

	mfp2->iera = 0x00;	/* turn off interrupts */
	mfp2->ierb = 0x00;
	mfp2->imra = 0x00;
	mfp2->imrb = 0x00;
	mfp2->vr = 0x50;	/* set the vector base to 0x40 */

	mfp2->tcdr = TCDR_9600;		/* serial port paramenters */
	mfp2->tddr = TDDR_9600;
	mfp2->tcdcr = TCDCR_9600;
	mfp2->ucr = UCR_DV16 | UCR_8BITS | UCR_NOPARITY | UCR_1STOP;
	mfp2->rsr = RSR_RE;
	mfp2->tsr = TSR_HIGH;
	mfp2->tsr = TSR_TE | TSR_HIGH;


	mfp2->ipra = 0x00;	/* get rid of pending interrupts */
	mfp2->isra = 0x00;	/* get rid of in-service interrupts */

	mfp2->iera = IERA_RBF|IERA_RE;		/* enable interrupts */
	mfp2->ierb = 0x00;

	mfp2->imra = IERA_RBF|IERA_RE;		/* unmask interrupts */
	mfp2->imrb = 0x00;

}


/*
 * putchar() - Put a character to the console.
 */

putchar(c)
	char c;
{
	register struct mc68901 *mfp = (struct mc68901 *) MFP;

	while ( ! (mfp->tsr & TSR_BUF_EMPTY));	/* wait for uart */
	mfp->udr = c;				/* put the character */
}


/*
 * getchar() - Get a character from the console.
 */

getchar()
{
	char c;
	unsigned s;

	while ( ! rb_count )
		idle();			/* wait for a character */
	s = spl( SPL_HIGH );		/* ensure privacy */
	if ( ++rb_last >= &rb_buffer[BUFSIZ] )	/* check ptr wrap-around */
		rb_last = rb_buffer;
	rb_count -= 1;				/* one less in buffer */
	c = *rb_last;				/* grab the character */
	spl( s );			/* return to old level */
	return c;			/* return the character */
}


/*
 * rx_isr() - The receiver interrupt service.
 */

rx_isr()
{
	register struct mc68901 *mfp = (struct mc68901 *) MFP;
	register char c;

	c = mfp->rsr;		/* dummy read of status register */
	c = mfp->udr;		/* get the character from the uart */

	if ( rb_count < BUFSIZ ) {
		if ( ++rb_first >= &rb_buffer[BUFSIZ] )
			rb_first = rb_buffer;
		*rb_first = c;
		rb_count += 1;
	}

	mfp->ipra = ~IERA_RBF;	/* get rid of pending interrupt */
	mfp->isra = ~IERA_RBF;	/* get rid of in-service interrupt */
}


/*
 * re_isr - The receiver error interrupt service.
 */

re_isr()
{
	register struct mc68901 *mfp = (struct mc68901 *) MFP;
	register char c;

	c = mfp->rsr;		/* dummy read of status register */
	c = mfp->udr;		/* get the character from the uart */
#ifdef LINT
	c = c;
#endif
	mfp->ipra = ~IERA_RE;	/* get rid of pending interrupt */
	mfp->isra = ~IERA_RE;	/* get rid of in-service interrupt */
}


/*
 * rx2_isr() - The receiver 2 interrupt service.
 */

rx2_isr()
{
	register struct mc68901 *mfp2 = (struct mc68901 *) MFP2;
	register char c;

	c = mfp2->rsr;		/* dummy read of status register */
	c = mfp2->udr;		/* get the character from the uart */

	/*
	 * Handle the music information right away.
	 */

	if ( c == MR_BEAT ) {
		++mbeat;
	} else {
		mlevel = (c - MR_IBASE);
	}

	mfp2->ipra = ~IERA_RBF;	/* get rid of pending interrupt */
	mfp2->isra = ~IERA_RBF;	/* get rid of in-service interrupt */
}


/*
 * re2_isr - The receiver 2 error interrupt service.
 */

re2_isr()
{
	register struct mc68901 *mfp2 = (struct mc68901 *) MFP2;
	register char c;

	c = mfp2->rsr;		/* dummy read of status register */
	c = mfp2->udr;		/* get the character from the uart */
#ifdef LINT
	c = c;
#endif
	mfp2->ipra = ~IERA_RE;	/* get rid of pending interrupt */
	mfp2->isra = ~IERA_RE;	/* get rid of in-service interrupt */
}


/*
 * gp_isr - The 60Hz interrupt service.
 */

gp_isr()
{
	register struct mc68901 *mfp = (struct mc68901 *) MFP;
	unsigned s;

	fill_reg( 0 );			/* update the light regs for step 0 */
	mfp->ipra = ~IERA_GP;		/* get rid of pending interrupt */
	mfp->isra = ~IERA_GP;		/* get rid of in-service interrupt */
	mfp->tbcr = 0x00;		/* make sure timer is off */
	mfp->tbdr = TBDR_1440HZ;	/* start fast interrupts */
	mfp->tbcr = TBCR_1440HZ;	/* at 1440 hz */
	++ticks;			/* increment time-keeper */
	if ( ++stick >= HERTZ ) {	/* see if a second has passed */
		++seconds;		/* increment if so */
		stick = 0;
	}
	dim_step = 0;			/* reset to step 0 */

	/*
	 * It's time to update the lights.
	 * This will be done in interrupt service,
	 * but interrupts will be enabled.
	 * To protect against getting behind,
	 * a guard variable will protect us.
	 */

	if ( ++ttick >= ( LPTICKS ) ) {
		ttick = 0;
		if ( guard ) {
			guard_hits++;		/* the guard saved us */
		} else {
			guard = 1;		/* set the guard flag */
			s = spl( SPL_LOW );	/* reenable interrupts */
			run_loops();		/* run the loops */

			/*
			 * Once per second clock
			 * processing.
			 */

			if ( seconds != lasttime ) {
				lasttime = seconds;
				clock_proc();
			}

			spl( s );		/* disable interrupts */
			last_step = dim_step;	/* statistics */
			guard = 0;		/* clear the guard flag */
		}
	}
}


/*
 * tb_isr - The 960Hz interrupt service.
 */

tb_isr()
{
	register struct mc68901 *mfp = (struct mc68901 *) MFP;

	dim_step++;
	fill_reg( dim_step );
	mfp->ipra = ~IERA_TB;	/* get rid of pending interrupt */
	mfp->isra = ~IERA_TB;	/* get rid of in-service interrupt */
	if ( dim_step >= ( DIMSTEPS - 1 )) {	/* see if time */
		mfp->tbcr = 0x00;			/* to stop intrs */
		++full_count;				/* for sanity checks */
	}
}
