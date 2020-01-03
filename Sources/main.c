#include "HAL/clock_hal.h"
#include "HAL/gpio_hal.h"
#include "HAL/uart_hal.h"
#include "HAL/spi_hal.h"
#include "modules/log/log.h"

volatile int exit_code = 0;
uint8_t debug_verbose = 5;
uint8_t debug_instance = 0;
uint8_t spi_instance = 0;

int main(void)
{
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/
    uint8_t rxBuf[10];

    // Initial Clock Manager
    clock_drv_init();

	// Initial Pin Mux
	gpio_drv_init();

    uart_drv_init(debug_instance);
    spi_drv_init(spi_instance, SPI_MASTER_MODE);
    spi_cs_high(spi_instance);
    gpio_set_level(111, GPIO_LEVEL_HIGH);
    log_debug("Program Start!");

    spi_set_baudrate(spi_instance, 100000);
    while(rxBuf[0] != 'q') {
    	uart_recv(debug_instance, rxBuf, 1);
    }

    uint8_t data[1] = {0x33};
    spi_cs_low(spi_instance);
    spi_write_array(spi_instance, data, 1);
    spi_cs_high(spi_instance);
    gpio_set_level(111, GPIO_LEVEL_LOW);
    log_debug("Program Terminated!");




  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/
