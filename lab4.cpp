/*
 * lab4.cpp
 *
 *  Created on: 3 Dec 2023
 *      Author: luennam
 */

#include "board.h"
#include "chip.h"
#include <stdio.h>

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#if defined(BOARD_KEIL_MCB1500)||defined(BOARD_NXP_LPCXPRESSO_1549)
/* GPIO pin for PININT interrupt.  This is SW1-WAKE button switch input. */
#define SW1_INPUT_PIN          17	/* GPIO pin number mapped to PININT */
#define SW1_INPUT_PORT         0	/* GPIO port number mapped to PININT */
#define SW1_INPUT_PIN_PORT     0
#define SW1_INPUT_PIN_BIT      17

#define SW2_INPUT_PIN          11	/* GPIO pin number mapped to PININT */
#define SW2_INPUT_PIN_BIT      11

#define SW3_INPUT_PIN         	9	/* GPIO pin number mapped to PININT */
#define SW3_INPUT_PIN_BIT      9

#define PININT_INDEX_1   0	/* PININT index used for GPIO mapping */
#define PININT_INDEX_2   1	/* PININT index used for GPIO mapping */
#define PININT_INDEX_3   2	/* PININT index used for GPIO mapping */
#define PININT_IRQ_HANDLER1  PIN_INT0_IRQHandler	/* GPIO interrupt IRQ function name */
#define PININT_IRQ_HANDLER2 PIN_INT1_IRQHandler	/* GPIO interrupt IRQ function name */
#define PININT_IRQ_HANDLER3  PIN_INT2_IRQHandler	/* GPIO interrupt IRQ function name */
#define PININT_NVIC_NAME_1    PIN_INT0_IRQn	/* GPIO interrupt NVIC interrupt name */
#define PININT_NVIC_NAME_2    PIN_INT1_IRQn	/* GPIO interrupt NVIC interrupt name */
#define PININT_NVIC_NAME_3    PIN_INT2_IRQn	/* GPIO interrupt NVIC interrupt name */

#else
#error "PININT Interrupt not configured for this example"
#endif /* defined(BOARD_NXP_LPCXPRESSO_1549) */
QueueHandle_t pinIntQueue;
struct pinInt {
	IRQn_Type irq;
	long tickCount;
};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Handle interrupt from GPIO pin or GPIO pin mapped to PININT
 * @return	Nothing
 */
void PININT_IRQ_HANDLER1(void)
{
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_1));
	pinInt p;
	p.irq = PININT_NVIC_NAME_1;
	p.tickCount = xTaskGetTickCount();
	xQueueSendToBack(pinIntQueue, &p , portMAX_DELAY);
}

void PININT_IRQ_HANDLER2(void)
{
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_2));
	pinInt p;
	p.irq = PININT_NVIC_NAME_2;
	p.tickCount = xTaskGetTickCount();
	xQueueSendToBack(pinIntQueue, &p , portMAX_DELAY);

}

void PININT_IRQ_HANDLER3(void)
{
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_3));
	pinInt p;
	p.irq = PININT_NVIC_NAME_3;
	p.tickCount = xTaskGetTickCount();
	xQueueSendToBack(pinIntQueue, &p , portMAX_DELAY);

}

static void vLab4Ex1Task1(void *pvParameters){
	pinInt p;
	while(1){
		if(xQueueReceive(pinIntQueue, &p, portMAX_DELAY) == pdTRUE)
			{
				if(p.irq == PININT_NVIC_NAME_1){
					DEBUGOUT("1");
				}else if(p.irq == PININT_NVIC_NAME_2){
					DEBUGOUT("2");
				}else{
					DEBUGOUT("3");
				}
			}
	}
}

static void initEverything(void)
{
	SystemCoreClockUpdate();
		Board_Init();
		Board_LED_Set(0, false);
		pinIntQueue = xQueueCreate(10, sizeof(pinInt));

		/* Initialize PININT driver */
		Chip_PININT_Init(LPC_GPIO_PIN_INT);

		/* Set pin back to GPIO (on some boards may have been changed to something
		   else by Board_Init()) */
		Chip_IOCON_PinMuxSet(LPC_IOCON, SW1_INPUT_PIN_PORT, SW1_INPUT_PIN_BIT,
							 (IOCON_DIGMODE_EN | IOCON_MODE_INACT) );
		Chip_IOCON_PinMuxSet(LPC_IOCON, SW1_INPUT_PIN_PORT, SW2_INPUT_PIN_BIT,
								 (IOCON_DIGMODE_EN | IOCON_MODE_INACT) );
		Chip_IOCON_PinMuxSet(LPC_IOCON, SW1_INPUT_PIN_PORT, SW3_INPUT_PIN_BIT,
								 (IOCON_DIGMODE_EN | IOCON_MODE_INACT) );
		/* Configure GPIO pin as input */
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, SW1_INPUT_PORT, SW1_INPUT_PIN);
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, SW1_INPUT_PORT, SW2_INPUT_PIN);
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, SW1_INPUT_PORT, SW3_INPUT_PIN);

		/* Enable PININT clock */
		Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT);

		/* Reset the PININT block */
		Chip_SYSCTL_PeriphReset(RESET_PININT);

		/* Configure interrupt channel for the GPIO pin in INMUX block */
		Chip_INMUX_PinIntSel(PININT_INDEX_1, SW1_INPUT_PORT, SW1_INPUT_PIN);
		Chip_INMUX_PinIntSel(PININT_INDEX_2, SW1_INPUT_PORT, SW2_INPUT_PIN);
		Chip_INMUX_PinIntSel(PININT_INDEX_3, SW1_INPUT_PORT, SW3_INPUT_PIN);

		/* Configure channel interrupt as edge sensitive and falling edge interrupt */
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_1));
		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_1));
		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_1));

		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_2));
		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_2));
		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_2));

		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_3));
		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_3));
		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_3));

		/* Enable interrupt in the NVIC */
		NVIC_SetPriority(PININT_NVIC_NAME_1,1);
		NVIC_ClearPendingIRQ(PININT_NVIC_NAME_1);
		NVIC_EnableIRQ(PININT_NVIC_NAME_1);

		NVIC_SetPriority(PININT_NVIC_NAME_2,1);
		NVIC_ClearPendingIRQ(PININT_NVIC_NAME_2);
		NVIC_EnableIRQ(PININT_NVIC_NAME_2);

		NVIC_SetPriority(PININT_NVIC_NAME_3,1);
		NVIC_ClearPendingIRQ(PININT_NVIC_NAME_3);
		NVIC_EnableIRQ(PININT_NVIC_NAME_3);
}
/**
 * @brief	Main program body
 * @return	Does not return
 */
int main(void)
{


	initEverything();
	xTaskCreate(vLab4Ex1Task1, "vTask1",
			configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 2UL),
			(TaskHandle_t *) NULL);

	return 0;
}


