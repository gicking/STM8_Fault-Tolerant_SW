/*******************************************************************************
 *
 * ram_test.h - STM8 RAM test library header
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

#ifndef RAM_TEST_H__
#define RAM_TEST_H__

// IMPORTANT: ALWAYS USE THESE MACROS; DO NOT DIRECTLY CALL THE FUNCTIONS
// DECLARED BELOW.
// The purpose of these macros is to bypass any sub-call to the test function,
// avoiding creating a new stack frame (and thus an extra return address on the
// stack that would need to be preserved during the test), so that when the test
// function returns, it returns directly from __sdcc_external_startup() instead.
#ifdef __SDCC_MODEL_LARGE
    #define ram_test_checkerboard() do { __asm__("jpf _ram_test_checkerboard_impl"); } while(0)
    #define ram_test_march_c() do { __asm__("jpf _ram_test_march_c_impl"); } while(0)
#else
    #define ram_test_checkerboard() do { __asm__("jp _ram_test_checkerboard_impl"); } while(0)
    #define ram_test_march_c() do { __asm__("jp _ram_test_march_c_impl"); } while(0)
#endif

// WARNING: DO NOT CALL THESE FUNCTIONS DIRECTLY. USE THE MACROS ABOVE.
extern unsigned char ram_test_checkerboard_impl(void);
extern unsigned char ram_test_march_c_impl(void);

#endif
