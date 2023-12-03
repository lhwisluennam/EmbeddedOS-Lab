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
//using namespace std;
//
//QueueHandle_t debugEventQueue;
//Fmutex guard;
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
//	debugEventQueue = xQueueCreate(10, sizeof(debugEvent));
//}
//
//
//void debug(const char *format, uint32_t d1, uint32_t d2, uint32_t d3){
//	debugEvent e;
//	e.format = format;
//	e.data[0] = d1;
//	e.data[1] = d2;
//	e.data[2] = d3;
//	xQueueSendToBack(debugEventQueue, &e, portMAX_DELAY);
//}
//void dbguPrint(LpcUart &dbgu,string &str, int N)
//{
//	// Traverse the string
//	for (int i = 0; i < N; i++) {
//
//		// Print current character
//		dbgu.write(str[i]);
//	}
//	dbgu.write('\r\n');
//}
//
//static void vLab3Ex3Task1(void *pvParameters){
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
//	while(1){
//		readResult = dbgu.read(c);
//		if(readResult == 1){
//			if(c != EOF) {
//				if(c == '\n' || c == '\r' || c == ' ' || c == '\t'){
//					{
//						dbguPrint(dbgu, userInput, userInput.length());
//						vTaskDelay(100);
//						debug("The word count is %d, the timestamp is %lu \r\n",userInput.length(),xTaskGetTickCount(),0);
//					}
//					userInput.clear();
//				}
//				else{
//					userInput.push_back(c);		//store the userInput char in a string
//				}
//				if(c == '\r') dbgu.write('\n'); // send line feed after carriage return
//			}
//			vTaskDelay(1);
//		}
//		else{
//			vTaskDelay(10);
//		}
//
//	}
//}
//static void vLab3Ex3Task2(void *pvParameters){
//	int sec = -1;
//	bool pressed = false;
//	DigitalIoPin pin17 = DigitalIoPin(0,17, DigitalIoPin::pullup);
//	while(1){
//		while(!pin17.read()){
//			pressed = true;
//			sec += 1;
//			vTaskDelay(1000);
//		}
//		if(pressed == true){
//			debug("The number of seconds s1 being pressed is %d, the timestamp is %lu \r\n",sec,xTaskGetTickCount(),0);
//			sec = -1;
//			pressed = false;
//		}
//		vTaskDelay(10);
//
//	}
//}
//static void vLab3Ex3Task3(void *pvParameters){char buffer[64];
//debugEvent e;
//ITM_output itm;
//// this is not complete! how do we know which queue to wait on?
//while (1) {
//	// read queue
//	if(xQueueReceive(debugEventQueue, &e, portMAX_DELAY) == pdTRUE)
//	{
//		snprintf(buffer, 64, e.format, e.data[0], e.data[1], e.data[2]);
//		ITM_write(buffer);
//		itm.print('\n');
//	}
//}
//}
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
//	xTaskCreate(vLab3Ex3Task1, "vTask1",
//			configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 2UL),
//			(TaskHandle_t *) NULL);	/* sleep and wait for more data thread */
//	xTaskCreate(vLab3Ex3Task2, "vTask2",
//			configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 2UL),
//			(TaskHandle_t *) NULL);	/* sleep and wait for more data thread */
//	xTaskCreate(vLab3Ex3Task3, "vTask3",
//			configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
//			(TaskHandle_t *) NULL);
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
