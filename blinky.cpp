/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif
#include "DigitalIoPin.h"
#include <cr_section_macros.h>
#include<cmath>
#include <cstdlib>
#include <string.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

#include "FreeRTOS.h"
#include "task.h"
#include "heap_lock_monitor.h"
#include "semphr.h"
#include "LpcUart.h"
#include "ITM_write.h"



/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Sets up system hardware */
static void prvSetupHardware(void)
{

	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */

	Board_LED_Set(0, false);

}


static void vExercise2(void *pvParameters) {
	int codeCnt = 0;
	bool rest = false;
	bool greenOn = false;
	bool shortCode = true;
	bool redOn = false;


	while(1){
		if(codeCnt==3){
			if(rest && shortCode){
				if(redOn){
					Board_LED_Set(0, false);
					redOn = false;
					vTaskDelay(configTICK_RATE_HZ /2);
				}
				rest = false;
				codeCnt = 0;
				Board_LED_Set(1, false);
				greenOn = (bool) !greenOn;
				vTaskDelay(configTICK_RATE_HZ * 2);
			}
			else{
				rest = true;
				codeCnt = 0;
				shortCode =(bool) !shortCode;
				vTaskDelay(configTICK_RATE_HZ / 2);
			}

		}
		else{
			Board_LED_Set(1,greenOn);
			if(redOn){
				Board_LED_Set(0, false);
				redOn = false;
				vTaskDelay(configTICK_RATE_HZ / 2);
			}
			Board_LED_Set(0, true);
			redOn = true;
			codeCnt += 1;
			if(shortCode){
				vTaskDelay(configTICK_RATE_HZ / 2);
			}
			else{
				vTaskDelay(configTICK_RATE_HZ * 2);
			}
		}
	}
}


static void vExercise3(void *pvParameters){
	DigitalIoPin pin = DigitalIoPin(0,17, DigitalIoPin::pullup);
	int tickCnt = 0;
	while (1){
		DEBUGOUT("Tick: %d \r\n",tickCnt);
		tickCnt++;
		if(!pin.read()){
			vTaskDelay(configTICK_RATE_HZ/10);
		}
		else{
			vTaskDelay(configTICK_RATE_HZ);
		}
	}
}



//SemaphoreHandle_t mutex = xSemaphoreCreateMutex();

//static void vSw1(void *pvParameters){
//	DigitalIoPin pin17 = DigitalIoPin(0,17, DigitalIoPin::pullup);
//	while(1){
//
//				while(!pin17.read()){
//					if(xSemaphoreTake( mutex, ( TickType_t ) INFINITY ) == pdTRUE )
//						{
//								DEBUGOUT("SW 1 pressed \r\n");
//								xSemaphoreGive( mutex );
//								vTaskDelay(configTICK_RATE_HZ/3);
//						}
//		}
//		}
//}
//static void vSw2(void *pvParameters){
//	DigitalIoPin pin11 = DigitalIoPin(1,11, DigitalIoPin::pullup);
//	while(1){
//
//			while(!pin11.read()){
//				if(xSemaphoreTake( mutex, ( TickType_t ) INFINITY ) == pdTRUE )
//					{
//							DEBUGOUT("SW 2 pressed \r\n");
//							xSemaphoreGive( mutex );
//							vTaskDelay(configTICK_RATE_HZ/3);
//					}
//	}
//	}
//
//}
//static void vSw3(void *pvParameters){
//	DigitalIoPin pin9 = DigitalIoPin(1, 9, DigitalIoPin::pullup);
//	while(1){
//
//				while(!pin9.read()){
//					if(xSemaphoreTake( mutex, ( TickType_t ) INFINITY ) == pdTRUE )
//						{
//								DEBUGOUT("SW 3 pressed \r\n");
//								xSemaphoreGive( mutex );
//								vTaskDelay(configTICK_RATE_HZ/3);
//						}
//		}
//		}
//
//}

SemaphoreHandle_t binarySemaphore = xSemaphoreCreateBinary();
bool hasInput = false;
static void vLab2Task1(void *pvParameters){
	int c;
	int count = 0;
	while(1){
		//DEBUGOUT("count is %d \r\n",count);
			c = Board_UARTGetChar();
			if (c != -1){
				hasInput = true;
				DEBUGOUT("c is %c \r\n", c);
				xSemaphoreGive(binarySemaphore);
			}
			else{
				hasInput = false;
				//DEBUGOUT("c is %c, no input \r\n", c);
			}
			vTaskDelay(2);
		}
}

static void vLab2Task2(void *pvParameters){
	bool redOn = false;
	while(1){
		if(!hasInput){
			continue;
		}
		else{
			if(xSemaphoreTake( binarySemaphore, ( TickType_t ) INFINITY ) == pdTRUE )
											{
						if(!redOn){
							DEBUGOUT("start blinking \r\n");
							Board_LED_Set(0, true);
							xSemaphoreGive( binarySemaphore );
							redOn = true;
							vTaskDelay(configTICK_RATE_HZ/10);
						}
						else{
							DEBUGOUT("end blinking \r\n");
							Board_LED_Set(0, false);
							redOn = false;
							hasInput = false;
						}

											}
		}

	}
}


DigitalIoPin::DigitalIoPin(int port_, int pin_, pinMode mode, bool invert) : port(port_), pin(pin_),
inv(invert) {
	if(mode == output) {
			Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, IOCON_MODE_INACT | IOCON_DIGMODE_EN);
			Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
}
	else {
		uint32_t pm = IOCON_DIGMODE_EN;
		if(invert) pm |= IOCON_INV_EN;
		if(mode == pullup) {
			pm |= IOCON_MODE_PULLUP;
		}
		else if(mode == pulldown) {
			pm |= IOCON_MODE_PULLDOWN;
		}
		Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, pm);
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
	}
}

DigitalIoPin::~DigitalIoPin() {
	// TODO Auto-generated destructor stub
}
bool DigitalIoPin::read() {
	return Chip_GPIO_GetPinState(LPC_GPIO, port, pin);
}
void DigitalIoPin::write(bool value) {
	return Chip_GPIO_SetPinState(LPC_GPIO, port, pin, inv ^ value);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* the following is required if runtime statistics are to be collected */
//extern "C" {
//
//void vConfigureTimerForRunTimeStats( void ) {
//	Chip_SCT_Init(LPC_SCTSMALL1);
//	LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
//	LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
//}
//
//}
///* end runtime statictics collection */
//
///**
// * @brief	main routine for FreeRTOS blinky example
// * @return	Nothing, function should not exit
// */
//
//
//int main(void)
//{
//
//	prvSetupHardware();
//	heap_monitor_setup();
//
//
//	xTaskCreate(vLab2Task1, "vLab2Task1",
//			configMINIMAL_STACK_SIZE+128, NULL, (tskIDLE_PRIORITY + 1UL),
//			(TaskHandle_t *) NULL);
//	xTaskCreate(vLab2Task2, "vLab2Task2",
//				configMINIMAL_STACK_SIZE+128, NULL, (tskIDLE_PRIORITY + 1UL),
//				(TaskHandle_t *) NULL);
//
//
//
//
//
//
//	/* Start the scheduler */
//	vTaskStartScheduler();
//
//	/* Should never arrive here */
//	return 1;
//}

