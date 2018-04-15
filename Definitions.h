/*
 *  ABOUT
 *
 * 	Date:		09 de Marzo del 2017
 *
 * 	Authors:	MARISCAL ARROYO JUAN PABLO
 * 				MORENO ALONZO LUIS FERNANDO
 * 				ZUÑIGA CARRILLO MARIO EUGENIO
 *
 * 	Description: Práctica #1
 *
 */

#ifndef SOURCE_DEFINITIONS_H_
#define SOURCE_DEFINITIONS_H_

/*******************************************************************************
 * LIBRARIES
 ******************************************************************************/
#include "stdio.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_uart_freertos.h"
#include "fsl_uart.h"
#include "fsl_device_registers.h"
#include "LCDNokia5110.h"
#include "Delay.h"

/*******************************************************************************
 * GENERAL DEFINITIONS
 ******************************************************************************/
#define TRUE 1
#define FALSE 0
#define FREE 1
#define BUSY 0
#define PC 1
#define BT 0
#define ENTER_KEY 0xD
#define NEW_LINE 0xA
#define EMPTY 0
#define EEMPTY 1
#define TEST_ARRAY_SIZE 100
#define QUEUE_WAITTIME 500
#define CREATETASK_DELAY 100
#define ONE_SECOND 1000
#define LCD_DELAY 25
#define SEM_WAITTIME ( TickType_t ) 0
#define MUTEX_WAITTIME ( TickType_t ) 0
#define pMAX_DELAY ( TickType_t ) 0xffffffffUL

/*******************************************************************************
 * BOTTONS DEFINITIONS
 ******************************************************************************/
#define BOTTONS_SEM_WAITTIME ( TickType_t ) 0
#define BOTTONS_MAXCOUNT 1
#define BOTTONS_INITCOUNT 0
#define BOTON1 0x10000
#define BOTON2 0x4
#define BOTON3 0x8
#define BOTON4 0x20000
#define DAYS 	0
#define MONTHS 	1
#define YEARS	2
#define HOURS 	3
#define MINU 	4
#define SEC 	5

/*******************************************************************************
 * TASK PRIORITY DEFINITIONS
 ******************************************************************************/
#define INIT_TASK_PRIORITY (configMAX_PRIORITIES)
#define UART0_Eco_task_PRIORITY (configMAX_PRIORITIES - 1)
#define UART0_Chat_task_PRIORITY (configMAX_PRIORITIES - 1)
#define UART3_Eco_task_PRIORITY (configMAX_PRIORITIES - 1)
#define UART3_Chat_task_PRIORITY (configMAX_PRIORITIES - 1)
#define MEMORY_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define RTC_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define PC_uart_task_PRIORITY (configMAX_PRIORITIES - 1)
#define MENU_TASK_PRIORITY (configMAX_PRIORITIES - 3)
#define BT_uart_task_PRIORITY (configMAX_PRIORITIES - 1)
#define WRITE_MEMORY_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define READ_MEMORY_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define SET_HOUR_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define GET_HOUR_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define SET_DATE_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define GET_DATE_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define HOUR_FORMAT_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define UART0_CHAT_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define UART3_CHAT_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define PRINT_LCD_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define BOTTONS_TASK_PRIORITY (configMAX_PRIORITIES - 3)
#define ECO_TASK_PRIORITY (configMAX_PRIORITIES - 1)
/*******************************************************************************
 * UART DEFINITIONS
 ******************************************************************************/
#define PC_UART UART0
#define PC_UART_CLKSRC UART0_CLK_SRC
#define PC_UART_RX_TX_IRQn UART0_RX_TX_IRQn
#define BT_UART UART3
#define BT_UART_CLKSRC UART3_CLK_SRC
#define BT_UART_RX_TX_IRQn UART3_RX_TX_IRQn
#define UART0_NVIC_PRIORITY 5
#define UART3_NVIC_PRIORITY 5
#define N_LETTERS 1
#define UARTTASK_MAXCOUNT 1
#define UARTTASK_INITCOUNT 1
#define UART_QUEUE_SIZE 35
#define BIG_BUFFER_SIZE UART_QUEUE_SIZE
#define INFO_BUFFER_SIZE BIG_BUFFER_SIZE+10
#define SMALL_BUFFER_SIZE 15
#define MIN_BUFFER_SIZE 1

/*******************************************************************************
 * I2C DEFINITIONS
 ******************************************************************************/
#define I2C_RELEASE_SDA_PORT PORTE
#define I2C_RELEASE_SCL_PORT PORTE
#define I2C_RELEASE_SDA_GPIO GPIOE
#define I2C_RELEASE_SDA_PIN 25U
#define I2C_RELEASE_SCL_GPIO GPIOE
#define I2C_RELEASE_SCL_PIN 24U
#define I2C_RELEASE_BUS_COUNT 100U
#define I2C_BAUDRATE 100000U
#define RTC_ADDRESS 0b1101111
#define MEMORY_ADDRESS 0b1010000
#define READDATA_SIZE 1
#define MASK_ST_BIT 0x7F
#define MASK_24H_SET 0x3F
#define MASK_12H_SET 0x40
#define MASK_12H_GET 0x1F
#define MASK_OFFSET_FORMAT 0x0C
#define MASK_AM_FORMAT 0xDF
#define MASK_PM_FORMAT 0x60
#define MASK_INIT_RTC 0x80
#define MASK_INIT_SEC 0x80
#define MASK_INIT_MIN 0x00
#define MASK_INIT_HRS 0x40
#define MASK_INIT_DAY 0x21
#define MASK_INIT_MONTH 0x03
#define MASK_INIT_YEAR 0x17

#define RTC_SECONDS_REG 0x00
#define RTC_MINUTES_REG 0x01
#define RTC_HOURS_REG 	0x02
#define RTC_DAY_REG 	0x04
#define RTC_MONTH_REG 	0x05
#define RTC_YEAR_REG 	0x06

/*******************************************************************************
 * MENU DEFINITIONS
 ******************************************************************************/
#define ONE 49
#define TWO 50
#define THREE 51
#define FOUR 52
#define FIVE 53
#define SIX 54
#define SEVEN 55
#define EIGHT 56
#define NINE 57
#define ESC 27

/*******************************************************************************
 * TASKS
 ******************************************************************************/
void INIT_vTask(void *pvParameter);
void PC_Menu_vTask( void *pvParameter );
void BT_Menu_vTask( void *pvParameter );
void Memory_Task( void *pvParameter );
void Write_Memory_Task( void *pvParameter );
void Read_Memory_Task( void *pvParameter );
void Set_Hour_Task( void *pvParameter );
void Get_Hour_Task( void *pvParameter );
void Set_Date_Task( void *pvParameter );
void Get_Date_Task( void *pvParameter );
void Hour_Format_Task( void *pvParameter );
void Print_LCD_task(void *pvParameters);
void UART0_Eco_vTask( void *pvParameter );
void UART3_Eco_vTask( void *pvParameter );
void UART0_Chat_vTask( void * pvParameter );
void UART3_Chat_vTask( void * pvParameter );
void bottons_task(void *pvParameters);
void Eco_vTask( void *pvParameter );

/*******************************************************************************
 * FUNCTION PROTOTYPES
 ******************************************************************************/
/* General Functions */
void bufferBackup(uint8_t *,uint8_t *);
void zerosBuffer(uint8_t *,uint8_t);
uint8_t hexToASCII(uint8_t);
uint8_t BCDtoHex(uint8_t);
uint8_t HexToBCD (uint8_t);
uint8_t DecToBCD(uint8_t);
void Print_Menu();
void QueuesAndSem_Init();

/* UART Functions */
void UARTPC_Printf_ESC();
void UARTPC_Printf_BTN();
void UARTPC_Printf_Key(uint8_t* msj);
void UARTPC_Printf_Hex(uint8_t*,uint8_t*);
void UARTPC_Printf_String(uint8_t*, uint8_t*);
void UARTPC_waitingKey(uint8_t*);

void UARTBT_Printf_ESC();
void UARTBT_Printf_BTN();
void UARTBT_Printf_Key(uint8_t* msj);
void UARTBT_Printf_Hex(uint8_t* msj, uint8_t*);
void UARTBT_Printf_String(uint8_t*, uint8_t*);
void UARTBT_waitingKey(uint8_t*);

/* I2C Functions */
bool I2C_WriteReg(I2C_Type *base, uint8_t device_addr, uint16_t reg_addr, uint8_t* value);
bool I2C_WriteReg_Hex(I2C_Type *base, uint8_t device_addr, uint16_t reg_addr, uint8_t value);
bool I2C_ReadReg(I2C_Type *base, uint8_t device_addr, uint16_t reg_addr, uint8_t *rxBuff, uint32_t rxSize);
void i2c_release_bus_delay(void);
void BOARD_I2C_ReleaseBus(void);
void MEMORY_DataInit();
void RTC_DataInit();
void RTC_Hour_Init();

/* SPI LCD Functions */
void printLCD_Numeros(uint8_t x, uint8_t y, uint8_t *var);
void printLCD_Strings(uint8_t x, uint8_t y, uint8_t *msj);
void PRINT_LCD_UART0();

/* MENU Functions */
void print_Menu_BT();
void print_Menu();
void print_PC_ERROR();
void print_BT_ERROR();

/*******************************************************************************
 * GETTERS & SETTERS FUNCTIONS
 ******************************************************************************/
/* HANDLERS G&S */
TaskHandle_t getMenuTask_Handler();
TaskHandle_t getMenu_BT_Task_Handler();
TaskHandle_t getReadMemoryTask_Handler();
TaskHandle_t getWriteMemoryTask_Handler();
TaskHandle_t getSetHourTask_Handler();
TaskHandle_t getGetHourTask_Handler();
TaskHandle_t getSetDateTask_Handler();
TaskHandle_t getGetDateTask_Handler();
TaskHandle_t getHourFormatTask_Handler();
TaskHandle_t getUART0CHATTask_Handler();
TaskHandle_t getUART3CHATTask_Handler();
TaskHandle_t getBottonsTask_Handler();
TaskHandle_t getECOTask_Handler();
TaskHandle_t getPrintLCDTask_Handler();

/* BOTTONS G&S */
SemaphoreHandle_t getBotton1Sem();
void setBotton1Sem(SemaphoreHandle_t);
SemaphoreHandle_t getBotton2Sem();
void setBotton2Sem(SemaphoreHandle_t);
SemaphoreHandle_t getBotton3Sem();
void setBotton3Sem(SemaphoreHandle_t);
SemaphoreHandle_t getBotton4Sem();
void setBotton4Sem(SemaphoreHandle_t);

/* I2C G&S */
uint32_t get_I2CMasterHandle();
bool get_CompletionFlag();
bool get_nakFlag();
SemaphoreHandle_t getMutex();
void setMutex(SemaphoreHandle_t);
//SemaphoreHandle_t getMutexBT();
//void setMutexBT(SemaphoreHandle_t);

QueueHandle_t getUART0Queue();
uint32_t getUART0Handle();
QueueHandle_t getUART3Queue();
uint32_t getUART3Handle();
SemaphoreHandle_t getUART0Sem();
SemaphoreHandle_t getUART3Sem();

void set_CompletionFlag(bool value);
void set_nakFlag(bool value);
void setUART0Queue(QueueHandle_t);
void setUART0Sem(SemaphoreHandle_t);
void setUART3Queue(QueueHandle_t);
void setUART3Sem(SemaphoreHandle_t);

/*******************************************************************************
 * END
 ******************************************************************************/
#endif /* SOURCE_DEFINITIONS_H_ */
