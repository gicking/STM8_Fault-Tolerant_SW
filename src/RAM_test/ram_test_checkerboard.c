/*******************************************************************************
 *
 * ram_test_checkerboard.c - Checkerboard RAM test implementation
 * 
 * from https://github.com/basilhussain/stm8-ram-test
 *
 * Copyright (c) 2023 Basil Hussain
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ******************************************************************************/

#include "ram_test.h"

// 30 bytes
unsigned char ram_test_checkerboard_impl(void) __naked {
	__asm
#ifdef __SDCC_MODEL_LARGE
		; Return address on stack is 3 bytes, but because GSINIT section (where
		; we are called from) will always reside very near start of flash at
		; 0x8000, the MSB will always be zero (e.g. 0x0080nn) and can be
		; discarded, leaving only the two LSBs to be saved in Y reg.
		pop a
		popw y
#else
		; Save the 16-bit return address from the stack in Y reg, because
		; otherwise it will be overwritten.
		popw y
#endif

		; Start off initially with checkerboard pattern 0x55 (0b01010101).
		ld a, #0x55

	0001$:
		ldw x, #RAM_END

	0002$:
		; Fill entire RAM with test pattern.
		ld (x), a
		decw x
		jrpl 0002$

		;;;;;;;;;;;;
		; TEST ONLY: provoke RAM error -> permanent reset cycle!
        ;;;;;;;;;;;;
		;;;bset 0x0010, #7

		ldw x, #RAM_END

	0003$:
		; Read back all RAM bytes and compare each to pattern. If any differ,
		; jump to failure action.
		cp a, (x)
		jrne 9999$
		decw x
		jrpl 0003$

		; Invert the pattern. When bit 7 is set - i.e. it becomes 0xAA
		; (0b10101010) - go back for a second pass. Otherwise, when inverted
		; back to 0x55 (bit 7 is clear), testing is finished so continue.
		cpl a
		jrmi 0001$

#ifdef __SDCC_MODEL_LARGE
		; We previously only saved the 2 LSBs of the return address, so restore
		; the MSB to a fixed value of zero, then do a far return.
		pushw y
		push #0x00
		ld a, #0
		retf
#else
		; Put the return address back on to the stack and return.
		pushw y
		ld a, #0
		ret
#endif

	9999$:
		; Upon failure, perform a software reset by executing an illegal opcode.
		.db 0x75
	__endasm;
}
