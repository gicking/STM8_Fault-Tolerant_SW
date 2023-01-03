/**********************
  
  Demonstrate RAM checkerboard test during flash startup

  Functionality:
    - during startup perform checkerboard RAM test (see RAM_test.c)
    - in case of RAM error perform ILLOP reset
    - if no RAM error blink LED periodically 

  Supported Hardware:
    - Nucleo 8S207K8
  
  Note:
    - Requires SDCC version >=4.2.10, see https://sourceforge.net/p/sdcc/bugs/3520/
    - Other toolchains need different implementations

**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stdio.h"


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


/*----------------------------------------------------------
    GLOBAL FUNCTIONS
----------------------------------------------------------*/

//////////
// Startup routine with RAM test is in file "RAM_test.c"
//////////



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
      nop();

  } // main loop
  
} // main()


/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
