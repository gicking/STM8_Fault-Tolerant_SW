/**********************
  
  Implement RAM checkerboard test in SDCC startup routine

  Note:
    - Actual RAM test in assembler by Basil Hussain (https://github.com/basilhussain)
    - Requires SDCC version >=4.2.10, see https://sourceforge.net/p/sdcc/bugs/3520/
    - Other toolchains need different implementations

**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "stdio.h"


/*----------------------------------------------------------
    MACROS / DEFINES
----------------------------------------------------------*/

// Highest RAM address. Check device datasheet for correct value
#define RAM_END           0x17FF

// for testing purposes only! Uncomment to provoke a failure
//#define TRIGGER_ERROR


/*----------------------------------------------------------
    GLOBAL FUNCTIONS
----------------------------------------------------------*/

// Flash startup code, is called prior to jump to main(). 
// For reference see http://www.gtoal.com/compilers101/small_c/gbdk/sdcc/doc/sdccman.html/node31.html
// return: 0: initialize globals; 1: skip initialization of globals 
void __sdcc_external_startup(void) __naked
{
    __asm
#ifdef __SDCC_MODEL_LARGE
        ; Return address on stack is 3 bytes, but because GSINIT section (where
        ; we are called from) will always reside very near start of flash at
        ; 0x8000, the MSB will always be zero (e.g. 0x0080nn) and can be
        ; discarded, leaving only the two LSBs to be saved in Y reg.
        pop a
        popw y
#else // __SDCC_MODEL_LARGE
        ; Save the 16-bit return address from the stack in Y reg, because
        ; otherwise it will be overwritten.
        popw y
#endif // __SDCC_MODEL_LARGE

        ; Start off initially with checkerboard pattern 0x55 (0b01010101).
        ld a, #0x55

    0001$:
        ldw x, #RAM_END

    0002$:
        ; Fill entire RAM with test pattern.
        ld (x), a
        decw x
        jrpl 0002$

#ifdef TRIGGER_ERROR
        ; test only: provoke RAM error
        bset 0x0010, #7
#endif

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
        ; Set return value to 0 for initialization of globals 
        ld a, #0
        ; Far return 
        retf
#else // __SDCC_MODEL_LARGE
        ; Put the return address back on to the stack and return.
        pushw y
        ; Set return value to 0 for initialization of globals 
        ld a, #0
        ; Return 
        ret
#endif // __SDCC_MODEL_LARGE

    9999$:
        ; Upon failure, perform a software reset by executing an illegal opcode.
        .db 0x75

    __endasm;
}


/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
