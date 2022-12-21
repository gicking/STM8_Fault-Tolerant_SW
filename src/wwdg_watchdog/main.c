/**********************
  
  Demonstrate use of WWDG window watchdog to assert execution time.
  Omit use of SPL function for actual watchdog service. 

  Functionality:
    - initialization:
      - configure WWDG (max timeout, no closed window)
      - configure LED pin to output
      - initialize SW clock
      - configure UART @ 115.2kBaud / 8N1 
      - print reset source via UART
      - blocking wait 1s (with dummy WWDG service)
    - main loop
      - blink LED periodically
      - call some test routines with individual runtime check via WWDG      
      - if UART receives
        - 'w': endless loop -> reset
        - 'W': WWDG service in closed window -> reset
        - 's': skip one test routine -> reset
        - 'S': extend one test routine -> reset

  Supported Hardware:
    - Nucleo 8S207K8
  
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_it.h"     // required here by SDCC for ISR
#include "stm8s_clk.h"
#include "stm8s_uart3.h"
#include "stm8s_wwdg.h"
#include "stdio.h"
#define _MAIN_            // required for global variables
  #include "sw_clock.h"
  #include "uart_stdio.h"
#undef _MAIN_


/*----------------------------------------------------------
    MACROS / DEFINES
----------------------------------------------------------*/

// define LED pin
#if defined(STM8S_NUCLEO_207K8)
  #define PORT_TEST       (GPIOC)               ///< LED port 
  #define PIN_LED         (GPIO_PIN_5)          ///< LED pin = board D13 = STM8 PC5
#else
  #error Board not supported
#endif

/// LED blink period [ms]
#define LED_PERIOD      500

/// communication speed [Baud]
#define BAUDRATE        115200L

/// set WWDG timeout and open window [768us] (=12288/16MHz). Values clipped to 64 (=49.152ms) 
#define WWDG_SETCOUNTER(period)   (WWDG->CR = 0xC0 | (period - 1))
#define WWDG_OPENWINDOW(window)   (WWDG->WR = 0xC0 | (window - 1))


/*----------------------------------------------------------
    GLOBAL FUNCTIONS
----------------------------------------------------------*/

/////////////////
// test routine 1
/////////////////
void test_1(void)
{
  // wait some time to emulate CPU load
  delay(1);

} // test_1()



/////////////////
// test routine 2
/////////////////
void test_2(void)
{
  // wait some time to emulate CPU load
  delay(5);

} // test_2()



/////////////////
// test routine 3
/////////////////
void test_3(void)
{
  // wait some time to emulate CPU load
  delay(10);

} // test_3()



/////////////////
// test routine 4
/////////////////
void test_4(void)
{
  // wait some time to emulate CPU load
  delay(20);

} // test_4()



/////////////////
//  main routine
/////////////////
void main(void)
{
  uint32_t  lastLED=0;              // for SW scheduler
  uint8_t   flagTest2 = 0;          // control calling test routine
  

  /////////////
  // initialization
  /////////////

  // disable interrupts
  disableInterrupts();
  
  // set HSI and HSE prescaler to 1 and fCPU=fMaster
  CLK->CKDIVR = 0x00;
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

  // start WWDG window watchdog [768us]. Values clipped to 64 (=49.152ms) 
  WWDG_SETCOUNTER(64);
  WWDG_OPENWINDOW(64);

  // Initialize LED pins as output high
  GPIO_Init(PORT_TEST, PIN_LED, GPIO_MODE_OUT_PP_HIGH_FAST);

  // start 1ms clock via TIM4
  init_SW_clock();

  // Configure UART3 for 115kBaud, 8N1
  UART3_Init(BAUDRATE, UART3_WORDLENGTH_8D, UART3_STOPBITS_1, UART3_PARITY_NO, UART3_MODE_TXRX_ENABLE);

  // bind stdio input/output to UART3
  g_UART_SendData8 = &UART3_SendData8;
  g_UART_ReceiveData8 = &UART3_ReceiveData8;
  g_UART_GetFlagStatus = &UART3_GetFlagStatus;

  // enable interrupts
  enableInterrupts();

  // print reset source
  printf("\nreset source: ");
  if (!RST_GetFlagStatus(RST_FLAG_EMCF | RST_FLAG_SWIMF | RST_FLAG_ILLOPF | RST_FLAG_IWDGF | RST_FLAG_WWDGF))
    printf("HW / BOR\n");
  if (RST_GetFlagStatus(RST_FLAG_EMCF))
    printf("EMC\n");
  if (RST_GetFlagStatus(RST_FLAG_SWIMF))
    printf("SWIM\n");
  if (RST_GetFlagStatus(RST_FLAG_ILLOPF))
    printf("ILLOP\n");
  if (RST_GetFlagStatus(RST_FLAG_IWDGF))
    printf("IWDG\n");
  if (RST_GetFlagStatus(RST_FLAG_WWDGF))
    printf("WWDG\n");
  RST_ClearFlag(RST_FLAG_EMCF | RST_FLAG_SWIMF | RST_FLAG_ILLOPF | RST_FLAG_IWDGF | RST_FLAG_WWDGF);


  // debug: measure timeout periods of WWDG via LED pin (=D13)
  #if (0)
    GPIO_WriteLow(PORT_TEST, PIN_LED);
    delay(1);
    WWDG_SETCOUNTER(26);                   // period [768us]. Value clipped to 64 (=49.152ms) 
    GPIO_WriteHigh(PORT_TEST, PIN_LED);
    while(1);
  #endif

  // debug: measure WWGD open window via LED pin (=D13)
  #if (0)
    uint8_t   openWindow = 32;          // open window [768us]. Value clipped to 64 (=49.152ms) 
    WWDG_SETCOUNTER(0xFF);
    WWDG_OPENWINDOW(openWindow);
    while (1)
    { 
      uint32_t timeStart = micros();
      while ((WWDG_GetCounter() & 0x3F) >= openWindow);     // wait for open window
      uint32_t timeEnd = micros();
      printf("closed window: %ldus\n", timeEnd - timeStart);
      WWDG_SETCOUNTER(0xFF);
    }
  #endif


  // dummy wait for 1s (measured). Use a simple exit condition and service WWDG w/o closed window
  printf("wait ... ");
  uint32_t tStart = millis();
  for (uint32_t i=0; i<842100L; i++)
    WWDG_SETCOUNTER(64);
  printf("done (%ldms)\n", millis() - tStart);
  

  /////////////
  // main loop
  /////////////
  while (1)
  {
    // blink LED to indicate activity
    if (millis() - lastLED > LED_PERIOD)
    {
      lastLED = millis();
      GPIO_WriteReverse(PORT_TEST, PIN_LED);
    } // task LED


    /////////
    // call some routines which emulate CPU load
    /////////
    
    // call test routine 1 to emulate CPU load. Runtime 1ms
    // service WWDG [768us] and set 2.3ms period, no closed window (timing too tight)
    WWDG_SETCOUNTER(3);
    WWDG_OPENWINDOW(3);
    test_1();
    

    // call test routine 1 to emulate CPU load. Runtime 5ms
    // config WWDG [768us] for 7.7ms period, 5.3ms open window
    WWDG_SETCOUNTER(10);
    WWDG_OPENWINDOW(7);
    if (flagTest2 == 0)             // normal call
      test_2();
    else if (flagTest2 == 1)        // skip -> time too short
      ;
    else                            // add delay -> time too long
    {
      test_2();
      delay(5);
    }


    // call test routine 1 to emulate CPU load. Runtime 10ms
    // config WWDG for 13.1ms period, 7.7ms open window 
    WWDG_SETCOUNTER(17);
    WWDG_OPENWINDOW(10);
    test_3();


    // call test routine 1 to emulate CPU load. Runtime 20ms
    // config WWDG for 24.6ms period, 10.0ms open window 
    WWDG_SETCOUNTER(32);
    WWDG_OPENWINDOW(13);
    test_4();
 

    // if byte received from serial monitor
    if (UART3_GetFlagStatus(UART3_FLAG_RXNE))
    {
      char c = getchar();
      
      // if 'w' received, disable WWDG service --> reset
      if (c == 'w')
      {
        // print message
        printf("endless loop\n");

        // set flag
        while(1);

      } // received 'w'

      // if 'W' received, service WWDG 2x without pause --> reset
      else if (c == 'W')
      {
        // print message
        printf("service in closed window\n");

        // WWDG double service with 50% closed window --> reset
        WWDG_SETCOUNTER(64);              // 49ms timeout
        WWDG_OPENWINDOW(32);              // 50% closed/open windows
        WWDG_SETCOUNTER(64);              // double service -> reset

      } // received 'W'
      
      // if 's' received, skip one test routine -> reset
      else if (c == 's')
      {
        // print message
        printf("skip test_2()\n");

        // set flag to skip call
        flagTest2 = 1;

      } // received 's'
      
      // if 'S' received, extend one test routine -> reset
      else if (c == 'S')
      {
        // print message
        printf("extend test_2()\n");

        // set flag to call twice
        flagTest2 = 2;

      } // received 'S'

    } // byte received

  } // main loop
  
} // main()


/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
