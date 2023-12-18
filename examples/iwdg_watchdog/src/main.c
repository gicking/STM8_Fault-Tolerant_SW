/**********************
  
  Demonstrate use of IWDG timeout watchdog with state machine.
  Omit use of SPL function for actual watchdog service.

  Functionality:
    - initialization:
      - configure IWDG (20ms timeout)
      - configure LED pin to output
      - initialize SW clock
      - configure UART @ 115.2kBaud / 8N1 
      - print reset source via UART
      - blocking wait 1s (with dummy IWDG service)
    - main loop
      - blink LED periodically
      - call some test routines with total runtime 10ms
      - if UART receives
        - 'i': disable IWDG service -> reset
        - 'I': 2x IWDG service -> no reset
        - 's': skip one test routine -> reset
      - service IWDG if all sub-routines have been executed

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
#include "stm8s_iwdg.h"
#include "stdio.h"
#define _MAIN_            // required for global variables
  #include "sw_clock.h"
  #include "uart_stdio.h"
  #include "checksum_crc16.h"
#undef _MAIN_


/*----------------------------------------------------------
    MACROS / DEFINES
----------------------------------------------------------*/

// define LED pin
#if defined(STM8S_NUCLEO_207K8)
  #define PORT_TEST       (GPIOC)                       ///< LED port 
  #define PIN_LED         (GPIO_PIN_5)                  ///< LED pin = board D13 = STM8 PC5
#else
  #error Board not supported
#endif

/// LED blink period [ms]
#define LED_PERIOD      500

/// communication speed [Baud]
#define BAUDRATE        115200L

// IWDG prescaler and timeout
#define IWDG_PRESCALER  IWDG_Prescaler_64               ///< IWDG clock prescaler 1kHz (=LSI/2/64)
#define IWDG_RELOAD     20                              ///< IWDG reload value (= 64kHz/PRE/1000*timeout[ms])
#define IWDG_SERVICE()  (IWDG->KR = IWDG_KEY_REFRESH)   ///< reload IWDG counter
#define IWDG_CRC16_VAL  0x89C3                          ///< CRC16-CCITT checksum for [1,2,3,4]. Adapt accordingly

/*----------------------------------------------------------
    GLOBAL VARIABLES
----------------------------------------------------------*/

volatile uint16_t   iwdg_crc16;     ///< CRC16-CCITT checksum to assert SW flow


/*----------------------------------------------------------
    GLOBAL FUNCTIONS
----------------------------------------------------------*/

/////////////////
// test routine 1
/////////////////
void test_1(void)
{
  // update CRC16 with unique key for this function
  iwdg_crc16 = crc16_ccitt_update(iwdg_crc16, 0x01);
  
  // wait some time to emulate CPU load
  delay(2);

} // test_1()



/////////////////
// test routine 2
/////////////////
void test_2(void)
{
  // update CRC16 with unique key for this function
  iwdg_crc16 = crc16_ccitt_update(iwdg_crc16, 0x02);

  // wait some time to emulate CPU load
  delay(2);

} // test_2()



/////////////////
// test routine 3
/////////////////
void test_3(void)
{
  // update CRC16 with unique key for this function
  iwdg_crc16 = crc16_ccitt_update(iwdg_crc16, 0x03);

  // wait some time to emulate CPU load
  delay(3);

} // test_3()



/////////////////
// test routine 4
/////////////////
void test_4(void)
{
  // update CRC16 with unique key for this function
  iwdg_crc16 = crc16_ccitt_update(iwdg_crc16, 0x04);
    
  // wait some time to emulate CPU load
  delay(3);

} // test_4()



/////////////////
//  main routine
/////////////////
void main(void)
{
  uint32_t  lastLED=0;              // for SW scheduler
  bool      flagIWDG = TRUE;        // control IWDG service
  bool      flagTest2 = TRUE;       // control calling test routine


  /////////////
  // initialization
  /////////////

  // disable interrupts
  disableInterrupts();
  
  // set HSI and HSE prescaler to 1 and fCPU=fMaster
  CLK->CKDIVR = 0x00;
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

  // configure IWDG timeout watchdog and initial service
  IWDG_Enable();                                    // enable IWDG (LSI is enabled by HW)
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);     // unlock protected registers
  IWDG_SetPrescaler(IWDG_PRESCALER);                // set IWDG clock prescaler
  IWDG_SetReload(IWDG_RELOAD);                      // set IWDG timeout
  IWDG_SERVICE();                                   // inital IWDG service and lock protected registers

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


  // debug: measure timeout periods of IWDG via LED pin (=D13)
  #if (0)
    while(1);
  #endif


  // dummy wait for 1s. Use a simple exit condition and service IWDG!
  printf("wait ... ");
  uint32_t tStart = millis();
  for (uint32_t i=0; i<1455000L; i++)
    IWDG_SERVICE();
  printf("done (%ldms)\n", millis() - tStart);
  

  /////////////
  // main loop
  /////////////
  while (1)
  {
    // initialize global IWDG checksum (is updated in sub-routines)
    iwdg_crc16 = crc16_ccitt_initialize();
    

    // blink LED to indicate activity
    if (millis() - lastLED > LED_PERIOD)
    {
      lastLED = millis();
      GPIO_WriteReverse(PORT_TEST, PIN_LED);
    } // task LED


    // call some routines which emulate CPU load. Total runtime ~10ms
    test_1();
    if (flagTest2 == TRUE)    // optionally provoke a checksum error
      test_2();
    test_3();
    test_4();


    // if byte received from serial monitor
    if (UART3_GetFlagStatus(UART3_FLAG_RXNE))
    {
      char c = getchar();
      
      // if 'i' received, disable IWDG service --> reset
      if (c == 'i')
      {
        // print message
        printf("stop IWDG service\n");

        // set flag
        flagIWDG = FALSE;

      } // received 'i'
      
      // if 'I' received, service IWDG 2x without pause -> no reset
      else if (c == 'I')
      {
        // print message
        printf("2x IWDG service\n");

        // IWDG double service --> no reset, because only timeout watchdog
        IWDG_SERVICE();
        IWDG_SERVICE();

      } // received 'I'
      
      // if 's' received, skip one test routine -> reset
      else if (c == 's')
      {
        // print message
        printf("skip test_2()\n");

        // set flag
        flagTest2 = FALSE;

      } // received 's'

    } // byte received
 
    // service IWDG watchdog only if checksum is correct -> routines have been called in correct order
    iwdg_crc16 = crc16_ccitt_finalize(iwdg_crc16);
    if ((iwdg_crc16 == IWDG_CRC16_VAL) && (flagIWDG == TRUE))
      IWDG_SERVICE();

  } // main loop
  
} // main()


/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
