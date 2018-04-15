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
 * GLOBAL HANDLER VARIABLES
 ******************************************************************************/
SemaphoreHandle_t xBotton1_SemaphoreHandler = NULL;
SemaphoreHandle_t xBotton2_SemaphoreHandler = NULL;
SemaphoreHandle_t xBotton3_SemaphoreHandler = NULL;
SemaphoreHandle_t xBotton4_SemaphoreHandler = NULL;

SemaphoreHandle_t xMutexHandler;
SemaphoreHandle_t xBT_MutexHandler;

SemaphoreHandle_t xUART0_SemaphoreHandler;
SemaphoreHandle_t xUART3_SemaphoreHandler;
QueueHandle_t xUART0_QueueHandler;
QueueHandle_t xUART3_QueueHandler;

size_t n;

/*******************************************************************************
 * GLOBAL STRING VARIABLES
 ******************************************************************************/
/* MENU */
uint8_t welcome[] = "**********************************************************************\r\n\r\n*                         P R A C T I C A  1                         *\r\n\r\n**********************************************************************\r\n";
uint8_t option_1[] = "1)   Leer memoria I2C";
uint8_t option_2[] = "2)   Escribir memoria I2C";
uint8_t option_3[] = "3)   Establecer hora";
uint8_t option_4[] = "4)   Establecer fecha";
uint8_t option_5[] = "5)   Formato de hora";
uint8_t option_6[] = "6)   Leer hora";
uint8_t option_7[] = "7)   Leer fecha";
uint8_t option_8[] = "8)   Chat";
uint8_t option_9[] = "9)   Eco LCD";

/* COMMANDS */
uint8_t clear_terminal[] = "\033[2J";
uint8_t cursor_to_home[] = "\033[H";
uint8_t cursor_to_line_1[] = "\033[8;10H";
uint8_t cursor_to_line_2[] = "\033[9;10H";
uint8_t cursor_to_line_3[] = "\033[10;10H";
uint8_t cursor_to_line_4[] = "\033[11;10H";
uint8_t cursor_to_line_5[] = "\033[12;10H";
uint8_t cursor_to_line_6[] = "\033[13;10H";
uint8_t cursor_to_line_7[] = "\033[14;10H";
uint8_t cursor_to_line_8[] = "\033[15;10H";
uint8_t cursor_to_line_9[] = "\033[16;10H";
uint8_t cursor_to_lastline[] = "\033[18;10H";
uint8_t blueText[] = "\033[1;34;40m";
uint8_t whiteText[] = "\033[1;37;40m";
uint8_t yellowText[] = "\033[1;33;40m";
uint8_t redText[] = "\033[1;31;40m";

uint8_t terminal1[] = "Terminal 1: ";
uint8_t terminal2[] = "Terminal 2: ";

/* ERROR MESSAGE*/
uint8_t menssage_error_1[] = "\r\n--------> ALERTA <-------- \r\n\r\n";
uint8_t menssage_error_2[] = "Este recurso esta en uso.\r\n";


/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/*!
 * @brief i2c_release_bus_delay().
 *
 * Delay para que el bus sea liberado.
 *
 */

void i2c_release_bus_delay(void) {

    uint32_t i = 0;
    for (i = 0; i < I2C_RELEASE_BUS_COUNT; i++) {
        __NOP();
    }
}

/*!
 * @brief BOARD_I2C_ReleaseBus().
 *
 * Configura e inicializa el bus de la I2C.
 *
 */

void BOARD_I2C_ReleaseBus(void) {

    uint8_t i = 0;
    gpio_pin_config_t pin_config;
    port_pin_config_t i2c_pin_config = {0};

    /* Config pin mux as gpio */
    i2c_pin_config.pullSelect = kPORT_PullUp;
    i2c_pin_config.mux = kPORT_MuxAsGpio;

    pin_config.pinDirection = kGPIO_DigitalOutput;
    pin_config.outputLogic = 1U;
    PORT_SetPinConfig(I2C_RELEASE_SCL_PORT, I2C_RELEASE_SCL_PIN, &i2c_pin_config);
    PORT_SetPinConfig(I2C_RELEASE_SCL_PORT, I2C_RELEASE_SDA_PIN, &i2c_pin_config);

    GPIO_PinInit(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, &pin_config);
    GPIO_PinInit(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, &pin_config);

    /* Drive SDA low first to simulate a start */
    GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    /* Send 9 pulses on SCL and keep SDA low */
    for (i = 0; i < 9; i++) {

        GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
        i2c_release_bus_delay();

        GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
        i2c_release_bus_delay();

        GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
        i2c_release_bus_delay();
        i2c_release_bus_delay();
    }

    /* Send stop */
    GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
    i2c_release_bus_delay();

    GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
    i2c_release_bus_delay();
}

/*!
 * @brief I2C_WriteReg().
 *
 * Escribe en el dispositivo I2C correspondiente, en el registro correspondiente un valor determinado.
 *
 */

bool I2C_WriteReg(I2C_Type *base, uint8_t device_addr, uint16_t reg_addr, uint8_t* value) {
		i2c_master_transfer_t masterXfer;
		memset(&masterXfer, 0, sizeof(masterXfer));

		masterXfer.slaveAddress = device_addr;
		masterXfer.direction = kI2C_Write;
		masterXfer.data = value;
		masterXfer.dataSize = strlen(value);
		masterXfer.flags = kI2C_TransferDefaultFlag;
		masterXfer.subaddress = reg_addr;

		if(MEMORY_ADDRESS == device_addr){
			masterXfer.subaddressSize = 2;
		}else if(RTC_ADDRESS == device_addr){
			masterXfer.subaddress = (uint8_t) reg_addr;
			masterXfer.subaddressSize = 1;
		}
		I2C_MasterTransferNonBlocking(BOARD_I2C0_BASEADDR, get_I2CMasterHandle(), &masterXfer);
		vTaskDelay(100);
		/*  wait for transfer completed. */
		while ((!get_nakFlag()) && (!get_CompletionFlag())) {
		}

		set_nakFlag(false);

		if (get_CompletionFlag() == true) {
			set_CompletionFlag(false);
			return true;
		} else {
			return false;

		}
}
bool I2C_WriteReg_Hex(I2C_Type *base, uint8_t device_addr, uint16_t reg_addr, uint8_t value) {
		i2c_master_transfer_t masterXfer;
		memset(&masterXfer, 0, sizeof(masterXfer));

		masterXfer.slaveAddress = device_addr;
		masterXfer.direction = kI2C_Write;
		masterXfer.data = &value;
		masterXfer.dataSize = 1;
		masterXfer.flags = kI2C_TransferDefaultFlag;
		masterXfer.subaddress = reg_addr;

		if(MEMORY_ADDRESS == device_addr){
			masterXfer.subaddressSize = 2;
		}else if(RTC_ADDRESS == device_addr){
			masterXfer.subaddress = (uint8_t) reg_addr;
			masterXfer.subaddressSize = 1;
		}
		I2C_MasterTransferNonBlocking(BOARD_I2C0_BASEADDR, get_I2CMasterHandle(), &masterXfer);
		vTaskDelay(100);
		/*  wait for transfer completed. */
		while ((!get_nakFlag()) && (!get_CompletionFlag())) {
		}

		set_nakFlag(false);

		if (get_CompletionFlag() == true) {
			set_CompletionFlag(false);
			return true;
		} else {
			return false;

		}
}

/*!
 * @brief I2C_ReadReg().
 *
 * Lee en el dispositivo I2C correspondiente, un registro determinado y lo almacena en una variable especificada.
 *
 */
bool I2C_ReadReg(I2C_Type *base, uint8_t device_addr, uint16_t reg_addr, uint8_t *rxBuff, uint32_t rxSize) {
		i2c_master_transfer_t masterXfer;
		memset(&masterXfer, 0, sizeof(masterXfer));
		masterXfer.slaveAddress = device_addr;
		masterXfer.direction = kI2C_Read;
		masterXfer.data = rxBuff;
		masterXfer.dataSize = rxSize;
		masterXfer.flags = kI2C_TransferDefaultFlag;
		masterXfer.subaddress = reg_addr;

		if(MEMORY_ADDRESS == device_addr){
			masterXfer.subaddressSize = 2;
		}else if(RTC_ADDRESS == device_addr){
			masterXfer.subaddress = (uint8_t) reg_addr;
			masterXfer.subaddressSize = 1;
		}
		I2C_MasterTransferNonBlocking(BOARD_I2C0_BASEADDR, get_I2CMasterHandle(), &masterXfer);
		vTaskDelay(100);
		/*  wait for transfer completed. */
		while ((!get_nakFlag()) && (!get_CompletionFlag())) {
		}

		set_nakFlag(false);

		if (get_CompletionFlag() == true) {
			set_CompletionFlag(false);
			return true;
		} else {
			return false;
		}
}

void UARTPC_Printf_Key(uint8_t* msj){
	if(NULL != msj){
		UART_RTOS_Send((uart_rtos_handle_t *)getUART0Handle(), (uint8_t *)msj, N_LETTERS);
	}
}

void UARTBT_Printf_Key(uint8_t* msj){
	if(NULL != msj){
		UART_RTOS_Send((uart_rtos_handle_t *)getUART3Handle(), (uint8_t *)msj, N_LETTERS);
	}
}

void UARTPC_Printf_Hex(uint8_t* msj, uint8_t* var){
	if(NULL != msj){
		UART_RTOS_Send((uart_rtos_handle_t *)getUART0Handle(), (uint8_t *)msj, strlen(msj));
	}
	if(NULL != var){
		char string[3] = {hexToASCII(*var/0x10), hexToASCII(*var%0x10),'\0'};
		uint8_t *stringPtr;
		stringPtr = string;
		UART_RTOS_Send((uart_rtos_handle_t *)getUART0Handle(), (uint8_t *)stringPtr, strlen(stringPtr));
	}
}

void UARTBT_Printf_Hex(uint8_t* msj, uint8_t* var){
	if(NULL != msj){
		UART_RTOS_Send((uart_rtos_handle_t *)getUART3Handle(), (uint8_t *)msj, strlen(msj));
	}
	if(NULL != var){
		char string[3] = {hexToASCII(*var/0x10), hexToASCII(*var%0x10),'\0'};
		uint8_t *stringPtr;
		stringPtr = string;
		UART_RTOS_Send((uart_rtos_handle_t *)getUART3Handle(), (uint8_t *)stringPtr, strlen(stringPtr));
	}
}


void UARTPC_Printf_String(uint8_t* msj, uint8_t* string){
	if(NULL != msj){
		UART_RTOS_Send((uart_rtos_handle_t *)getUART0Handle(), (uint8_t *)msj, strlen(msj));
	}
	if(NULL != string){
		UART_RTOS_Send((uart_rtos_handle_t *)getUART0Handle(), (uint8_t *)string, strlen(string));
	}
}

void UARTBT_Printf_String(uint8_t* msj, uint8_t* string){
	if(NULL != msj){
		UART_RTOS_Send((uart_rtos_handle_t *)getUART3Handle(), (uint8_t *)msj, strlen(msj));
	}
	if(NULL != string){
		UART_RTOS_Send((uart_rtos_handle_t *)getUART3Handle(), (uint8_t *)string, strlen(string));
	}
}

uint8_t hexToASCII(uint8_t n)
{
    if (n < 10) {
        return n + '0';
    } else {
        return (n - 10) + 'A';
    }
}

uint8_t BCDtoHex(uint8_t bcd)
{
	uint8_t dec=0;
	uint8_t mult;
    for (mult=1; bcd; bcd=bcd>>4,mult*=10)
    {
        dec += (bcd & 0x0f) * mult;
    }
    return dec;
}

uint8_t HexToBCD (uint8_t hex)
{
	uint8_t bcd;
	bcd = (hex / 10) << 4;
	bcd = bcd | (hex % 10);
    return bcd;
}

uint8_t DecToBCD(uint8_t dec){
	uint8_t total = 0;
	uint8_t resultbase = 1;
	while(dec > 0 ){
		total += resultbase * (dec % 10);
		resultbase *= 16;
		dec /= 10;
	}
	return total;
}

void zerosBuffer(uint8_t * buffer,  uint8_t nElements){
	for(uint8_t i = 0;i<nElements;i++){
		buffer[i] = 0;
	}
}

void printLCD_Numeros(uint8_t x, uint8_t y, uint8_t *var) {
	if (NULL != var) {
		char string[3] = { hexToASCII(*var / 0x10), hexToASCII(*var % 0x10),'\0' };
		uint8_t *stringPtr;
		stringPtr = string;
		LCDNokia_gotoXY(x, y);
		LCDNokia_sendString(stringPtr);
		vTaskDelay(LCD_DELAY);
	}
}

void printLCD_Strings(uint8_t x, uint8_t y, uint8_t *msj){
	if(NULL != msj){
		LCDNokia_gotoXY(x,y);
		LCDNokia_sendString(msj);
		vTaskDelay(LCD_DELAY);
	}
}

void PRINT_LCD_UART0(){
	/* Both the terminal and Nokia screen are cleaned and prepared for the user */
		UART_RTOS_Send((uart_rtos_handle_t *)getUART0Handle(), clear_terminal, strlen(clear_terminal));
		UART_RTOS_Send((uart_rtos_handle_t *)getUART0Handle(), cursor_to_home, strlen(cursor_to_home));
//		vTaskDelay(100);
		UART_RTOS_Send((uart_rtos_handle_t *)getUART0Handle(), terminal1, strlen(terminal1));
}

void UARTPC_waitingKey(uint8_t* buffer){
	UART_RTOS_Receive((uart_rtos_handle_t *)getUART0Handle(), buffer, N_LETTERS, &n);
}

void UARTBT_waitingKey(uint8_t* buffer){
	UART_RTOS_Receive((uart_rtos_handle_t *)getUART3Handle(), buffer, N_LETTERS, &n);
}

void print_PC_ERROR(){
	UARTPC_Printf_Hex(clear_terminal,NULL);
	UARTPC_Printf_Hex(cursor_to_home,NULL);
	UARTPC_Printf_Hex(yellowText,NULL);
	UARTPC_Printf_Hex(menssage_error_1,NULL);
	UARTPC_Printf_Hex(whiteText,NULL);
	UARTPC_Printf_Hex(menssage_error_2,NULL);
    UARTPC_Printf_String("Presione ",NULL);
    UARTPC_Printf_Hex(redText,NULL);
    UARTPC_Printf_String("[ESC]",NULL);
    UARTPC_Printf_Hex(whiteText,NULL);
    UARTPC_Printf_String(" para regresar.",NULL);
}

void print_BT_ERROR(){
	UARTBT_Printf_Hex(clear_terminal,NULL);
	UARTBT_Printf_Hex(cursor_to_home,NULL);
	UARTBT_Printf_Hex(yellowText,NULL);
	UARTBT_Printf_Hex(menssage_error_1,NULL);
	UARTBT_Printf_Hex(whiteText,NULL);
	UARTBT_Printf_Hex(menssage_error_2,NULL);
    UARTBT_Printf_String("Presione ",NULL);
    UARTBT_Printf_Hex(redText,NULL);
    UARTBT_Printf_String("[ESC]",NULL);
    UARTBT_Printf_Hex(whiteText,NULL);
    UARTBT_Printf_String(" para regresar.",NULL);
}

void print_Menu(){
	UARTPC_Printf_Hex(clear_terminal,NULL);
	UARTPC_Printf_Hex(cursor_to_home,NULL);
	UARTPC_Printf_Hex(whiteText,NULL);
	UARTPC_Printf_Hex(welcome,NULL);
	UARTPC_Printf_Hex(yellowText,NULL);
	UARTPC_Printf_Hex(cursor_to_line_1,NULL);
	UARTPC_Printf_Hex(option_1,NULL);
	UARTPC_Printf_Hex(cursor_to_line_2,NULL);
	UARTPC_Printf_Hex(option_2,NULL);
	UARTPC_Printf_Hex(cursor_to_line_3,NULL);
	UARTPC_Printf_Hex(option_3,NULL);
	UARTPC_Printf_Hex(cursor_to_line_4,NULL);
	UARTPC_Printf_Hex(option_4,NULL);
	UARTPC_Printf_Hex(cursor_to_line_5,NULL);
	UARTPC_Printf_Hex(option_5,NULL);
	UARTPC_Printf_Hex(cursor_to_line_6,NULL);
	UARTPC_Printf_Hex(option_6,NULL);
	UARTPC_Printf_Hex(cursor_to_line_7,NULL);
	UARTPC_Printf_Hex(option_7,NULL);
	UARTPC_Printf_Hex(cursor_to_line_8,NULL);
	UARTPC_Printf_Hex(option_8,NULL);
	UARTPC_Printf_Hex(cursor_to_line_9,NULL);
	UARTPC_Printf_Hex(option_9,NULL);
	UARTPC_Printf_Hex(cursor_to_lastline,NULL);
}

void print_Menu_BT(){
	UARTBT_Printf_Hex(clear_terminal,NULL);
	UARTBT_Printf_Hex(cursor_to_home,NULL);
	UARTBT_Printf_Hex(whiteText,NULL);
	UARTBT_Printf_Hex(welcome,NULL);
	UARTBT_Printf_Hex(yellowText,NULL);
	UARTBT_Printf_Hex(cursor_to_line_1,NULL);
	UARTBT_Printf_Hex(option_1,NULL);
	UARTBT_Printf_Hex(cursor_to_line_2,NULL);
	UARTBT_Printf_Hex(option_2,NULL);
	UARTBT_Printf_Hex(cursor_to_line_3,NULL);
	UARTBT_Printf_Hex(option_3,NULL);
	UARTBT_Printf_Hex(cursor_to_line_4,NULL);
	UARTBT_Printf_Hex(option_4,NULL);
	UARTBT_Printf_Hex(cursor_to_line_5,NULL);
	UARTBT_Printf_Hex(option_5,NULL);
	UARTBT_Printf_Hex(cursor_to_line_6,NULL);
	UARTBT_Printf_Hex(option_6,NULL);
	UARTBT_Printf_Hex(cursor_to_line_7,NULL);
	UARTBT_Printf_Hex(option_7,NULL);
	UARTBT_Printf_Hex(cursor_to_line_8,NULL);
	UARTBT_Printf_Hex(option_8,NULL);
	UARTBT_Printf_Hex(cursor_to_line_9,NULL);
	UARTBT_Printf_Hex(option_9,NULL);
	UARTBT_Printf_Hex(cursor_to_lastline,NULL);
}

void UARTPC_Printf_ESC(){
	UARTPC_Printf_Hex(whiteText,NULL);
    UARTPC_Printf_String("\r\n\r\nPresione ",NULL);
    UARTPC_Printf_Hex(redText,NULL);
    UARTPC_Printf_String("[ESC]",NULL);
    UARTPC_Printf_Hex(whiteText,NULL);
    UARTPC_Printf_String(" para salir...",NULL);
}

void UARTBT_Printf_ESC(){
	UARTBT_Printf_Hex(whiteText,NULL);
    UARTBT_Printf_String("\r\n\r\nPresione ",NULL);
    UARTBT_Printf_Hex(redText,NULL);
    UARTBT_Printf_String("[ESC]",NULL);
    UARTBT_Printf_Hex(whiteText,NULL);
    UARTBT_Printf_String(" para salir...",NULL);
}

void UARTPC_Printf_BTN(){
	UARTPC_Printf_Hex(whiteText,NULL);
    UARTPC_Printf_String("\r\n\r\nPresione ",NULL);
    UARTPC_Printf_Hex(redText,NULL);
    UARTPC_Printf_String("[BOTON 1]",NULL);
    UARTPC_Printf_Hex(whiteText,NULL);
    UARTPC_Printf_String(" para salir...",NULL);
}

void UARTBT_Printf_BTN(){
	UARTBT_Printf_Hex(whiteText,NULL);
    UARTBT_Printf_String("\r\n\r\nPresione ",NULL);
    UARTBT_Printf_Hex(redText,NULL);
    UARTBT_Printf_String("[BOTON 1]",NULL);
    UARTBT_Printf_Hex(whiteText,NULL);
    UARTBT_Printf_String(" para salir...",NULL);
}
/*******************************************************************************
 * GETTERS & SETTERS FUNCTIONS
 ******************************************************************************/

QueueHandle_t getUART0Queue(){
	return xUART0_QueueHandler;
}
void setUART0Queue(QueueHandle_t queueHandler){
	xUART0_QueueHandler = queueHandler;
}
SemaphoreHandle_t getUART0Sem(){
	return xUART0_SemaphoreHandler;
}
void setUART0Sem(SemaphoreHandle_t semHandler){
	xUART0_SemaphoreHandler = semHandler;
}
QueueHandle_t getUART3Queue(){
	return xUART3_QueueHandler;
}
void setUART3Queue(QueueHandle_t queueHandler){
	xUART3_QueueHandler = queueHandler;
}
SemaphoreHandle_t getUART3Sem(){
	return xUART3_SemaphoreHandler;
}
void setUART3Sem(SemaphoreHandle_t semHandler){
	xUART3_SemaphoreHandler = semHandler;
}
SemaphoreHandle_t getBotton1Sem(){
	return xBotton1_SemaphoreHandler;
}
void setBotton1Sem(SemaphoreHandle_t sem){
	xBotton1_SemaphoreHandler = sem;
}
SemaphoreHandle_t getBotton2Sem(){
	return xBotton2_SemaphoreHandler;
}
void setBotton2Sem(SemaphoreHandle_t sem){
	xBotton2_SemaphoreHandler = sem;
}
SemaphoreHandle_t getBotton3Sem(){
	return xBotton3_SemaphoreHandler;
}
void setBotton3Sem(SemaphoreHandle_t sem){
	xBotton3_SemaphoreHandler = sem;
}
SemaphoreHandle_t getBotton4Sem(){
	return xBotton4_SemaphoreHandler;
}
void setBotton4Sem(SemaphoreHandle_t sem){
	xBotton4_SemaphoreHandler = sem;
}
SemaphoreHandle_t getMutex(){
	return xMutexHandler;
}
void setMutex(SemaphoreHandle_t mutex){
	xMutexHandler = mutex;
}
//portBASE_TYPE getxHigherPriorityTaskWoken(){
//	return (portBASE_TYPE*)&xHigherPriorityTaskWoken;
//}

/*******************************************************************************
 * END
 ******************************************************************************/
