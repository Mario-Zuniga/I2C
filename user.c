/*
 *  ABOUT
 *
 * 	Date:		Marzo del 2017
 *
 * 	Authors:	MARISCAL ARROYO JUAN PABLO
 * 				MORENO ALONZO LUIS FERNANDO
 * 				ZUÑIGA CARRILLO MARIO EUGENIO
 *
 * 	Description: Práctica #1
 *
 */

/*******************************************************************************
 * LIBRARIES
 ******************************************************************************/
#include "Definitions.h"

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/
/* Semaphore Handle Variables */
SemaphoreHandle_t xMenu_SemaphoreHandler;
SemaphoreHandle_t xMenu_BT_SemaphoreHandler;

/* Queue Handle Variables */
uint8_t PC_keyPressed[N_LETTERS];
uint8_t BT_keyPressed[N_LETTERS];

uint8_t botton1_pressed = FALSE;
uint8_t configuringButtons = FALSE;
uint8_t configuring = DAYS;
uint8_t Someone_exit = FALSE;
size_t n;
uint8_t format24h;
uint8_t formatAM;

uint8_t Task_ONE = FREE;
uint8_t Task_TWO = FREE;
uint8_t Task_THREE = FREE;
uint8_t Task_FOUR = FREE;
uint8_t Task_FIVE = FREE;
uint8_t Task_SIX = FREE;
uint8_t Task_SEVEN = FREE;
uint8_t Task_EIGHT = FREE;
uint8_t Task_NINE = FREE;

uint8_t Terminal_ONE;
uint8_t Terminal_TWO;
uint8_t Terminal_THREE;
uint8_t Terminal_FOUR;
uint8_t Terminal_FIVE;
uint8_t Terminal_SIX;
uint8_t Terminal_SEVEN;
uint8_t Terminal_EIGHT;
uint8_t Terminal_NINE;

/*******************************************************************************
 * GLOBAL STRING VARIABLES
 ******************************************************************************/
uint8_t PC_printed = FALSE;
uint8_t BT_printed = FALSE;

uint8_t *requestString = "\r\n\r\nIngrese los datos pedidos: ";
uint8_t *readMemory_string =
		"\r\nIngrese la direccion de lectura y la longitud en bytes \r\ncon el siguiente formato: '0xABCD 50' (sin comillas)";
uint8_t *writeMemory_string =
		"\r\nIngrese la direccion de escritura y la informacion a escribir \r\ncon el siguiente formato: '0xABCD informacion' (sin comillas)";
uint8_t *setHour_string_24h =
		"\r\nIngrese la hora con el siguiente formato: hh:mm:ss";
uint8_t *setHour_string_12h =
		"\r\nIngrese la hora con el siguiente formato: hh:mm:ss AM/PM";
uint8_t *setDate_string =
		"\r\nIngrese la fecha con el siguiente formato: dd/mm/yy";

/* FORMATO */
uint8_t *Format12h_string_1 = "\r\nEl formato actual es 12h.";
uint8_t *Format12h_string_2 = "\r\n\r\nDesea cambiar el formato a 24h? si/no: ";
uint8_t *Format24h_string_1 = "\r\nEl formato actual es 24h.";
uint8_t *Format24h_string_2 = "\r\n\r\nDesea cambiar el formato a 12h? si/no: ";

/* LCD   */
uint8_t *Fecha_LCD = "Fecha:";
uint8_t *Hour_LCD = "Hora:";

/* CHAT */
uint8_t terminal1[] = "Terminal 1: ";
uint8_t terminal2[] = "Terminal 2: ";
uint8_t terminal1_exit[] = "\r\n\r\nLa terminal 1 se desconecto.";
uint8_t terminal2_exit[] = "\r\n\r\nLa terminal 2 se desconecto.";

/* COMMANDS */
uint8_t clear_terminal[] = "\033[2J";
uint8_t cursor_to_home[] = "\033[H";
uint8_t cursor_to_time[] = "\033[3;25H";
uint8_t blueText[] = "\033[1;34;40m";
uint8_t whiteText[] = "\033[1;37;40m";
uint8_t yellowText[] = "\033[1;33;40m";
uint8_t redText[] = "\033[1;31;40m";

/*******************************************************************************
 * IRQ HANDLERS
 ******************************************************************************/
void PORTC_IRQHandler(void) {
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	uint32_t botton_pressed = GPIO_GetPinsInterruptFlags(
			BOARD_BOTTONS_GPIO_MUX);
	delay_msOrus(200, 120000000, 0);

	switch (botton_pressed) {
	case BOTON1:
		GPIO_ClearPinsInterruptFlags(BOARD_BOTTONS_GPIO_MUX,
				1U << BOARD_BOTTON1_GPIO_PIN_MUX);
		botton1_pressed = TRUE;
		xSemaphoreGiveFromISR(getBotton1Sem(), &xHigherPriorityTaskWoken);
		break;
	case BOTON2:
		GPIO_ClearPinsInterruptFlags(BOARD_BOTTONS_GPIO_MUX,
				1U << BOARD_BOTTON2_GPIO_PIN_MUX);
		xSemaphoreGiveFromISR(getBotton2Sem(), &xHigherPriorityTaskWoken);
		break;
	case BOTON3:
		GPIO_ClearPinsInterruptFlags(BOARD_BOTTONS_GPIO_MUX,
				1U << BOARD_BOTTON3_GPIO_PIN_MUX);
		xSemaphoreGiveFromISR(getBotton3Sem(), &xHigherPriorityTaskWoken);
		break;
	case BOTON4:
		GPIO_ClearPinsInterruptFlags(BOARD_BOTTONS_GPIO_MUX,
				1U << BOARD_BOTTON4_GPIO_PIN_MUX);
		xSemaphoreGiveFromISR(getBotton4Sem(), &xHigherPriorityTaskWoken);
		break;

	default:
		GPIO_ClearPinsInterruptFlags(BOARD_BOTTONS_GPIO_MUX,
				1U << BOARD_BOTTON1_GPIO_PIN_MUX);
		GPIO_ClearPinsInterruptFlags(BOARD_BOTTONS_GPIO_MUX,
				1U << BOARD_BOTTON2_GPIO_PIN_MUX);
		GPIO_ClearPinsInterruptFlags(BOARD_BOTTONS_GPIO_MUX,
				1U << BOARD_BOTTON3_GPIO_PIN_MUX);
		break;
	}
}

/*******************************************************************************
 * TASKS
 ******************************************************************************/

void PC_Menu_vTask(void *pvParameter) {

	QueuesAndSem_Init();
	RTC_Hour_Init();
	xSemaphoreGive(xMenu_SemaphoreHandler);

	for (;;) {
		if (pdTRUE == xSemaphoreTake(xMenu_SemaphoreHandler, SEM_WAITTIME)) {
			if (FALSE == PC_printed) {
				print_Menu();
				PC_printed = TRUE;
			}

			UARTPC_waitingKey(PC_keyPressed);

			switch (*PC_keyPressed) {
			case ONE:
				if (FREE == Task_ONE) {
					Task_ONE = BUSY;
					Terminal_ONE = PC;
					vTaskResume(getReadMemoryTask_Handler());
				} else {
					print_PC_ERROR();
					xSemaphoreGive(xMenu_SemaphoreHandler);

				}
				break;
			case TWO:
				if (FREE == Task_TWO) {
					Task_TWO = BUSY;
					Terminal_TWO = PC;
					vTaskResume(getWriteMemoryTask_Handler());
				} else {
					print_PC_ERROR();
					xSemaphoreGive(xMenu_SemaphoreHandler);
				}

				break;
			case THREE:
				if (FREE == Task_THREE) {
					Task_THREE = BUSY;
					Terminal_THREE = PC;
					vTaskResume(getSetHourTask_Handler());
				} else {
					print_PC_ERROR();
					xSemaphoreGive(xMenu_SemaphoreHandler);
				}
				break;
			case FOUR:
				if (FREE == Task_FOUR) {
					Task_FOUR = BUSY;
					Terminal_FOUR = PC;
					vTaskResume(getSetDateTask_Handler());
				} else {
					print_PC_ERROR();
					xSemaphoreGive(xMenu_SemaphoreHandler);
				}
				break;
			case FIVE:
				if (FREE == Task_FIVE) {
					Task_FIVE = BUSY;
					Terminal_FIVE = PC;
					vTaskResume(getHourFormatTask_Handler());
				} else {
					print_PC_ERROR();
					xSemaphoreGive(xMenu_SemaphoreHandler);
				}
				break;
			case SIX:
				if (FREE == Task_SIX) {
					Task_SIX = BUSY;
					Terminal_SIX = PC;
				} else {
					print_PC_ERROR();
					xSemaphoreGive(xMenu_SemaphoreHandler);
				}
				vTaskResume(getGetHourTask_Handler());
				break;
			case SEVEN:
				if (FREE == Task_SEVEN) {
					Task_SEVEN = BUSY;
					Terminal_SEVEN = PC;
					vTaskResume(getGetDateTask_Handler());
				} else {
					print_PC_ERROR();
					xSemaphoreGive(xMenu_SemaphoreHandler);
				}
				break;
			case EIGHT:	vTaskResume(getUART0CHATTask_Handler());
						break;
			case NINE:if (FREE == Task_NINE) {
							vTaskSuspend(getPrintLCDTask_Handler());
							Task_NINE = BUSY;
							Terminal_NINE = PC;
							vTaskResume(getECOTask_Handler());
						} else {
							print_PC_ERROR();
							xSemaphoreGive(xMenu_SemaphoreHandler);
						}
				break;

			default:
				PC_printed = FALSE;
				xSemaphoreGive(xMenu_SemaphoreHandler);
				break;
			}
		}
		taskYIELD();
	}

}


void BT_Menu_vTask(void *pvParameter) {
	xSemaphoreGive(xMenu_BT_SemaphoreHandler);

	for (;;) {
		if (pdTRUE == xSemaphoreTake(xMenu_BT_SemaphoreHandler, SEM_WAITTIME)) {
			if (FALSE == BT_printed) {
				print_Menu_BT();
				BT_printed = TRUE;
			}

			UARTBT_waitingKey(BT_keyPressed);

			switch (*BT_keyPressed) {
			case ONE:
				if (FREE == Task_ONE) {
					Task_ONE = BUSY;
					Terminal_ONE = BT;
					vTaskResume(getReadMemoryTask_Handler());
				} else {
					print_BT_ERROR();
					xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				}
				break;
			case TWO:
				if (FREE == Task_TWO) {
					Task_TWO = BUSY;
					Terminal_TWO = BT;
					vTaskResume(getWriteMemoryTask_Handler());
				} else {
					print_BT_ERROR();
					xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				}
				break;
			case THREE:
				if (FREE == Task_THREE) {
					Task_THREE = BUSY;
					Terminal_THREE = BT;
					vTaskResume(getSetHourTask_Handler());
				} else {
					print_BT_ERROR();
					xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				}
				break;
			case FOUR:
				if (FREE == Task_FOUR) {
					Task_FOUR = BUSY;
					Terminal_FOUR = BT;
					vTaskResume(getSetDateTask_Handler());
				} else {
					print_BT_ERROR();
					xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				}
				break;
			case FIVE:
				if (FREE == Task_FIVE) {
					Task_FIVE = BUSY;
					Terminal_FIVE = BT;
					vTaskResume(getHourFormatTask_Handler());
				} else {
					print_BT_ERROR();
					xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				}
				break;
			case SIX:
				if (FREE == Task_SIX) {
					Task_SIX = BUSY;
					Terminal_SIX = BT;
					vTaskResume(getGetHourTask_Handler());
				} else {
					print_BT_ERROR();
					xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				}
				break;
			case SEVEN:
				if (FREE == Task_SEVEN) {
					Task_SEVEN = BUSY;
					Terminal_SEVEN = BT;
					vTaskResume(getGetDateTask_Handler());
				} else {
					print_BT_ERROR();
					xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				}
				break;
			case EIGHT:	vTaskResume(getUART3CHATTask_Handler());
						break;
			case NINE: if (FREE == Task_NINE) {
							vTaskSuspend(getPrintLCDTask_Handler());
							Task_NINE = FALSE;
							Terminal_NINE = BT;
							vTaskResume(getECOTask_Handler());
						} else {
							print_BT_ERROR();
							xSemaphoreGive(xMenu_BT_SemaphoreHandler);
						}
				break;

			default:
				xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				BT_printed = FALSE;
				break;
			}
		}
		taskYIELD()
		;
	}
}


void Read_Memory_Task(void *pvParameter) {
	vTaskSuspend(NULL);

	uint8_t UART0_Buffer[SMALL_BUFFER_SIZE];
	uint8_t UART3_Buffer[SMALL_BUFFER_SIZE];
	uint8_t readBuff[BIG_BUFFER_SIZE];
	zerosBuffer(UART0_Buffer, SMALL_BUFFER_SIZE);
	zerosBuffer(UART3_Buffer, SMALL_BUFFER_SIZE);
	zerosBuffer(readBuff, UART_QUEUE_SIZE);

	uint8_t queueSize;
	uint8_t *stringPtr;
	uint16_t mem_address;
	uint8_t mem_size;
	uint8_t reset = TRUE;
	uint8_t SB = 2;

	for (;;) {
		if (BT == Terminal_ONE) {
			if (reset) {
				UARTBT_Printf_Hex(cursor_to_home, NULL);
				UARTBT_Printf_Hex(clear_terminal, NULL);
				UARTBT_Printf_Hex(whiteText, NULL);
				UARTBT_Printf_Hex(readMemory_string, NULL);
				UARTBT_Printf_Hex(blueText, NULL);
				UARTBT_Printf_Hex(requestString, NULL);
				reset = FALSE;
			}

			UARTBT_waitingKey(BT_keyPressed);
			if ('\e' != *BT_keyPressed && '\r' != *BT_keyPressed && 0x1 != *PC_keyPressed) {
				xQueueSend(getUART3Queue(), (void * )&BT_keyPressed,
						QUEUE_WAITTIME);
			}
			UARTBT_Printf_Key(BT_keyPressed);

			if (*BT_keyPressed == ENTER_KEY) {
				queueSize = uxQueueMessagesWaiting(getUART3Queue());
				for (uint8_t i = 0; i < queueSize; i++) {
					xQueueReceive(getUART3Queue(), &UART3_Buffer[i],
							QUEUE_WAITTIME);
				}

				UART3_Buffer[SB] = UART3_Buffer[SB] - 0x30;
				UART3_Buffer[SB + 1] = UART3_Buffer[SB + 1] - 0x30;
				UART3_Buffer[SB + 2] = UART3_Buffer[SB + 2] - 0x30;
				UART3_Buffer[SB + 3] = UART3_Buffer[SB + 3] - 0x30;
				mem_address = (UART3_Buffer[SB] << 12)
						| (UART3_Buffer[SB + 1] << 8)
						| (UART3_Buffer[SB + 2] << 4) | UART3_Buffer[SB + 3];

				UART3_Buffer[SB + 5] = UART3_Buffer[SB + 5] - 0x30;
				UART3_Buffer[SB + 6] = UART3_Buffer[SB + 6] - 0x30;
				mem_size = (UART3_Buffer[SB + 5] << 4) | UART3_Buffer[SB + 6];
				mem_size = BCDtoHex(mem_size);
				xSemaphoreTake(getMutex(),pMAX_DELAY);
				{
				I2C_ReadReg(BOARD_I2C0_BASEADDR, MEMORY_ADDRESS, mem_address,
						readBuff, mem_size);
				}
				xSemaphoreGive(getMutex());

				UARTBT_Printf_Hex(whiteText, NULL);
				UARTBT_Printf_String("\r\n\r\nContenido:	", readBuff);
				UARTBT_Printf_ESC();
				zerosBuffer(UART3_Buffer, SMALL_BUFFER_SIZE);
				zerosBuffer(readBuff, BIG_BUFFER_SIZE);
				xQueueReset(getUART3Queue());
			}
			if (ESC == *BT_keyPressed) {
				BT_printed = FALSE;
				reset = TRUE;
				Task_ONE = FREE;
				xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				vTaskSuspend(NULL);
			}
		}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else {
			if (reset) {
				UARTPC_Printf_Hex(cursor_to_home, NULL);
				UARTPC_Printf_Hex(clear_terminal, NULL);
				UARTPC_Printf_Hex(whiteText, NULL);
				UARTPC_Printf_Hex(readMemory_string, NULL);
				UARTPC_Printf_Hex(blueText, NULL);
				UARTPC_Printf_Hex(requestString, NULL);
				reset = FALSE;
			}

			UARTPC_waitingKey(PC_keyPressed);
			if ('\e' != *PC_keyPressed && '\r' != *PC_keyPressed) {
				xQueueSend(getUART0Queue(), (void * )&PC_keyPressed,
						QUEUE_WAITTIME);
			}
			UARTPC_Printf_Key(PC_keyPressed);

			if (ENTER_KEY == *PC_keyPressed) {
				queueSize = uxQueueMessagesWaiting(getUART0Queue());
				for (uint8_t i = 0; i < queueSize; i++) {
					xQueueReceive(getUART0Queue(), &UART0_Buffer[i],
							QUEUE_WAITTIME);
				}

				UART0_Buffer[SB] = UART0_Buffer[SB] - 0x30;
				UART0_Buffer[SB + 1] = UART0_Buffer[SB + 1] - 0x30;
				UART0_Buffer[SB + 2] = UART0_Buffer[SB + 2] - 0x30;
				UART0_Buffer[SB + 3] = UART0_Buffer[SB + 3] - 0x30;
				mem_address = (UART0_Buffer[SB] << 12)
						| (UART0_Buffer[SB + 1] << 8)
						| (UART0_Buffer[SB + 2] << 4) | UART0_Buffer[SB + 3];

				UART0_Buffer[SB + 5] = UART0_Buffer[SB + 5] - 0x30;
				UART0_Buffer[SB + 6] = UART0_Buffer[SB + 6] - 0x30;
				mem_size = (UART0_Buffer[SB + 5] << 4) | UART0_Buffer[SB + 6];
				mem_size = BCDtoHex(mem_size);
				xSemaphoreTake(getMutex(),pMAX_DELAY);
				{
				I2C_ReadReg(BOARD_I2C0_BASEADDR, MEMORY_ADDRESS, mem_address,
						readBuff, mem_size);
				}
				xSemaphoreGive(getMutex());

				UARTPC_Printf_Hex(whiteText, NULL);
				UARTPC_Printf_String("\r\n\r\nContenido:	", readBuff);
				UARTPC_Printf_ESC();
				zerosBuffer(UART0_Buffer, SMALL_BUFFER_SIZE);
				zerosBuffer(readBuff, BIG_BUFFER_SIZE);
				xQueueReset(getUART0Queue());
			}
			if (ESC == *PC_keyPressed) {
				PC_printed = FALSE;
				reset = TRUE;
				Task_ONE = FREE;
				xSemaphoreGive(xMenu_SemaphoreHandler);
				vTaskSuspend(NULL);
			}
		}
	}
}


void Write_Memory_Task(void *pvParameter) {
	vTaskSuspend(NULL);

	uint8_t UART0_Buffer[BIG_BUFFER_SIZE];
	uint8_t UART3_Buffer[BIG_BUFFER_SIZE];
	uint8_t INFO_Buffer[INFO_BUFFER_SIZE];

	zerosBuffer(UART0_Buffer, BIG_BUFFER_SIZE);
	zerosBuffer(UART3_Buffer, BIG_BUFFER_SIZE);
	zerosBuffer(INFO_Buffer, INFO_BUFFER_SIZE);

	uint8_t queueSize;
	uint8_t *stringPtr;
	uint16_t mem_address;
	uint8_t mem_size;
	uint8_t info_size;
	uint8_t reset = TRUE;
	const uint8_t SB = 2;
	const uint8_t OFFSET = 7;

	for (;;) {
		if (BT == Terminal_TWO) {
			if (reset) {
				UARTBT_Printf_Hex(cursor_to_home, NULL);
				UARTBT_Printf_Hex(clear_terminal, NULL);
				UARTBT_Printf_Hex(whiteText, NULL);
				UARTBT_Printf_Hex(writeMemory_string, NULL);
				UARTBT_Printf_Hex(blueText, NULL);
				UARTBT_Printf_Hex(requestString, NULL);
				reset = FALSE;
			}

			UARTBT_waitingKey(BT_keyPressed);
			if ('\e' != *BT_keyPressed && '\r' != *BT_keyPressed && 0x1 != *PC_keyPressed) {
				xQueueSend(getUART3Queue(), (void * )&BT_keyPressed,
						QUEUE_WAITTIME);
			}
			UARTBT_Printf_Key(BT_keyPressed);

			if (*BT_keyPressed == ENTER_KEY) {
				queueSize = uxQueueMessagesWaiting(getUART3Queue());
				for (uint8_t i = 0; i < queueSize; i++) {
					xQueueReceive(getUART3Queue(), &UART3_Buffer[i],
							QUEUE_WAITTIME);
				}

				UART3_Buffer[SB] = UART3_Buffer[SB] - 0x30;
				UART3_Buffer[SB + 1] = UART3_Buffer[SB + 1] - 0x30;
				UART3_Buffer[SB + 2] = UART3_Buffer[SB + 2] - 0x30;
				UART3_Buffer[SB + 3] = UART3_Buffer[SB + 3] - 0x30;
				mem_address = (UART3_Buffer[SB] << 12)
						| (UART3_Buffer[SB + 1] << 8)
						| (UART3_Buffer[SB + 2] << 4) | UART3_Buffer[SB + 3];

				for (uint8_t i = 0; UART3_Buffer[i + OFFSET] != '\0'; i++) {
					info_size = i;
				}
				if(0x1 != UART3_Buffer[info_size+OFFSET]){
					info_size++;
				}
				for (uint8_t i = 0; i < info_size; i++) {
					INFO_Buffer[i] = UART3_Buffer[i + OFFSET];
				}
				stringPtr = INFO_Buffer;
				xSemaphoreTake(getMutex(),pMAX_DELAY);
				{
				I2C_WriteReg(BOARD_I2C0_BASEADDR, MEMORY_ADDRESS, mem_address,
						stringPtr);
				}
				xSemaphoreGive(getMutex());

				UARTBT_Printf_Hex(whiteText, NULL);
				zerosBuffer(UART3_Buffer, UART_QUEUE_SIZE);
				zerosBuffer(INFO_Buffer, INFO_BUFFER_SIZE);
				xQueueReset(getUART3Queue());
				info_size = HexToBCD(info_size);
				UARTBT_Printf_Hex(
						"\r\n\r\nSu texto ha sido guardado. Se escribieron: ",
						&info_size);
				UARTBT_Printf_String(" bytes.", NULL);
				UARTBT_Printf_ESC();
			}

			if (ESC == *BT_keyPressed) {
				BT_printed = FALSE;
				reset = TRUE;
				Task_TWO = FREE;
				xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				vTaskSuspend(NULL);
			}
		}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else {
			if (reset) {
				UARTPC_Printf_Hex(cursor_to_home, NULL);
				UARTPC_Printf_Hex(clear_terminal, NULL);
				UARTPC_Printf_Hex(whiteText, NULL);
				UARTPC_Printf_Hex(writeMemory_string, NULL);
				UARTPC_Printf_Hex(blueText, NULL);
				UARTPC_Printf_Hex(requestString, NULL);
				reset = FALSE;
			}

			UARTPC_waitingKey(PC_keyPressed);
			if ('\e' != *PC_keyPressed && '\r' != *PC_keyPressed && 0x1 != *PC_keyPressed) {
				xQueueSend(getUART0Queue(), (void * )&PC_keyPressed,
						QUEUE_WAITTIME);
			}
			UARTPC_Printf_Key(PC_keyPressed);

			if (ENTER_KEY == *PC_keyPressed) {
				queueSize = uxQueueMessagesWaiting(getUART0Queue());
				for (uint8_t i = 0; i < queueSize; i++) {
					xQueueReceive(getUART0Queue(), &UART0_Buffer[i],
							QUEUE_WAITTIME);
				}

				UART0_Buffer[SB] = UART0_Buffer[SB] - 0x30;
				UART0_Buffer[SB + 1] = UART0_Buffer[SB + 1] - 0x30;
				UART0_Buffer[SB + 2] = UART0_Buffer[SB + 2] - 0x30;
				UART0_Buffer[SB + 3] = UART0_Buffer[SB + 3] - 0x30;
				mem_address = (UART0_Buffer[SB] << 12)
						| (UART0_Buffer[SB + 1] << 8)
						| (UART0_Buffer[SB + 2] << 4) | UART0_Buffer[SB + 3];

				for (uint8_t i = 0; UART0_Buffer[i + OFFSET] != '\0'; i++) {
					info_size = i;
				}
				if(0x1 != UART0_Buffer[info_size+OFFSET]){
					info_size++;
				}
				for (uint8_t i = 0; i < info_size; i++) {
					INFO_Buffer[i] = UART0_Buffer[i + OFFSET];
				}
				stringPtr = INFO_Buffer;
				xSemaphoreTake(getMutex(),pMAX_DELAY);
				{
				I2C_WriteReg(BOARD_I2C0_BASEADDR, MEMORY_ADDRESS, mem_address,
						stringPtr);
				}
				xSemaphoreGive(getMutex());

				UARTPC_Printf_Hex(whiteText, NULL);
				zerosBuffer(UART0_Buffer, UART_QUEUE_SIZE);
				zerosBuffer(INFO_Buffer, INFO_BUFFER_SIZE);
				xQueueReset(getUART0Queue());
				info_size = HexToBCD(info_size);
				UARTPC_Printf_Hex(
						"\r\n\r\nSu texto ha sido guardado. Se escribieron: ",
						&info_size);
				UARTPC_Printf_String(" bytes.", NULL);
				UARTPC_Printf_ESC();
			}

			if (ESC == *PC_keyPressed) {
				PC_printed = FALSE;
				reset = TRUE;
				Task_TWO = FREE;
				xSemaphoreGive(xMenu_SemaphoreHandler);
				vTaskSuspend(NULL);
			}
		}
	}
}

void Set_Hour_Task(void *pvParameters) {
	vTaskSuspend(NULL);

	uint8_t UART0_Buffer[SMALL_BUFFER_SIZE];
	uint8_t UART3_Buffer[SMALL_BUFFER_SIZE];
	uint8_t readBuff[MIN_BUFFER_SIZE];
	zerosBuffer(UART0_Buffer, SMALL_BUFFER_SIZE);
	zerosBuffer(UART3_Buffer, SMALL_BUFFER_SIZE);
	zerosBuffer(readBuff, MIN_BUFFER_SIZE);

	uint8_t queueSize;
	uint8_t count = 0;
	uint8_t databyte;
	uint8_t sec, min, hour; /*< Variebles que almacenan la informacion de la hora y fecha */
	uint8_t reset = TRUE;
	uint8_t SB = 0;

	for (;;) {

		if (BT == Terminal_THREE) {
			if (reset) {
				UARTBT_Printf_Hex(cursor_to_home, NULL);
				UARTBT_Printf_Hex(clear_terminal, NULL);
				UARTBT_Printf_Hex(whiteText, NULL);
				if (format24h) {
					UARTBT_Printf_Hex(setHour_string_24h, NULL);
				} else {
					UARTBT_Printf_Hex(setHour_string_12h, NULL);
				}
				UARTBT_Printf_Hex(blueText, NULL);
				UARTBT_Printf_Hex(requestString, NULL);
				reset = FALSE;
			}

			UARTBT_waitingKey(BT_keyPressed);
			if ('\e' != *BT_keyPressed && '\r' != *BT_keyPressed && 0x1 != *PC_keyPressed) {
				xQueueSend(getUART3Queue(), (void * )&BT_keyPressed,
						QUEUE_WAITTIME);
			}
			UARTBT_Printf_Key(BT_keyPressed);

			if (*BT_keyPressed == ENTER_KEY) {
				queueSize = uxQueueMessagesWaiting(getUART3Queue());
				for (uint8_t i = 0; i < queueSize; i++) {
					xQueueReceive(getUART3Queue(), &UART3_Buffer[i],
							QUEUE_WAITTIME);
				}

				UART3_Buffer[SB] = UART3_Buffer[SB] - 0x30;
				UART3_Buffer[SB + 1] = UART3_Buffer[SB + 1] - 0x30;
				databyte = (UART3_Buffer[SB] << 4) | UART3_Buffer[SB + 1];
				if (!format24h) {
					if ('A' == UART3_Buffer[SB + 9]
							|| 'a' == UART3_Buffer[SB + 9])
						formatAM = TRUE;
					else if ('P' == UART3_Buffer[SB + 9]
							|| 'p' == UART3_Buffer[SB + 9])
						formatAM = FALSE;
					if (formatAM)
						databyte &= MASK_AM_FORMAT;
					else
						databyte |= MASK_PM_FORMAT;
				} else {
					if (UART3_Buffer[SB + 1] >= 2 && UART3_Buffer[SB] > 0) {
						formatAM = FALSE;
					}
				}
				xSemaphoreTake(getMutex(),pMAX_DELAY);
				{
				I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,
						RTC_HOURS_REG, databyte);
				UART3_Buffer[SB + 3] = UART3_Buffer[SB + 3] - 0x30;
				UART3_Buffer[SB + 4] = UART3_Buffer[SB + 4] - 0x30;
				databyte = (UART3_Buffer[SB + 3] << 4) | UART3_Buffer[SB + 4];
				I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,
						RTC_MINUTES_REG, databyte);
				UART3_Buffer[SB + 6] = UART3_Buffer[SB + 6] - 0x30;
				UART3_Buffer[SB + 7] = UART3_Buffer[SB + 7] - 0x30;
				databyte = (UART3_Buffer[SB + 6] << 4) | UART3_Buffer[SB + 7];
				databyte = databyte | MASK_INIT_RTC;
				I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,
						RTC_SECONDS_REG, databyte);
				}
				xSemaphoreGive(getMutex());
				UARTBT_Printf_Hex(whiteText, NULL);
				zerosBuffer(UART3_Buffer, SMALL_BUFFER_SIZE);
				zerosBuffer(readBuff, MIN_BUFFER_SIZE);
				xQueueReset(getUART3Queue());
				UARTBT_Printf_Hex("\r\n\r\nHora establecita correctamente.",
				NULL);
				UARTBT_Printf_ESC();
			}
			if (ESC == *BT_keyPressed) {
				BT_printed = FALSE;
				reset = TRUE;
				Task_THREE = FREE;
				xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				vTaskSuspend(NULL);
			}
		}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else {

			if (reset) {
				UARTPC_Printf_Hex(cursor_to_home, NULL);
				UARTPC_Printf_Hex(clear_terminal, NULL);
				UARTPC_Printf_Hex(whiteText, NULL);
				if (format24h) {
					UARTPC_Printf_Hex(setHour_string_24h, NULL);
				} else {
					UARTPC_Printf_Hex(setHour_string_12h, NULL);
				}
				UARTPC_Printf_Hex(blueText, NULL);
				UARTPC_Printf_Hex(requestString, NULL);
				reset = FALSE;
			}
			UARTPC_waitingKey(PC_keyPressed);
			if ('\e' != *PC_keyPressed && '\r' != *PC_keyPressed) {
				xQueueSend(getUART0Queue(), (void * )&PC_keyPressed,QUEUE_WAITTIME);
			}
			UARTPC_Printf_Key(PC_keyPressed);

			if (ENTER_KEY == *PC_keyPressed) {
				queueSize = uxQueueMessagesWaiting(getUART0Queue());
				for (uint8_t i = 0; i < queueSize; i++) {
					xQueueReceive(getUART0Queue(), &UART0_Buffer[i],
							QUEUE_WAITTIME);
				}

				UART0_Buffer[SB] = UART0_Buffer[SB] - 0x30;
				UART0_Buffer[SB + 1] = UART0_Buffer[SB + 1] - 0x30;
				databyte = (UART0_Buffer[SB] << 4) | UART0_Buffer[SB + 1];
				if (!format24h) {
					if ('A' == UART0_Buffer[SB + 9]
							|| 'a' == UART0_Buffer[SB + 9])
						formatAM = TRUE;
					else if ('P' == UART0_Buffer[SB + 9]
							|| 'p' == UART0_Buffer[SB + 9])
						formatAM = FALSE;
					if (formatAM)
						databyte &= MASK_AM_FORMAT;
					else
						databyte |= MASK_PM_FORMAT;
				} else {
					if (UART0_Buffer[SB + 1] >= 2 && UART0_Buffer[SB] > 0) {
						formatAM = FALSE;
					}
				}
				xSemaphoreTake(getMutex(),pMAX_DELAY);
				{
				I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,
						RTC_HOURS_REG, databyte);
				UART0_Buffer[SB + 3] = UART0_Buffer[SB + 3] - 0x30;
				UART0_Buffer[SB + 4] = UART0_Buffer[SB + 4] - 0x30;
				databyte = (UART0_Buffer[SB + 3] << 4) | UART0_Buffer[SB + 4];
				I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,
						RTC_MINUTES_REG, databyte);
				UART0_Buffer[SB + 6] = UART0_Buffer[SB + 6] - 0x30;
				UART0_Buffer[SB + 7] = UART0_Buffer[SB + 7] - 0x30;
				databyte = (UART0_Buffer[SB + 6] << 4) | UART0_Buffer[SB + 7];
				databyte = databyte | MASK_INIT_RTC;
				I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,
						RTC_SECONDS_REG, databyte);
				}
				xSemaphoreGive(getMutex());
				UARTPC_Printf_Hex(whiteText, NULL);
				zerosBuffer(UART0_Buffer, SMALL_BUFFER_SIZE);
				zerosBuffer(readBuff, MIN_BUFFER_SIZE);
				xQueueReset(getUART0Queue());
				UARTPC_Printf_Hex("\r\n\r\nHora establecita correctamente.",
						NULL);
				UARTPC_Printf_ESC();
			}
			if (ESC == *PC_keyPressed) {
				PC_printed = FALSE;
				reset = TRUE;
				Task_THREE = FREE;
				xSemaphoreGive(xMenu_SemaphoreHandler);
				vTaskSuspend(NULL);
			}
		}
	}
}

void Get_Hour_Task(void *pvParameters) {
	vTaskSuspend(NULL);

	uint8_t readBuff[MIN_BUFFER_SIZE]; /*< Buffer donde se guarda el byte recibido del RTC */
	uint8_t sec, min, hour; /*< Variebles que almacenan la informacion de la hora y fecha */
	uint8_t databyte; /*< Informacion que se enviara al RTC al escribir la I2C. */

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = ONE_SECOND;
	xLastWakeTime = xTaskGetTickCount();
	uint8_t reset = TRUE;

	for (;;) {
		if (BT == Terminal_SIX) {
			if (reset) {
				UARTBT_Printf_Hex(cursor_to_home, NULL);
				UARTBT_Printf_Hex(clear_terminal, NULL);
				UARTBT_Printf_Hex(whiteText, NULL);
				UARTBT_Printf_Hex("\r\n\r\nLa hora actual es:	", NULL);
				UARTBT_Printf_Hex(blueText, NULL);
				reset = FALSE;
			}
			xSemaphoreTake(getMutex(),pMAX_DELAY);
			{
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_SECONDS_REG,
					readBuff, READDATA_SIZE);
			sec = readBuff[0] & MASK_ST_BIT; /* Dado que el primer SB del registro de los segundos no es relevante, se debe filtrar dicho SB para leer los 7 restantes. */
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_MINUTES_REG,
					readBuff, READDATA_SIZE);
			min = readBuff[0];
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_HOURS_REG,
					readBuff, READDATA_SIZE);
			}
			xSemaphoreGive(getMutex());
			hour = readBuff[0];
			if (!format24h)
				hour = hour & MASK_12H_GET;

			UARTBT_Printf_Hex(blueText, NULL);
			UARTBT_Printf_Hex(" ", &hour);
			UARTBT_Printf_Hex(":", &min);
			UARTBT_Printf_Hex(":", &sec);

			if (!format24h) {
				if (formatAM)
					UARTBT_Printf_Hex(" AM", NULL);
				else
					UARTBT_Printf_Hex(" PM", NULL);
			}
			UARTBT_Printf_BTN();
			UARTBT_Printf_Hex(cursor_to_time, NULL);

//			if (pdTRUE == xSemaphoreTake(getBotton1Sem(), BOTTONS_SEM_WAITTIME)) {
			if(botton1_pressed){
				botton1_pressed = FALSE;
				BT_printed = FALSE;
				reset = TRUE;
				Task_SIX = FREE;
				xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				vTaskSuspend(NULL);
			}
			vTaskDelayUntil(&xLastWakeTime, xFrequency); /* Espera a que termine 1 segundo. */
		}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else {
			if (reset) {
				UARTPC_Printf_Hex(cursor_to_home, NULL);
				UARTPC_Printf_Hex(clear_terminal, NULL);
				UARTPC_Printf_Hex(whiteText, NULL);
				UARTPC_Printf_Hex("\r\n\r\nLa hora actual es:	", NULL);
				UARTPC_Printf_Hex(blueText, NULL);
				reset = FALSE;
			}
			xSemaphoreTake(getMutex(),pMAX_DELAY);
			{
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_SECONDS_REG,
					readBuff, READDATA_SIZE);
			sec = readBuff[0] & MASK_ST_BIT; /* Dado que el primer SB del registro de los segundos no es relevante, se debe filtrar dicho SB para leer los 7 restantes. */
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_MINUTES_REG,
					readBuff, READDATA_SIZE);
			min = readBuff[0];
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_HOURS_REG,
					readBuff, READDATA_SIZE);
			}
			xSemaphoreGive(getMutex());
			hour = readBuff[0];
			if (!format24h)
				hour = hour & MASK_12H_GET;

			UARTPC_Printf_Hex(blueText, NULL);
			UARTPC_Printf_Hex(" ", &hour);
			UARTPC_Printf_Hex(":", &min);
			UARTPC_Printf_Hex(":", &sec);

			if (!format24h) {
				if (formatAM)
					UARTPC_Printf_Hex(" AM", NULL);
				else
					UARTPC_Printf_Hex(" PM", NULL);
			}
			UARTPC_Printf_BTN();
			UARTPC_Printf_Hex(cursor_to_time, NULL);

//			if (pdTRUE == xSemaphoreTake(getBotton1Sem(), BOTTONS_SEM_WAITTIME)) {
			if(botton1_pressed){
				botton1_pressed = FALSE;
				PC_printed = FALSE;
				reset = TRUE;
				Task_SIX = FREE;
				xSemaphoreGive(xMenu_SemaphoreHandler);
				vTaskSuspend(NULL);
			}
			vTaskDelayUntil(&xLastWakeTime, xFrequency); /* Espera a que termine 1 segundo. */
		}
	}
}

void Set_Date_Task(void *pvParameters) {
	vTaskSuspend(NULL);

	uint8_t queueSize;
	uint8_t UART0_Buffer[BIG_BUFFER_SIZE];
	uint8_t UART3_Buffer[BIG_BUFFER_SIZE];
	uint8_t readBuff[MIN_BUFFER_SIZE];
	zerosBuffer(UART0_Buffer, BIG_BUFFER_SIZE);
	zerosBuffer(UART3_Buffer, BIG_BUFFER_SIZE);
	zerosBuffer(readBuff, MIN_BUFFER_SIZE);

	uint8_t databyte;
	uint8_t day, month, year; /*< Variebles que almacenan la informacion de la hora y fecha */
	uint8_t reset = TRUE;
	uint8_t SB = 0;

	for (;;) {
		if (BT == Terminal_FOUR) {
			if (reset) {
				UARTBT_Printf_Hex(cursor_to_home, NULL);
				UARTBT_Printf_Hex(clear_terminal, NULL);
				UARTBT_Printf_Hex(whiteText, NULL);
				UARTBT_Printf_Hex(setDate_string, NULL);
				UARTBT_Printf_Hex(blueText, NULL);
				UARTBT_Printf_Hex(requestString, NULL);
				reset = FALSE;
			}
			UARTBT_waitingKey(BT_keyPressed);
			if ('\e' != *BT_keyPressed && '\r' != *BT_keyPressed && 0x1 != *PC_keyPressed) {
				xQueueSend(getUART3Queue(), (void * )&BT_keyPressed,
						QUEUE_WAITTIME);
			}
			UARTBT_Printf_Key(BT_keyPressed);

			if (ENTER_KEY == *BT_keyPressed) {
				UARTBT_Printf_Hex(whiteText, NULL);
				queueSize = uxQueueMessagesWaiting(getUART3Queue());
				for (uint8_t i = 0; i < queueSize; i++) {
					xQueueReceive(getUART3Queue(), &UART3_Buffer[i],
							QUEUE_WAITTIME);
				}
				xSemaphoreTake(getMutex(),pMAX_DELAY);
				{
				UART3_Buffer[SB] = UART3_Buffer[SB] - 0x30;
				UART3_Buffer[SB + 1] = UART3_Buffer[SB + 1] - 0x30;
				databyte = (UART3_Buffer[SB] << 4) | UART3_Buffer[SB + 1];
				I2C_WriteReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_DAY_REG,
						&databyte);
				UART3_Buffer[SB + 3] = UART3_Buffer[SB + 3] - 0x30;
				UART3_Buffer[SB + 4] = UART3_Buffer[SB + 4] - 0x30;
				databyte = (UART3_Buffer[SB + 3] << 4) | UART3_Buffer[SB + 4];
				I2C_WriteReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_MONTH_REG,
						&databyte);
				UART3_Buffer[SB + 6] = UART3_Buffer[SB + 6] - 0x30;
				UART3_Buffer[SB + 7] = UART3_Buffer[SB + 7] - 0x30;
				databyte = (UART3_Buffer[SB + 6] << 4) | UART3_Buffer[SB + 7];
				databyte = databyte;
				I2C_WriteReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_YEAR_REG,
						&databyte);
				}
				xSemaphoreGive(getMutex());
				UARTBT_Printf_Hex("\r\n\r\nFecha establecita correctamente.",
						NULL);
				UARTBT_Printf_ESC();
			}
			if (ESC == *BT_keyPressed) {
				BT_printed = FALSE;
				reset = TRUE;
				Task_FOUR = FREE;
				xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				vTaskSuspend(NULL);
			}
		}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else {

			if (reset) {
				UARTPC_Printf_Hex(cursor_to_home, NULL);
				UARTPC_Printf_Hex(clear_terminal, NULL);
				UARTPC_Printf_Hex(whiteText, NULL);
				UARTPC_Printf_Hex(setDate_string, NULL);
				UARTPC_Printf_Hex(blueText, NULL);
				UARTPC_Printf_Hex(requestString, NULL);
				reset = FALSE;
			}
			UARTPC_waitingKey(PC_keyPressed);
			if ('\e' != *PC_keyPressed && '\r' != *PC_keyPressed) {
				xQueueSend(getUART0Queue(), (void * )&PC_keyPressed,
						QUEUE_WAITTIME);
			}
			UARTPC_Printf_Key(PC_keyPressed);

			if (ENTER_KEY == *PC_keyPressed) {
				UARTPC_Printf_Hex(whiteText, NULL);
				queueSize = uxQueueMessagesWaiting(getUART0Queue());
				for (uint8_t i = 0; i < queueSize; i++) {
					xQueueReceive(getUART0Queue(), &UART0_Buffer[i],
							QUEUE_WAITTIME);
				}
				xSemaphoreTake(getMutex(),pMAX_DELAY);
				{
				UART0_Buffer[SB] = UART0_Buffer[SB] - 0x30;
				UART0_Buffer[SB + 1] = UART0_Buffer[SB + 1] - 0x30;
				databyte = (UART0_Buffer[SB] << 4) | UART0_Buffer[SB + 1];
				I2C_WriteReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_DAY_REG,
						&databyte);
				UART0_Buffer[SB + 3] = UART0_Buffer[SB + 3] - 0x30;
				UART0_Buffer[SB + 4] = UART0_Buffer[SB + 4] - 0x30;
				databyte = (UART0_Buffer[SB + 3] << 4) | UART0_Buffer[SB + 4];
				I2C_WriteReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_MONTH_REG,
						&databyte);
				UART0_Buffer[SB + 6] = UART0_Buffer[SB + 6] - 0x30;
				UART0_Buffer[SB + 7] = UART0_Buffer[SB + 7] - 0x30;
				databyte = (UART0_Buffer[SB + 6] << 4) | UART0_Buffer[SB + 7];
				databyte = databyte;
				I2C_WriteReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_YEAR_REG,
						&databyte);
				}
				xSemaphoreGive(getMutex());
				UARTPC_Printf_Hex("\r\n\r\nFecha establecita correctamente.",
						NULL);
				UARTPC_Printf_ESC();
			}
			if (ESC == *PC_keyPressed) {
				PC_printed = FALSE;
				reset = TRUE;
				Task_FOUR = FREE;
				xSemaphoreGive(xMenu_SemaphoreHandler);
				vTaskSuspend(NULL);
			}
		}
	}
}

void Get_Date_Task(void *pvParameters) {
	vTaskSuspend(NULL);

	uint8_t UART0_Buffer[SMALL_BUFFER_SIZE];
	uint8_t UART3_Buffer[SMALL_BUFFER_SIZE];
	uint8_t readBuff[MIN_BUFFER_SIZE]; /*< Buffer donde se guarda el byte recibido del RTC */
	zerosBuffer(UART0_Buffer, SMALL_BUFFER_SIZE);
	zerosBuffer(UART3_Buffer, SMALL_BUFFER_SIZE);
	zerosBuffer(readBuff, MIN_BUFFER_SIZE);

	uint8_t queueSize;
	uint8_t day, month, year; /*< Variebles que almacenan la informacion de la hora y fecha */
	uint8_t databyte; /*< Informacion que se enviara al RTC al escribir la I2C. */
	uint8_t reset = TRUE;

	for (;;) {
		if (BT == Terminal_SEVEN) {
			if (reset) {
				UARTBT_Printf_Hex(cursor_to_home, NULL);
				UARTBT_Printf_Hex(clear_terminal, NULL);
				UARTBT_Printf_Hex(whiteText, NULL);
				UARTBT_Printf_Hex("\r\n\r\nLa fecha actual es:	", NULL);
				UARTBT_Printf_Hex(blueText, NULL);
				reset = FALSE;
			}
			xSemaphoreTake(getMutex(),pMAX_DELAY);
			{
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_DAY_REG, readBuff,READDATA_SIZE);
			day = readBuff[0];
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_MONTH_REG,readBuff, READDATA_SIZE);
			month = readBuff[0];
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_YEAR_REG,readBuff, READDATA_SIZE);
			year = readBuff[0];
			}
			xSemaphoreGive(getMutex());

			UARTBT_Printf_Hex(blueText, NULL);
			UARTBT_Printf_Hex(" ", &day);
			UARTBT_Printf_Hex("/", &month);
			UARTBT_Printf_Hex("/", &year);

			UARTBT_Printf_ESC();
			UARTBT_waitingKey(BT_keyPressed);

			if (ESC == *BT_keyPressed) {
				BT_printed = FALSE;
				reset = TRUE;
				Task_SEVEN = FREE;
				xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				vTaskSuspend(NULL);
			}
		}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else {
			if (reset) {
				UARTPC_Printf_Hex(cursor_to_home, NULL);
				UARTPC_Printf_Hex(clear_terminal, NULL);
				UARTPC_Printf_Hex(whiteText, NULL);
				UARTPC_Printf_Hex("\r\n\r\nLa fecha actual es:	", NULL);
				UARTPC_Printf_Hex(blueText, NULL);
				reset = FALSE;
			}
			xSemaphoreTake(getMutex(),pMAX_DELAY);
			{
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_DAY_REG, readBuff,READDATA_SIZE);
			day = readBuff[0];
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_MONTH_REG,readBuff, READDATA_SIZE);
			month = readBuff[0];
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_YEAR_REG,readBuff, READDATA_SIZE);
			year = readBuff[0];
			}
			xSemaphoreGive(getMutex());

			UARTPC_Printf_Hex(blueText, NULL);
			UARTPC_Printf_Hex(" ", &day);
			UARTPC_Printf_Hex("/", &month);
			UARTPC_Printf_Hex("/", &year);

			UARTPC_Printf_ESC();
			UARTPC_waitingKey(PC_keyPressed);

			if (ESC == *PC_keyPressed) {
				PC_printed = FALSE;
				reset = TRUE;
				Task_SEVEN = FREE;
				xSemaphoreGive(xMenu_SemaphoreHandler);
				vTaskSuspend(NULL);
			}
		}
	}
}

void Hour_Format_Task(void *pvParameters) {
	vTaskSuspend(NULL);

	uint8_t queueSize;
	uint8_t UART0_Buffer[SMALL_BUFFER_SIZE];
	uint8_t UART3_Buffer[SMALL_BUFFER_SIZE];
	uint8_t readBuff[MIN_BUFFER_SIZE];
	zerosBuffer(UART0_Buffer, SMALL_BUFFER_SIZE);
	zerosBuffer(readBuff, MIN_BUFFER_SIZE);

	uint8_t databyte;
	uint8_t sec, min, hour; /*< Variebles que almacenan la informacion de la hora y fecha */
	uint8_t reset = TRUE;

	for (;;) {
		if (BT == Terminal_FIVE) {
			if (reset) {
				UARTBT_Printf_Hex(cursor_to_home, NULL);
				UARTBT_Printf_Hex(clear_terminal, NULL);
				UARTBT_Printf_Hex(whiteText, NULL);
				if (format24h) {
					UARTBT_Printf_Hex(Format24h_string_1, NULL);
					UARTBT_Printf_Hex(blueText, NULL);
					UARTBT_Printf_Hex(Format24h_string_2, NULL);
					reset = FALSE;
				} else {
					UARTBT_Printf_Hex(Format12h_string_1, NULL);
					UARTBT_Printf_Hex(blueText, NULL);
					UARTBT_Printf_Hex(Format12h_string_2, NULL);
					reset = FALSE;
				}
			}
			UARTBT_waitingKey(BT_keyPressed);
			if ('\e' != *BT_keyPressed && '\r' != *BT_keyPressed && 0x1 != *PC_keyPressed) {
				xQueueSend(getUART3Queue(), (void * )&BT_keyPressed,
						QUEUE_WAITTIME);
			}
			UARTBT_Printf_Key(BT_keyPressed);

			if (*BT_keyPressed == ENTER_KEY) {
				UARTBT_Printf_Hex(whiteText, NULL);
				queueSize = uxQueueMessagesWaiting(getUART3Queue());
				for (uint8_t i = 0; i < queueSize; i++) {
					xQueueReceive(getUART3Queue(), &UART3_Buffer[i],
							QUEUE_WAITTIME);
				}
				xSemaphoreTake(getMutex(),pMAX_DELAY);
				{
				if ('S' == UART3_Buffer[0] || 's' == UART3_Buffer[0]) {
					if (format24h) {
						format24h = FALSE;
						I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS,RTC_HOURS_REG, readBuff, READDATA_SIZE);
						databyte = readBuff[0];
						databyte = BCDtoHex(databyte);
						if (databyte >= MASK_OFFSET_FORMAT)
							databyte -= MASK_OFFSET_FORMAT;
						databyte = databyte | MASK_12H_SET;
						I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,
								RTC_HOURS_REG, databyte);
					} else {
						format24h = TRUE;
						I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS,
								RTC_HOURS_REG, readBuff, READDATA_SIZE);
						databyte = readBuff[0];
						databyte &= MASK_12H_GET;
						if (!formatAM) {
							databyte = BCDtoHex(databyte);
							databyte += MASK_OFFSET_FORMAT;
						}
						databyte &= MASK_24H_SET;
						databyte = DecToBCD(databyte);
						I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,
								RTC_HOURS_REG, databyte);
					}
					UARTBT_Printf_Hex("\r\n\r\nEl formato ha sido cambiado.",
							NULL);
				}
				}
				xSemaphoreGive(getMutex());
				UARTBT_Printf_ESC();
				UARTBT_waitingKey(BT_keyPressed);

				if (ESC == *BT_keyPressed) {
					BT_printed = FALSE;
					reset = TRUE;
					Task_FIVE = FREE;
					xSemaphoreGive(xMenu_BT_SemaphoreHandler);
					vTaskSuspend(NULL);
				}
			}
		}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else {
			if (reset) {
				UARTPC_Printf_Hex(cursor_to_home, NULL);
				UARTPC_Printf_Hex(clear_terminal, NULL);
				UARTPC_Printf_Hex(whiteText, NULL);
				if (format24h) {
					UARTPC_Printf_Hex(Format24h_string_1, NULL);
					UARTPC_Printf_Hex(blueText, NULL);
					UARTPC_Printf_Hex(Format24h_string_2, NULL);
					reset = FALSE;
				} else {
					UARTPC_Printf_Hex(Format12h_string_1, NULL);
					UARTPC_Printf_Hex(blueText, NULL);
					UARTPC_Printf_Hex(Format12h_string_2, NULL);
					reset = FALSE;
				}
			}
			UARTPC_waitingKey(PC_keyPressed);
			if ('\e' != *PC_keyPressed && '\r' != *PC_keyPressed) {
				xQueueSend(getUART0Queue(), (void * )&PC_keyPressed,
						QUEUE_WAITTIME);
			}
			UARTPC_Printf_Key(PC_keyPressed);

			if (ENTER_KEY == *PC_keyPressed) {
				UARTPC_Printf_Hex(whiteText, NULL);
				queueSize = uxQueueMessagesWaiting(getUART0Queue());
				for (uint8_t i = 0; i < queueSize; i++) {
					xQueueReceive(getUART0Queue(), &UART0_Buffer[i],
							QUEUE_WAITTIME);
				}
				xSemaphoreTake(getMutex(),pMAX_DELAY);
				{
				if ('S' == UART0_Buffer[0] || 's' == UART0_Buffer[0]) {
					if (format24h) {
						format24h = FALSE;
						I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS,RTC_HOURS_REG, readBuff, READDATA_SIZE);
						databyte = readBuff[0];
						databyte = BCDtoHex(databyte);
						if (databyte >= MASK_OFFSET_FORMAT)
							databyte -= MASK_OFFSET_FORMAT;
						databyte = databyte | MASK_12H_SET;
						I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,RTC_HOURS_REG, databyte);
					} else {
						format24h = TRUE;
						I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS,RTC_HOURS_REG, readBuff, READDATA_SIZE);
						databyte = readBuff[0];
						databyte &= MASK_12H_GET;
						if (!formatAM) {
							databyte = BCDtoHex(databyte);
							databyte += MASK_OFFSET_FORMAT;
						}
						databyte &= MASK_24H_SET;
						databyte = DecToBCD(databyte);
						I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,
								RTC_HOURS_REG, databyte);
					}
					UARTPC_Printf_Hex("\r\n\r\nEl formato ha sido cambiado.",
							NULL);
				}
				}
				xSemaphoreGive(getMutex());
				UARTPC_Printf_ESC();
				UARTPC_waitingKey(PC_keyPressed);

				if (ESC == *PC_keyPressed) {
					PC_printed = FALSE;
					reset = TRUE;
					Task_FIVE = FREE;
					xSemaphoreGive(xMenu_SemaphoreHandler);
					vTaskSuspend(NULL);
				}
			}
		}

	}
}

void UART0_Chat_vTask( void * pvParameter ) {
	vTaskSuspend(NULL);
	uint8_t queueSize;

	uint8_t UART0_Buffer[BIG_BUFFER_SIZE];
	uint8_t readBuff[MIN_BUFFER_SIZE];
	zerosBuffer(UART0_Buffer,BIG_BUFFER_SIZE);
	zerosBuffer(readBuff,MIN_BUFFER_SIZE);

	uint8_t reset = TRUE;

	for(;;) {
		if(reset){
			UARTPC_Printf_Hex(cursor_to_home,NULL);
		   	UARTPC_Printf_Hex(clear_terminal,NULL);
		  	UARTPC_Printf_Hex(blueText,NULL);
		  	UARTPC_Printf_Hex(terminal1,NULL);
		  	reset = FALSE;
		}

		UARTPC_waitingKey(PC_keyPressed);
		if('\e' != *PC_keyPressed && '\r' != *PC_keyPressed){
			xQueueSend(getUART0Queue(),(void *)&PC_keyPressed,QUEUE_WAITTIME);
		}
		UARTPC_Printf_Hex(blueText,NULL);
		UARTPC_Printf_Key(PC_keyPressed);

		if(ENTER_KEY == *PC_keyPressed) {
			queueSize = uxQueueMessagesWaiting(getUART0Queue());
			for (uint8_t i = 0; i < queueSize; i++) {
				xQueueReceive(getUART0Queue(), &UART0_Buffer[i],QUEUE_WAITTIME);
			}
			UARTPC_Printf_Hex("\r\n",NULL);
			UARTBT_Printf_Hex(blueText,NULL);
			UARTBT_Printf_Hex(terminal1,NULL);
			UARTBT_Printf_Hex(UART0_Buffer,NULL);
			UARTBT_Printf_Hex("\r\n",NULL);
			UARTBT_Printf_Hex(redText,NULL);
			UARTBT_Printf_Hex(terminal2,NULL);
		}
		if (ESC == *PC_keyPressed) {
			if(!Someone_exit){
				UARTBT_Printf_Hex(yellowText,NULL);
				UARTBT_Printf_Hex(terminal1_exit,NULL);
			}
			Someone_exit = TRUE;
			PC_printed = FALSE;
			reset = TRUE;
			Task_FIVE = FREE;
			xSemaphoreGive(xMenu_SemaphoreHandler);
			vTaskSuspend(NULL);
		}
	}
}

void UART3_Chat_vTask( void * pvParameter ) {
	vTaskSuspend(NULL);

	uint8_t queueSize;
	uint8_t UART3_Buffer[BIG_BUFFER_SIZE];
	uint8_t readBuff[MIN_BUFFER_SIZE];
	zerosBuffer(UART3_Buffer,BIG_BUFFER_SIZE);
	zerosBuffer(readBuff,MIN_BUFFER_SIZE);

	uint8_t reset = TRUE;

	for(;;) {
		if(reset){
			UARTBT_Printf_Hex(cursor_to_home,NULL);
		   	UARTBT_Printf_Hex(clear_terminal,NULL);
		  	reset = FALSE;
		}

		UARTBT_waitingKey(BT_keyPressed);
		if('\e' != *BT_keyPressed && '\r' != *BT_keyPressed && 0x1 != *PC_keyPressed){
			xQueueSend(getUART3Queue(),(void *)&BT_keyPressed,QUEUE_WAITTIME);
		}
		UARTBT_Printf_Hex(redText,NULL);
		UARTBT_Printf_Key(BT_keyPressed);

		if(ENTER_KEY == *BT_keyPressed) {
			queueSize = uxQueueMessagesWaiting(getUART3Queue());
			for (uint8_t i = 0; i < queueSize; i++) {
				xQueueReceive(getUART3Queue(), &UART3_Buffer[i],QUEUE_WAITTIME);
			}
			UARTBT_Printf_Hex("\r\n",NULL);
			UARTPC_Printf_Hex(redText,NULL);
			UARTPC_Printf_Hex(terminal2,NULL);
			UARTPC_Printf_Hex(UART3_Buffer,NULL);
			UARTPC_Printf_Hex("\r\n",NULL);
			UARTPC_Printf_Hex(blueText,NULL);
			UARTPC_Printf_Hex(terminal1,NULL);
		}
		if (ESC == *BT_keyPressed) {
			if(!Someone_exit){
				UARTPC_Printf_Hex(yellowText,NULL);
				UARTPC_Printf_Hex(terminal2_exit,NULL);
			}
			Someone_exit = TRUE;
			BT_printed = FALSE;
			reset = TRUE;
			Task_FIVE = FREE;
			xSemaphoreGive(xMenu_BT_SemaphoreHandler);
			vTaskSuspend(NULL);
		}
	}
}

void Print_LCD_task(void *pvParameters){
	uint8_t readBuff[MIN_BUFFER_SIZE];	/*< Buffer donde se guarda el byte recibido del RTC */
	uint8_t day, month, year;	/*< Variebles que almacenan la informacion de la hora y fecha */
	uint8_t sec, min, hour;	/*< Variebles que almacenan la informacion de la hora y fecha */

	LCDNokia_clear();

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = ONE_SECOND;
	xLastWakeTime = xTaskGetTickCount();


	for (;;) {
		xSemaphoreTake(getMutex(),pMAX_DELAY);
		{
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_DAY_REG, readBuff,READDATA_SIZE);
			day = readBuff[0];
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_MONTH_REG, readBuff,READDATA_SIZE);
			month = readBuff[0];
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_YEAR_REG, readBuff,READDATA_SIZE);
			year = readBuff[0];

			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_SECONDS_REG, readBuff,READDATA_SIZE);
			sec = readBuff[0] & MASK_ST_BIT;
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_MINUTES_REG, readBuff,READDATA_SIZE);
			min = readBuff[0];
			I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_HOURS_REG, readBuff,READDATA_SIZE);
			hour = readBuff[0];
			if(!format24h){
				hour &= MASK_12H_GET;
			}else{
				hour &= MASK_24H_SET;
			}
		}
		xSemaphoreGive(getMutex());

		printLCD_Strings(25, 0, Fecha_LCD);
		printLCD_Numeros(15, 1, &day);
		printLCD_Strings(30, 1, "/");
		printLCD_Numeros(37, 1, &month);
		printLCD_Strings(50, 1, "/");
		printLCD_Numeros(57, 1, &year);

		printLCD_Strings(27, 3, Hour_LCD);
		printLCD_Numeros(15, 4, &hour);
		printLCD_Strings(30, 4, ":");
		printLCD_Numeros(37, 4, &min);
		printLCD_Strings(50, 4, ":");
		printLCD_Numeros(57, 4, &sec);

		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void bottons_task(void *pvParameters) {
	uint8_t readBuff[MIN_BUFFER_SIZE];	/*< Buffer donde se guarda el byte recibido del RTC */
	uint8_t day, month, year;	/*< Variebles que almacenan la informacion de la hora y fecha */
	uint8_t sec, min, hour;		/*< Variebles que almacenan la informacion de la hora y fecha */

	for (;;) {
		if(pdTRUE == xSemaphoreTake(getBotton1Sem(), SEM_WAITTIME)){
			if(FREE == Task_FIVE){
				if(configuringButtons){
					configuringButtons = FALSE;
				}else{
					configuringButtons = TRUE;
				}
			}
		}
		else if(pdTRUE == xSemaphoreTake(getBotton2Sem(), SEM_WAITTIME)){
			if(configuringButtons){
				if(SEC == configuring)	configuring = DAYS;
				else					configuring++;
			}
		}
		else if(pdTRUE == xSemaphoreTake(getBotton3Sem(), SEM_WAITTIME)){
			if(configuringButtons){
				xSemaphoreTake(getMutex(),pMAX_DELAY);
				{
				switch(configuring){
				case DAYS:	I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_DAY_REG, readBuff,READDATA_SIZE);
							day = BCDtoHex(readBuff[0]);
							day++;
							day = DecToBCD(day);
							I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,RTC_DAY_REG,day);
							break;
				case MONTHS:I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_MONTH_REG, readBuff,READDATA_SIZE);
							month = BCDtoHex(readBuff[0]);
							month++;
							month = DecToBCD(month);
							I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,RTC_MONTH_REG,month);
							break;
				case YEARS:	I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_YEAR_REG, readBuff,READDATA_SIZE);
							year = BCDtoHex(readBuff[0]);
							year++;
							year = DecToBCD(year);
							I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,RTC_YEAR_REG,year);
							break;
				case HOURS:	I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_HOURS_REG, readBuff,READDATA_SIZE);
							hour = BCDtoHex(readBuff[0]);
							hour++;
							hour = DecToBCD(hour);
							I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,RTC_HOURS_REG,hour);
							break;
				case MINU:	I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_MINUTES_REG, readBuff,READDATA_SIZE);
							min = BCDtoHex(readBuff[0]);
							min++;
							min = DecToBCD(min);
							I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,RTC_MINUTES_REG,min);
							break;
				case SEC:	I2C_ReadReg(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_SECONDS_REG, readBuff,READDATA_SIZE);
							sec = BCDtoHex(readBuff[0] & MASK_ST_BIT);
							sec++;
							sec = DecToBCD(sec);
							I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS,RTC_SECONDS_REG,sec);
							break;
				}
				}
				xSemaphoreGive(getMutex());
			}
		}
		else if(pdTRUE == xSemaphoreTake(getBotton4Sem(), SEM_WAITTIME)){
			if(configuringButtons){

			}
		}
		taskYIELD();
	}
}

void Eco_vTask( void *pvParameter ) {
	vTaskSuspend(NULL);

	uint8_t dataOutput;
	uint8_t uart0_recieve = 0;
	uint8_t uart3_recieve = 0;
	size_t n;
	QueueHandle_t queueUART0_Eco;

//	setUART0Queue(xQueueCreate(UART_QUEUE_SIZE, sizeof(QueueHandle_t)));
//	setUART0Sem(xSemaphoreCreateCounting(UARTTASK_MAXCOUNT, UARTTASK_INITCOUNT));
//	xSemaphoreTake(getUART0Sem(), portMAX_DELAY);
//	setUART3Queue(xQueueCreate(UART_QUEUE_SIZE, sizeof(QueueHandle_t)));
//	setUART3Sem(xSemaphoreCreateCounting(UARTTASK_MAXCOUNT, UARTTASK_INITCOUNT));
//	xSemaphoreTake(getUART3Sem(), portMAX_DELAY);

	PRINT_LCD_UART0();

	LCDNokia_clear();

	for (;;) {
		if (BT == Terminal_NINE) {
			UARTBT_waitingKey(BT_keyPressed);
			if ('\e' != *BT_keyPressed && '\r' != *BT_keyPressed) {
				xQueueSend(getUART3Queue(), (void * )&BT_keyPressed,
						QUEUE_WAITTIME);
			}
			UARTBT_Printf_Key(BT_keyPressed);

			/* The value of the Queue is recovered */
			xQueueReceive(getUART3Queue(), &dataOutput, (TickType_t ) 0);

			/* The value of the Queue is send to the Nokia screen */
			LCDNokia_sendChar(dataOutput);

			/* We check if the key pressed was enter */
			if (*BT_keyPressed == ENTER_KEY) {
				uart3_recieve = NEW_LINE;
				UARTBT_Printf_Key(&uart0_recieve);
				UARTBT_Printf_Key(terminal1);
			}
			if (ESC == *BT_keyPressed) {
				BT_printed = FALSE;
				Task_NINE = TRUE;
				xSemaphoreGive(xMenu_BT_SemaphoreHandler);
				LCDNokia_clear();
				vTaskResume(getPrintLCDTask_Handler());
				vTaskSuspend(NULL);
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else {
			UARTPC_waitingKey(PC_keyPressed);
			if ('\e' != *BT_keyPressed && '\r' != *PC_keyPressed) {
				xQueueSend(getUART0Queue(), (void * )&PC_keyPressed,
						QUEUE_WAITTIME);
			}
			UARTPC_Printf_Key(PC_keyPressed);

			/* The value of the Queue is recovered */
			xQueueReceive(getUART0Queue(), &dataOutput, (TickType_t ) 0);

			/* The value of the Queue is send to the Nokia screen */
			LCDNokia_sendChar(dataOutput);

			/* We check if the key pressed was enter */
			if (*PC_keyPressed == ENTER_KEY) {
				uart0_recieve = NEW_LINE;
				UARTBT_Printf_Key(&uart0_recieve);
				UARTBT_Printf_Key(terminal1);
			}

			if (ESC == *PC_keyPressed) {
				PC_printed = FALSE;
				Task_NINE = TRUE;
				xSemaphoreGive(xMenu_SemaphoreHandler);
				LCDNokia_clear();
				vTaskResume(getPrintLCDTask_Handler());
				vTaskSuspend(NULL);
			}
		}
	}
}

void RTC_Hour_Init(){
	format24h = FALSE;
	formatAM = TRUE;
	uint8_t readBuff[MIN_BUFFER_SIZE];

	xSemaphoreTake(getMutex(),pMAX_DELAY);
	{
		I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_HOURS_REG, MASK_INIT_HRS);
		I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_MINUTES_REG, MASK_INIT_MIN);
		I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_SECONDS_REG, MASK_INIT_SEC);

		I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_DAY_REG, MASK_INIT_DAY);
		I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_MONTH_REG, MASK_INIT_MONTH);
		I2C_WriteReg_Hex(BOARD_I2C0_BASEADDR, RTC_ADDRESS, RTC_YEAR_REG, MASK_INIT_YEAR);
	}
	xSemaphoreGive(getMutex());
}

void QueuesAndSem_Init(){
	xMenu_SemaphoreHandler = xSemaphoreCreateCounting(UARTTASK_MAXCOUNT, 0);
	xMenu_BT_SemaphoreHandler = xSemaphoreCreateCounting(UARTTASK_MAXCOUNT, 0);
	setUART0Queue(xQueueCreate(UART_QUEUE_SIZE, sizeof(QueueHandle_t)));
	setUART0Sem(xSemaphoreCreateCounting(UARTTASK_MAXCOUNT, UARTTASK_INITCOUNT));
	setUART3Queue(xQueueCreate(UART_QUEUE_SIZE, sizeof(QueueHandle_t)));
	setUART3Sem(xSemaphoreCreateCounting(UARTTASK_MAXCOUNT, UARTTASK_INITCOUNT));
}
/*******************************************************************************
 * END
 ******************************************************************************/
