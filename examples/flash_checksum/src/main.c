/**********************
  
  Demonstrate flash Fletcher-16 checksum check.
  Note: safer CRC16 is also available in 'common/checksum', but it is ~3x slower

  Functionality:
    - during initialization calculate checksum over complete flash
    - in main loop periodically 
      - blink LED
      - calculate checksum over complete flash in background

  Supported Hardware:
    - Nucleo 8S207K8
  
  Note:
    - here only print calculated checksum, no comparison with stored value in EEPROM
    - checksum calculation is not size or speed optimized
    - the initial Fletcher-16 checksum calculation takes ~330ms (16MHz, SDCC)
    - when called every 1ms, a new checksum is available every ~65s with an additional CPU load of ~0.8% (16MHz, SDCC)

**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_it.h"     // required here by SDCC for ISR
#include "stm8s_clk.h"
#include "stm8s_gpio.h"
#include "stdio.h"
#define _MAIN_            // required for global variables
  #include "sw_clock.h"
  #include "uart_stdio.h"
  #include "checksum_fletcher16.h"
#undef _MAIN_


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

// LED blink period [ms]
#define LED_PERIOD      500

// communication speed [Baud]
#define BAUDRATE        115200L

// start/end address for checksum check
#define CHK_ADDR_START  0x8000    // flash start address
#define CHK_ADDR_END    0x17FFF   // flash end address (64kB)


/*----------------------------------------------------------
    GLOBAL FUNCTIONS
----------------------------------------------------------*/

/////////////////
//  main routine
/////////////////
void main(void)
{
  // for SW scheduler
  uint32_t  lastLED=0;

  // for checksum calculation
  uint32_t  addrChk;
  uint16_t  Chk;


  /////////////
  // initialization
  /////////////

  // disable interrupts
  disableInterrupts();
  
  // set HSI and HSE prescaler to 1 and fCPU=fMaster
  CLK->CKDIVR = 0x00;
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

  // Initialize LED pins as output low
  GPIO_Init(PORT_TEST, PIN_LED, GPIO_MODE_OUT_PP_LOW_FAST);

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

  // initial checksum calculation
  uint32_t tStart = millis();
  Chk = fletcher16_chk_range(CHK_ADDR_START, CHK_ADDR_END);
  uint32_t tEnd = millis();
  printf("initial: %ldms\t0x%04x\n", (long) (tEnd-tStart), Chk);

  // re-initialize checksum calculation
  addrChk = CHK_ADDR_START;
  Chk = fletcher16_chk_initialize();
  tStart = millis();


  /////////////
  // main loop
  /////////////
  while (1)
  {
    // check if 1ms has passed
    if (g_flagMilli)
    {
      g_flagMilli = FALSE;
    

      // update checksum value
      Chk = fletcher16_chk_update(Chk, read_1B_far(addrChk));

      // if checksum calculation is finished
      if (++addrChk > CHK_ADDR_END)
      {
        // perform final update (here dummy)
        Chk = fletcher16_chk_finalize(Chk);
        
        //////
        // compare calculated checksum with stored checksum from D-flash 
        //////
        
        // here just print checksum...
        printf("background: 0x%04x\n", Chk);
        
        // re-start checksum calculation
        addrChk = CHK_ADDR_START;
        Chk = fletcher16_chk_initialize();

      } // if addrChk > CHK_ADDR_END
      

      // task for LED blink
      if (millis() - lastLED >= LED_PERIOD)
      {
        lastLED = millis();

        GPIO_WriteReverse(PORT_TEST, PIN_LED);
        
      } // task LED

    } // if g_flagMilli

  } // main loop
  
} // main()


/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
