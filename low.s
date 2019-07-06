|
| Copyright (c) 1991 Eric A. Edwards
|
| This file is part of Cato, an implemenation of D.A.M.M.I.T.
|
| Permission is hereby granted, free of charge, to any person obtaining a copy
| of this software and associated documentation files (the "Software"), to deal
| in the Software without restriction, including without limitation the rights
| to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
| copies of the Software, and to permit persons to whom the Software is
| furnished to do so, subject to the following conditions:
|
| The above copyright notice and this permission notice shall be included in all
| copies or substantial portions of the Software.
|
| THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
| IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
| FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
| AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
| LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
| OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
| SOFTWARE.
|
| File: low.s
| Usage:
|

	text
.	=	$000000
	data
.	=	$000000
	bss
.	=	$040000

	text

	global	_start
	global	_mfp_init
	global	_main
	global	_getchar
	global	_putchar
	global	_spl
	global	_idle
	global	_rx_isr
	global	_re_isr
	global	_rx2_isr
	global	_re2_isr
	global	_gp_isr
	global	_tb_isr
	global	_stack
	global	_edata

|
| Exception vectors.
|

	| exception vectors 0-15

	dc.l	_stack		| reset: initial SP
	dc.l	start		| reset: initial PC
	dc.l	buserr		| bus error
	dc.l	adderr		| address error
	dc.l	except		| illegal instruction
	dc.l	except		| zero divide
	dc.l	except		| CHK instr
	dc.l	except		| TRAPV instr
	dc.l	except		| priv violation
	dc.l	except		| trace trap
	dc.l	except		| instr line 1010 emulation
	dc.l	except		| instr line 1111 emulation
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| format error
	dc.l	except		| uninitialized interrupt

	| exception vectors 16-23 (reserved)

	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)

	| exception vector 24

	dc.l	except		| spurious interrupt

	| exception vectors 25-31

	dc.l	except		| auto vector interrupt level 1
	dc.l	except		| auto vector interrupt level 2
	dc.l	auto3		| auto vector interrupt level 3
	dc.l	except		| auto vector interrupt level 4
	dc.l	except		| auto vector interrupt level 5
	dc.l	except		| auto vector interrupt level 6
	dc.l	except		| auto vector interrupt level 7

	| exception vectors 32-47

	dc.l	except		| trap instruction 0
	dc.l	except		| trap instruction 1
	dc.l	except		| trap instruction 2
	dc.l	except		| trap instruction 3
	dc.l	except		| trap instruction 4
	dc.l	except		| trap instruction 5
	dc.l	except		| trap instruction 6
	dc.l	except		| trap instruction 7
	dc.l	except		| trap instruction 8
	dc.l	except		| trap instruction 9
	dc.l	except		| trap instruction 10
	dc.l	except		| trap instruction 11
	dc.l	except		| trap instruction 12
	dc.l	except		| trap instruction 13
	dc.l	except		| trap instruction 14
	dc.l	except		| trap instruction 15

	| exception vectors 48-58

	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)

	| exception vectors 59-63

	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)
	dc.l	except		| (reserved)

	| exception vectors 64-95

	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except

	| exception vectors 96-127

	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except

	| exception vectors 128-159

	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except

	| exception vectors 160-191

	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except

	| exception vectors 192-223

	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except

	| exception vectors 224-255

	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except
	dc.l	except,except,except,except


|
| Application startup code. Clears uninitialized ram, sets up a
| stack and then runs the application.
|

start:
_start:
	move.w	#$2700,sr	| set up the status register
	move.w	#$0000,$08010e	| make sure the VHA junk won;'t
	move.w	#$0001,$080104	| bother us
	move.w	#$0000,$080188
	move.w	#$30c5,$08018c
	nop
	reset			| reset the local hardware
	nop
	move.w	#$0000,$08010e	| make sure the VHA junk won't
	move.w	#$0001,$080104	| bother us
	move.w	#$0000,$080188
	move.w	#$30c5,$08018c
	clr.l	d0		| clear out ram and stack areas
	move.l	#$40000,a0
	move.l	#_stack,a1
cloop:	move.b	d0,(a0)+	| clear a byte
	cmp.l	a0,a1		| loop until done
	bne	cloop
	move.l	#_stack,sp	| set the top of stack location
	jsr	_mfp_init	| init the junk
	move.w	#$2000,sr	| enable interrupts
	jsr	_main		| run the application
	bra	start		| go back to start if trap returns


|
| Bus error handler.
|

buserr:
adderr:
	stop	#$2700
	rte


|
| Unexpected interrupt handler.
|

except:
	stop	#$2700
	rte


|
| Interrupt service stub.
|

auto3:
	movem.l	#$c0c0,-(sp)
	move.b	$08000b,d0
	btst	#7,d0
	beq.s	au1
	jsr	_gp_isr
	bra.s	aud
au1:
	btst	#4,d0
	beq.s	au2
	jsr	_rx_isr
	bra.s	aud
au2:
	btst	#3,d0
	beq.s	au3
	jsr	_re_isr
	bra.s	aud
au3:
	btst	#0,d0
	beq.s	au4
	jsr	_tb_isr
	bra.s	aud
au4:
	move.b	$0a000b,d0
	btst	#4,d0
	beq.s	au5
	jsr	_rx2_isr
	bra.s	aud
au5:
	btst	#3,d0
	beq.s	au6
	jsr	_re2_isr
	bra.s	aud
au6:
	movem.l	(sp)+,#$0303
	bra	except
aud:
	movem.l	(sp)+,#$0303
	rte


|
| Set Processor Level.
|

_spl:
	move.w	sr,d0
	move.w	4(sp),sr
	rts


|
| Idle until next interrupt.
|

_idle:
	stop	#$2000
	rts

