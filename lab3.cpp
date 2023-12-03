///*
// * lab3.cpp
// *
// *  Created on: 14 Nov 2023
// *      Author: luennam
// */
//
//#if defined (__USE_LPCOPEN)
//#if defined(NO_BOARD_LIB)
//#include "chip.h"
//#else
//#include "board.h"
//#endif
//#endif
//
//#include <cr_section_macros.h>
//
//// TODO: insert other include files here
//
//// TODO: insert other definitions and declarations here
//
//#include "FreeRTOS.h"
//#include "task.h"
//#include "heap_lock_monitor.h"
//#include "DigitalIoPin.h"
//#include <mutex>
//#include "Fmutex.h"
//#include "queue.h"
//#include "LpcUart.h"
//#include "ITM_output.h"
//#include "ITM_write.h"
//
//
//using namespace std;
//
//bool calculate = false;
//QueueHandle_t integerQueue;
//const int endCount = -1;
//const int helpMe = 112;
//
//struct debugEvent {
//	const char *format;
//	uint32_t data[3];
//};
//
///* Sets up system hardware */
//static void prvSetupHardware(void)
//{
//	SystemCoreClockUpdate();
//	Board_Init();
//	integerQueue = xQueueCreate(20, sizeof(int));
//}
//
///* Read character from serial port and reply thread */
//static void vLab3Ex1Task1(void *pvParameters) {
//	int count = 0;
//	int c;
//	while(1){
//			c = Board_UARTGetChar();
//			if (c != -1 && c != '\n' && c != '\r'){
//				count += 1;
//			}
//			else if (c == '\n' || c == '\r'){
//				if (xQueueSendToBack(integerQueue, (void*) &count, portMAX_DELAY) == pdTRUE){
//					DEBUGOUT("sent count to queue \r\n");
//					count = 0;
//				}
//			}
//		}
//}
//
///* Monitor pin thread */
//static void vLab3Ex1Task2(void *pvParameters) {
//		DigitalIoPin pin17 = DigitalIoPin(0,17, DigitalIoPin::pullup);
//		bool pressed = false;
//		while(1){
//			while(!pin17.read()){
//				if (pressed == false){
//					pressed = true;
//					xQueueSendToBack(integerQueue, (void*) &endCount, portMAX_DELAY);
//				}
//			}
//			pressed = false;
//		}
//}
//
///* Print total thread */
//static void vLab3Ex1Task3(void *pvParameters){
//	int total = 0;
//	int newCount;
//	while(1){
//		if(xQueueReceive(integerQueue, &newCount, portMAX_DELAY) == pdPASS) {
//		if (newCount == -1){
//			DEBUGOUT("You have typed %d characters \r\n", total);
//			total = 0;
//		}
//		else{
//			total += newCount;
//		}
//	}
//}
//}
//
//
///* send random int at random intervals thread */
//static void vLab3Ex2Task1(void *pvParameters){
//	int randInt;
//	while(1){
//		randInt = (100+(rand()%400));
//		if (xQueueSendToBack(integerQueue, (void*) &randInt, portMAX_DELAY) == pdTRUE){
//							vTaskDelay(randInt);
//						}
//	}
//}
//
///* send 112 thread */
//static void vLab3Ex2Task2(void *pvParameters){
//	DigitalIoPin pin17 = DigitalIoPin(0,17, DigitalIoPin::pullup);
//			while(1){
//				while(!pin17.read()){
//					if (xQueueSendToFront(integerQueue, (void*) &helpMe, portMAX_DELAY) == pdTRUE){
//						vTaskDelay(200);
//					}
//				}
//			}
//}
//
///* sleep and wait for more data thread */
//static void vLab3Ex2Task3(void *pvParameters){
//	int reqInt;
//	while(1){
//			if(xQueueReceive(integerQueue, &reqInt, portMAX_DELAY) == pdPASS) {
//			if (reqInt == 112){
//				DEBUGOUT("Help me \r\n");
//				vTaskDelay(800);
//			}
//			else{
//				DEBUGOUT("The integer is: %d \r\n",reqInt);
//			}
//		}
//	}
//}
//
//
//
//
//
//
//
//
///* the following is required if runtime statistics are to be collected */
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
//int main(void)
//{
//	prvSetupHardware();
//
//	ITM_init();
//
//	heap_monitor_setup();
//
//	/* Read character from serial port thread */
//	xTaskCreate(vLab3Ex1Task1, "vTask1",
//			configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
//			(TaskHandle_t *) NULL);
//
//	/* Monitor pin thread */
//	xTaskCreate(vLab3Ex1Task2, "vTask2",
//			configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
//			(TaskHandle_t *) NULL);
//
//	/* Print total thread */
//		xTaskCreate(vLab3Ex1Task3, "vTask3",
//				configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
//				(TaskHandle_t *) NULL);
//
//
////
////	/* send random int at random intervals thread */
////	xTaskCreate(vLab3Ex2Task1, "vTask1",
////					configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
////					(TaskHandle_t *) NULL);
////	/* send 112 thread */
////	xTaskCreate(vLab3Ex2Task2, "vTask2",
////							configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
////							(TaskHandle_t *) NULL);
////	/* sleep and wait for more data thread */
////	xTaskCreate(vLab3Ex2Task3, "vTask3",
////							configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
////							(TaskHandle_t *) NULL);
//
//
//	/* Start the scheduler */
//	vTaskStartScheduler();
//
//	/* Should never arrive here */
//	return 1;
//}
//
//
//
