///*
//===============================================================================
// Name        : main.c
// Author      : $(author)
// Version     :
// Copyright   : $(copyright)
// Description : main definition
//===============================================================================
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
//#include "LpcUart.h"
//#include "ITM_output.h"
//#include "ITM_write.h"
//
//using namespace std;
//
//Fmutex guard;
//SemaphoreHandle_t responseSemaphore;
//QueueHandle_t questionQueue;
//
///* Sets up system hardware */
//static void prvSetupHardware(void)
//{
//	SystemCoreClockUpdate();
//	Board_Init();
//	responseSemaphore = xSemaphoreCreateCounting(3, 0);
//	questionQueue = xQueueCreate(5, sizeof(SemaphoreHandle_t));
//}
//
///* Read character from serial port and reply thread */
//static void vLab2Ex3Task1(void *pvParameters) {
//	LpcPinMap none = { .port = -1, .pin = -1}; // unused pin has negative values in it
//	LpcPinMap txpin = { .port = 0, .pin = 18 }; // transmit pin that goes to debugger's UART->USB converter
//	LpcPinMap rxpin = { .port = 0, .pin = 13 }; // receive pin that goes to debugger's UART->USB converter
//	LpcUartConfig cfg = {
//			.pUART = LPC_USART0,
//			.speed = 115200,
//			.data = UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1,
//			.rs485 = false,
//			.tx = txpin,
//			.rx = rxpin,
//			.rts = none,
//			.cts = none
//	};
//	LpcUart dbgu(cfg);
//	char c;
//	int readResult;
//	string userInput;
//	ITM_output itm;
//	while(1){
//		readResult = dbgu.read(c);
//
//		if(readResult == 1){
//			if(c != EOF) {
//				if(c == '\n') dbgu.write('\r');
//				if(c == '\n' || c == '\r' || userInput.length() == 60){
//					userInput.insert(0, "[You] ");
//					{
//						lock_guard<Fmutex> lock(guard);
//						itm.print(userInput);
//						itm.print("\n");
//					}
//					if(userInput.find("?") != std::string::npos){
//						if (xQueueSendToBack(questionQueue, &responseSemaphore, portMAX_DELAY) == pdTRUE) {
//							xSemaphoreGive(responseSemaphore);
//						}
//					}
//					userInput.clear();
//				}
//				else{
//					userInput.push_back(c);		//store the userInput char in a string
//				}
//				if(c == '\r') dbgu.write('\n'); // send line feed after carriage return
//			}
//		}
//
//	}
//
//}
//
//const char* getRandomAnswer() {
//		const char* randomAnswers[] = {
//				"I don't know",
//				"Please google it",
//				"I am not Chatgpt",
//				"Just do it",
//				"Don't overthink",
//				"Not today",
//				"Okay, then..?",
//				"Can you repeat?",
//		};
//
//	int randomIndex = rand() % 8;
//
//	return randomAnswers[randomIndex];
//}
//
///* Oracle answers thread */
//static void vLab2Ex3Task2(void *pvParameters) {
//	const char *answer = "";
//	ITM_output itm;
//    SemaphoreHandle_t questionSemaphore;
//
//	while(1){
//        if (xQueueReceive(questionQueue, &questionSemaphore, portMAX_DELAY) == pdPASS) {
//        	{
//        		lock_guard<Fmutex> lock(guard);
//        		itm.print("[Oracle] Hmmm...\n");
//        	}
//
//        	vTaskDelay(3000); // think for 3 seconds
//
//			answer = getRandomAnswer();
//
//			{
//			    lock_guard<Fmutex> lock(guard);
//			    itm.print("[Oracle] ");
//				itm.print(answer);
//				itm.print("\n");
//			}
//
//			vTaskDelay(2000);	// wait 2 second before answering next question
//		}
//	}
//
//}
//
//
///* the following is required if runtime statistics are to be collected */
////extern "C" {
////
////void vConfigureTimerForRunTimeStats( void ) {
////	Chip_SCT_Init(LPC_SCTSMALL1);
////	LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
////	LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
////}
////
////}
///* end runtime statictics collection */
//
///**
// * @brief	main routine for FreeRTOS blinky example
// * @return	Nothing, function should not exit
// */
////int main(void)
////{
////	prvSetupHardware();
////
////	ITM_init();
////
////	heap_monitor_setup();
////
////	/* Read character from serial port thread */
////	xTaskCreate(vLab2Ex3Task1, "vTask1",
////			configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
////			(TaskHandle_t *) NULL);
////
////	/* Oracle answers thread */
////	xTaskCreate(vLab2Ex3Task2, "vTask2",
////			configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
////			(TaskHandle_t *) NULL);
////
////
////	/* Start the scheduler */
////	vTaskStartScheduler();
////
////	/* Should never arrive here */
////	return 1;
////}
//
