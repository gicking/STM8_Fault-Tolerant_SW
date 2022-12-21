/**********************  
  Dummy test project to try out things
**********************/
#include "stm8s.h"

// Called by SDCC prior to jump to main(), see e.g. http://www.gtoal.com/compilers101/small_c/gbdk/sdcc/doc/sdccman.html/node31.html
// return 0/1: initialize global variables / skip initialization
uint8_t _sdcc_external_startup()
{
  //WWDG->CR = WWDG_CR_WDGA;      // trigger reset

  return 0;
}


//  main routine
void main(void)
{
  // initialize PC5/LED as output
  GPIOC->DDR = (1<<5);    // input(=0) or output(=1)
  GPIOC->CR1 = (1<<5);    // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
  GPIOC->CR1 = (1<<5);    // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope

  // main loop. Just blink LED
  while (1)
  {
    GPIOC->ODR ^= (1<<5);
    for (uint32_t i=0; i<40000L; i++)
      nop();
  }
  
}
