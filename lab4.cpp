/*
 * lab4.cpp
 *
 *  Created on: 3 Dec 2023
 *      Author: luennam
 */

#include "board.h"
#include "chip.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "heap_lock_monitor.h"
#include <mutex>
#include "Fmutex.h"
#include "queue.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#if defined(BOARD_KEIL_MCB1500)||defined(BOARD_NXP_LPCXPRESSO_1549)
/* GPIO pin for PININT interrupt.  This is SW1-WAKE button switch input. */


#else
#error "PININT Interrupt not configured for this example"
#endif /* defined(BOARD_NXP_LPCXPRESSO_1549) */


#define QUEUE_SIZE 10

using namespace std;

typedef struct {
    uint32_t pin;
    uint32_t tick;
} button_data_t;

static QueueHandle_t button_queue;
static uint32_t filter_time = 50; // default filter time in ms
static uint32_t last_tick = 0; // last tick

QueueHandle_t encoderQueue;
int encoderValue = 10;

extern "C" {
void PIN_INT0_IRQHandler(void) {
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    button_data_t data = {0};
    data.pin = 1;
    data.tick = xTaskGetTickCount();
    xQueueSendFromISR(button_queue, &data, &xHigherPriorityTaskWoken);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void PIN_INT1_IRQHandler(void) {
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    button_data_t data = {0};
    data.pin = 2;
    data.tick = xTaskGetTickCount();
    xQueueSendFromISR(button_queue, &data, &xHigherPriorityTaskWoken);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH1);
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void PIN_INT2_IRQHandler(void) {
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	button_data_t data = {0};
	data.pin = 3;
    data.tick = xTaskGetTickCount();
    xQueueSendFromISR(button_queue, &data, &xHigherPriorityTaskWoken);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH2);
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


void PIN_INT3_IRQHandler(void) {
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	int data = 0;
	static int prevStateA = 1;
	int currStateA = Chip_GPIO_GetPinState(LPC_GPIO, 0, 8);
    int currStateB = Chip_GPIO_GetPinState(LPC_GPIO, 1, 6);

    if(prevStateA > currStateA){
    	data = (currStateB==0)?1:-1; //falling edge, B=0,1;B=1,-1
    }else if (prevStateA < currStateA){
    	data = (currStateB==0)?-1:1; //rising edge, B=0,-1;B=1,1
    }else if (prevStateA==currStateA && currStateA == 1){
    	data = (currStateB==0)?1:-1; //same edge 1, B=0,1;B=1,-1
    }else if (prevStateA==currStateA && currStateA == 0){
    	data = (currStateB==0)?-1:1; //same edge 0, B=0,-1;B=1,1
    }

    prevStateA = currStateA;
    xQueueSendFromISR(encoderQueue, &data, &xHigherPriorityTaskWoken);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH3);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

}

static void button_task(void *pvParameters) {
    button_data_t data = {0};
    uint32_t elapsed_time = 0;
	NVIC_EnableIRQ(PIN_INT0_IRQn);
	NVIC_EnableIRQ(PIN_INT1_IRQn);
	NVIC_EnableIRQ(PIN_INT2_IRQn);
    while (1) {
        if (xQueueReceive(button_queue, &data, portMAX_DELAY) == pdTRUE) {
            if (data.tick - last_tick > filter_time) {
                elapsed_time = (data.tick - last_tick) * portTICK_PERIOD_MS;
                printf("Button %d pressed. Elapsed time since last press: %d ms\n", data.pin, elapsed_time);
                last_tick = data.tick;
            }
            else{
            	last_tick = data.tick;
            }
        }
    }
}

int getFilterTime(const string& userInput)
{
	const std::string prefix = "filter";
	    if (userInput.substr(0, prefix.size()) != prefix) {
	        return -1;
	    }
	    return atoi(userInput.substr(prefix.size()).c_str());

}

static void filter_task(void *pvParameters){
	int c;
	string userInput;
	int newFilterTime;
	while(1){
		c = Board_UARTGetChar();
		if (c == -1){
			continue;
		}
		else if (c == '\r' || c == '\n'){
			newFilterTime = getFilterTime(userInput);
			if (newFilterTime==-1){
				DEBUGOUT("Wrong input on changing filter time \r\n");
			} else if(newFilterTime>=0){
				DEBUGOUT("New filter time is %d \r\n", newFilterTime);
				filter_time = newFilterTime;
			}
			userInput.clear();
			}
		else{
			userInput.push_back(c);
					}
	}
}




static void encoderTask(void *pvParameters){
	int data = 0;
	int encoderInt = 10;
	NVIC_EnableIRQ(PIN_INT3_IRQn);
	while (1) {
	        if (xQueueReceive(encoderQueue, &data, pdMS_TO_TICKS(5000)) == pdTRUE) {
	        	DEBUGOUT("Variable: %d \r\n", encoderInt);
	            encoderInt += data;
	        } else{
	        	DEBUGOUT("Timeout with value: %d \r\n", encoderInt);
	        	encoderInt = 10;
	        }
	    }
}

int main(void) {
    SystemCoreClockUpdate();
    heap_monitor_setup();
    Board_Init();
    encoderQueue = xQueueCreate(5, sizeof(int));

    Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 8); //SIG A
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, 1, 6); //SIG B

    Chip_PININT_Init(LPC_GPIO_PIN_INT);

    //Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 8, IOCON_MODE_PULLUP | IOCON_DIGMODE_EN);

	Chip_INMUX_PinIntSel(3, 0, 8);

	NVIC_ClearPendingIRQ(PIN_INT3_IRQn);
    NVIC_SetPriority(PIN_INT3_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);

    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH3);
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH3);
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH3);

    xTaskCreate(encoderTask, "Encoder Task", configMINIMAL_STACK_SIZE + 256, NULL, tskIDLE_PRIORITY + 1, NULL);
    vTaskStartScheduler();

//    button_queue = xQueueCreate(QUEUE_SIZE, sizeof(button_data_t));
//    if (button_queue == NULL) {
//        printf("Failed to create button queue!\n");
//        return 1;
//    }
//    Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 17); // SW1
//    Chip_GPIO_SetPinDIRInput(LPC_GPIO, 1, 11); // SW2
//    Chip_GPIO_SetPinDIRInput(LPC_GPIO, 1, 9); // SW3
//
//    Chip_PININT_Init(LPC_GPIO_PIN_INT);
//
//    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 17, IOCON_MODE_PULLUP | IOCON_DIGMODE_EN);
//    Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 11, IOCON_MODE_PULLUP | IOCON_DIGMODE_EN);
//    Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 9, IOCON_MODE_PULLUP | IOCON_DIGMODE_EN);
//
//	Chip_INMUX_PinIntSel(0, 0, 17);
//	Chip_INMUX_PinIntSel(1, 1, 11);
//	Chip_INMUX_PinIntSel(2, 1, 9);
//
//	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
//	NVIC_ClearPendingIRQ(PIN_INT1_IRQn);
//	NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
//
//    NVIC_SetPriority(PIN_INT0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
//    NVIC_SetPriority(PIN_INT1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
//    NVIC_SetPriority(PIN_INT2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
//
//    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
//    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH0);
//    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH0);
//
//    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH1);
//    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH1);
//    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH1);
//
//    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH2);
//    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH2);
//    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH2);
//
//
//    xTaskCreate(button_task, "Button Task", configMINIMAL_STACK_SIZE + 256, NULL, tskIDLE_PRIORITY + 1, NULL);
//    xTaskCreate(filter_task, "Filter Task", configMINIMAL_STACK_SIZE + 256, NULL, tskIDLE_PRIORITY + 1, NULL);
//    vTaskStartScheduler();
    return 0;
}
