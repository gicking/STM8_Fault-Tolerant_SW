/**********************
  
  Demonstrate RAM test during flash startup

  Functionality:
    - during startup perform RAM test (checkerboard or march-c)
    - handle IWDG and WWDG watchdogs (important if WDs are activated via option bytes)
    - in case of
      - no RAM error blink LED periodically 
      - RAM error perform ILLOP reset

  Supported Hardware:
    - Nucleo 8S207K8
  
  Note:
    - actual RAM tests from https://github.com/basilhussain/stm8-ram-test
    - This implementation is SDCC specific. Other toolchains need adaptations
    - Requires SDCC version >=4.2.10, see https://sourceforge.net/p/sdcc/bugs/3520/ and https://sourceforge.net/p/sdcc/bugs/3533/#7b22
    - RAM size must be provided via project options, see file "platformio.ini"

**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_it.h"     // required here by SDCC for ISR
#include "stm8s_gpio.h"
#include "stdio.h"
#include "ram_test.h"


/*----------------------------------------------------------
    MACROS / DEFINES
----------------------------------------------------------*/

// define LED pin
#if defined(STM8S_NUCLEO_207K8)
  #define PORT_TEST       (GPIOC)         // LED port 
  #define PIN_LED         (GPIO_PIN_5)    // LED pin = board D13 = STM8 PC5
#else
  #error Board not supported
#endif

// check if RAM size is specified in project options or Makefile
#if !defined(RAM_END)
  #error parameter RAM_END must be specified via project options or Makefile
#endif

/*----------------------------------------------------------
    GLOBAL FUNCTIONS
----------------------------------------------------------*/

uint8_t __sdcc_external_startup(void)
{
  // Note: checkerboard test takes ~55ms for 6kB @ fCPU = 2MHz

  // If IWDG is started via option byte, it is started with typ. 16ms timeout.
  // To avoid reset during RAM test, set a longer IWDG timeout.
  // Can be omitted if IWDG is not started via option byte
  //IWDG->KR  = 0xCC;     // start IWDG (not required if IWDG is activated via option bytes)
  IWDG->KR  = 0x55;     // unlock write access to protected registers
  IWDG->PR  = 0x04;     // set prescaler for 1kHz (=64kHz/2^(PR+2))
  IWDG->RLR = 0xFF;     // set max. timeout period (255ms @ 1kHz)
  IWDG->KR  = 0xAA;     // reload IWDG with new timeout 

  // If WWDG is started via option byte, it is started with 393.6ms timeout @ fCPU=2MHz.
  // That is sufficient for both checkerboard and march-c tests, so no WWDG handling 
  // is required here, just take into account the test duration for your initial WWDG service

  // These are actually macros that jump directly to the test routine. When
  // that returns, this means it effectively does so directly from
  // __sdcc_external_startup() itself. Anything below the test is never
  // executed; the 'return' statement is just to avoid a compiler warning.
  ram_test_checkerboard();

  // just to avoid compiler warning
  return 0;

} // __sdcc_external_startup()


/////////////////
//  main routine
/////////////////
void main(void)
{
  /////////////
  // initialization
  /////////////

  // Configure LED pin as output
  GPIO_Init(PORT_TEST, PIN_LED, GPIO_MODE_OUT_PP_LOW_FAST);


  /////////////
  // main loop
  /////////////
  while (1)
  {
    // toggle LED
    GPIO_WriteReverse(PORT_TEST, PIN_LED);
      
    // dummy wait
    for (uint32_t i=0; i<50000L; i++)
    {
      IWDG->KR  = 0xAA;   // service IWDG watchdog
      WWDG->CR  = 0x7F;   // service WWDG watchdog
    }

  } // main loop
  
} // main()

/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
