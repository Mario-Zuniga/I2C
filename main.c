/*
 *  ABOUT
 *
 * 	Date:		Marzo del 2017
 *
 * 	Authors:	MARISCAL ARROYO JUAN PABLO
 * 				MORENO ALONZO LUIS FERNANDO
 * 				ZUÑIGA CARRILLO MARIO EUGENIO
 *
 * 	Description: Practica #1.
 *
 */

/*******************************************************************************
 * LIBRARIES
 ******************************************************************************/
#include "Definitions.h"

/*******************************************************************************
 * GLOBAL HANDLER VARIABLES
 ******************************************************************************/
TaskHandle_t xDeleteTask_Handler;
TaskHandle_t xMenuTask_Handler;
TaskHandle_t xMenu_BT_Task_Handler;

TaskHandle_t xReadMemoryTask_Handler;
TaskHandle_t xWriteMemoryTask_Handler;
TaskHandle_t xSetHourTask_Handler;
TaskHandle_t xGetHourTask_Handler;
TaskHandle_t xSetDateTask_Handler;
TaskHandle_t xGetDateTask_Handler;
TaskHandle_t xHourFormatTask_Handler;
TaskHandle_t xUART0ChatTask_Handler;
TaskHandle_t xUART3ChatTask_Handler;
TaskHandle_t xPrintLCDTask_Handler;
TaskHandle_t xBottonsTask_Handler;
TaskHandle_t xInitTask_Handler;
TaskHandle_t xECOTask_Handler;

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/
/* UART0 Variables */
uint8_t background_buffer[256];
uart_rtos_handle_t UART0_Handle;
struct _uart_handle t_UART0_Handle;
uart_rtos_config_t uart_config = {
    .baudrate = 115200,
    .parity = kUART_ParityDisabled,
    .stopbits = kUART_OneStopBit,
    .buffer = background_buffer,
    .buffer_size = sizeof(background_buffer),
};

/* UART3 Variables */
uint8_t BT_background_buffer[256];
uart_rtos_handle_t UART3_Handle;
struct _uart_handle t_UART3_Handle;
uart_rtos_config_t BT_uart_config = {
    .baudrate = 9600,
    .parity = kUART_ParityDisabled,
    .stopbits = kUART_OneStopBit,
    .buffer = BT_background_buffer,
    .buffer_size = sizeof(BT_background_buffer),
};

/* I2C Variables */
i2c_master_handle_t master_handle;		/*!< Handler de referencia para el maestros de la I2C. */
volatile bool completionFlag = false;	/*!< Bandera que indica que la transferencia termino. */
volatile bool nakFlag = false;			/*!< Bandera que se activa cuando el NACK fue recibido del esclavo. */

/* Others Variables */
static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

/*******************************************************************************
 * FUNCTION PROTOTYPES
 ******************************************************************************/
static bool I2C_Config(void);
static void i2c_master_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData);

/*******************************************************************************
 * MAIN PROGRAM
 ******************************************************************************/
/*!
 * @brief Main.
 *
 * Funcion principal del sistema. Se encarga de configurar los pines a utilizar.
 * Crea las tareas y inizializa el Scheduler...
 *
 */
int main(void) {
  /* Init board hardware. */
  BOARD_InitPins();
  BOARD_BootClockRUN();
  BOARD_I2C_ReleaseBus();
  BOARD_I2C_ConfigurePins();
  BOARD_InitDebugConsole();

  NVIC_SetPriority(PC_UART_RX_TX_IRQn, UART0_NVIC_PRIORITY);
  uart_config.srcclk = CLOCK_GetFreq(PC_UART_CLKSRC);
  uart_config.base = PC_UART;
  UART_RTOS_Init(&UART0_Handle, &t_UART0_Handle, &uart_config);

  NVIC_SetPriority(BT_UART_RX_TX_IRQn, UART3_NVIC_PRIORITY);
  BT_uart_config.srcclk = CLOCK_GetFreq(BT_UART_CLKSRC);
  BT_uart_config.base = BT_UART;
  UART_RTOS_Init(&UART3_Handle, &t_UART3_Handle, &BT_uart_config);

  I2C_MasterTransferCreateHandle(BOARD_I2C0_BASEADDR, &master_handle, i2c_master_callback, NULL);	/* Crea el handler del maestro */
  I2C_Config();
  LCDNokia_init();

  setBotton1Sem(xSemaphoreCreateCounting(BOTTONS_MAXCOUNT, BOTTONS_INITCOUNT));
  setBotton2Sem(xSemaphoreCreateCounting(BOTTONS_MAXCOUNT, BOTTONS_INITCOUNT));
  setBotton3Sem(xSemaphoreCreateCounting(BOTTONS_MAXCOUNT, BOTTONS_INITCOUNT));
  setBotton4Sem(xSemaphoreCreateCounting(BOTTONS_MAXCOUNT, BOTTONS_INITCOUNT));
  setMutex(xSemaphoreCreateMutex());

  /* Create RTOS task */
  xTaskCreate(PC_Menu_vTask, "PC_Menu_vTask", configMINIMAL_STACK_SIZE, NULL, MENU_TASK_PRIORITY, &xMenuTask_Handler);
  xTaskCreate(BT_Menu_vTask, "BT_Menu_vTask", configMINIMAL_STACK_SIZE, NULL, MENU_TASK_PRIORITY, &xMenu_BT_Task_Handler);
  xTaskCreate(Read_Memory_Task, "Read_Memory_Task", configMEDIUM_STACK_SIZE, NULL, READ_MEMORY_TASK_PRIORITY, &xReadMemoryTask_Handler);
  xTaskCreate(Write_Memory_Task, "Write_Memory_Task", configMEDIUM_STACK_SIZE, NULL, WRITE_MEMORY_TASK_PRIORITY, &xWriteMemoryTask_Handler);
  xTaskCreate(Set_Hour_Task, "Set_Hour_Task", configMEDIUM_STACK_SIZE, NULL, SET_HOUR_TASK_PRIORITY, &xSetHourTask_Handler);
  xTaskCreate(Get_Hour_Task, "Get_Hour_Task", configMINIMAL_STACK_SIZE, NULL, GET_HOUR_TASK_PRIORITY, &xGetHourTask_Handler);
  xTaskCreate(Set_Date_Task, "Set_Date_Task", configMEDIUM_STACK_SIZE, NULL, SET_DATE_TASK_PRIORITY, &xSetDateTask_Handler);
  xTaskCreate(Get_Date_Task, "Get_Date_Task", configMINIMAL_STACK_SIZE, NULL, GET_DATE_TASK_PRIORITY, &xGetDateTask_Handler);
  xTaskCreate(Hour_Format_Task, "Hour_Format_Task", configMINIMAL_STACK_SIZE, NULL, HOUR_FORMAT_TASK_PRIORITY, &xHourFormatTask_Handler);
  xTaskCreate(UART0_Chat_vTask, "UART0_Chat_vTask", configMEDIUM_STACK_SIZE, NULL, UART0_CHAT_TASK_PRIORITY, &xUART0ChatTask_Handler);
  xTaskCreate(UART3_Chat_vTask, "UART3_Chat_vTask", configMEDIUM_STACK_SIZE, NULL, UART3_CHAT_TASK_PRIORITY, &xUART3ChatTask_Handler);
  xTaskCreate(Print_LCD_task, "Print_LCD_task", configMINIMAL_STACK_SIZE, NULL, PRINT_LCD_TASK_PRIORITY, &xPrintLCDTask_Handler);
  xTaskCreate(bottons_task, "bottons_task", configMINIMAL_STACK_SIZE, NULL, BOTTONS_TASK_PRIORITY, &xBottonsTask_Handler);
  xTaskCreate(Eco_vTask, "Eco_vTask", configMEDIUM_STACK_SIZE, NULL, ECO_TASK_PRIORITY, &xECOTask_Handler);

  vTaskStartScheduler();	/* Inizializa el programador. */

  for(;;) { /* Infinite loop to avoid leaving the main function */
    __asm("NOP"); /* something to use as a breakpoint stop while looping */
  }
}

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/*!
 * @brief I2C_Config().
 *
 * Configura el maestro, establece el Baudrate y la fuente de reloj.
 * Inizializa la I2C con dicha configuracion.
 *
 */
static bool I2C_Config(void) {

    uint32_t sourceClock = 0;

    i2c_master_config_t masterConfig;
    I2C_MasterGetDefaultConfig(&masterConfig);

    masterConfig.baudRate_Bps = I2C_BAUDRATE;
    sourceClock = CLOCK_GetFreq(I2C0_CLK_SRC);

    I2C_MasterInit(BOARD_I2C0_BASEADDR, &masterConfig, sourceClock);
}

/*!
 * @brief i2c_master_callback().
 *
 * Comprueba el estado de la transferencia y activa las banderas correspondientes.
 *
 */
static void i2c_master_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData) {

    /* Signal transfer success when received success status. */
    if (status == kStatus_Success) {
        completionFlag = true;
    }
    /* Signal transfer success when received success status. */
    if ((status == kStatus_I2C_Nak) || (status == kStatus_I2C_Addr_Nak)) {
        nakFlag = true;
    }
}

/*******************************************************************************
 * GETTERS & SETTERS FUNCTIONS
 ******************************************************************************/

uint32_t getUART0Handle(){
	return (uint32_t*)&UART0_Handle;
}
uint32_t getUART3Handle(){
	return (uint32_t*)&UART3_Handle;
}
portBASE_TYPE getxHigherPriorityTaskWoken(){
	return (portBASE_TYPE*)&xHigherPriorityTaskWoken;
}
uint32_t get_I2CMasterHandle(){
	return (uint32_t*)&master_handle;
}
bool get_CompletionFlag(){
	return completionFlag;
}
void set_CompletionFlag(bool value){
	completionFlag = value;
}
bool get_nakFlag(){
	return nakFlag;
}
void set_nakFlag(bool value){
	nakFlag = value;
}
TaskHandle_t getMenuTask_Handler(){
	return xMenuTask_Handler;
}
TaskHandle_t getMenu_BT_Task_Handler(){
	return xMenu_BT_Task_Handler;
}
TaskHandle_t getReadMemoryTask_Handler(){
	return xReadMemoryTask_Handler;
}
TaskHandle_t getWriteMemoryTask_Handler(){
	return xWriteMemoryTask_Handler;
}
TaskHandle_t getSetHourTask_Handler(){
	return xSetHourTask_Handler;
}
TaskHandle_t getGetHourTask_Handler(){
	return xGetHourTask_Handler;
}
TaskHandle_t getSetDateTask_Handler(){
	return xSetDateTask_Handler;
}
TaskHandle_t getGetDateTask_Handler(){
	return xGetDateTask_Handler;
}
TaskHandle_t getHourFormatTask_Handler(){
	return xHourFormatTask_Handler;
}
TaskHandle_t getUART0CHATTask_Handler(){
	return xUART0ChatTask_Handler;
}
TaskHandle_t getUART3CHATTask_Handler(){
	return xUART3ChatTask_Handler;
}
TaskHandle_t getPrintLCDTask_Handler(){
	return xPrintLCDTask_Handler;
}
TaskHandle_t getBottonsTask_Handler(){
	return xBottonsTask_Handler;
}
TaskHandle_t getECOTask_Handler(){
	return xECOTask_Handler;
}
/*******************************************************************************
 * END
 ******************************************************************************/
