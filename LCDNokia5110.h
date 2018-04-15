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

#ifndef LCDNOKIA5110_H_
#define LCDNOKIA5110_H_

#include "stdint.h"
#include "fsl_gpio.h"
#include "fsl_dspi.h"

#define SCREENW 84
#define SCREENH 48

#define DSPI_MASTER_BASEADDR SPI0
#define DSPI_MASTER_CLK_SRC DSPI0_CLK_SRC
#define DSPI_MASTER_PCS_FOR_INIT kDSPI_Pcs0
#define DSPI_MASTER_PCS_FOR_TRANSFER kDSPI_MasterPcs0

#define TRANSFER_SIZE 1
#define TRANSFER_BAUDRATE 9600U /*! Transfer baudrate*/

#define LCD_X 84
#define LCD_Y 48
#define LCD_DATA 1
#define LCD_CMD 0
#define DATA_OR_CMD_PIN 3
#define RESET_PIN 0
/*It configures the LCD*/
void LCDNokia_init(void);
/*It writes a byte in the LCD memory. The place of writing is the last place that was indicated by LCDNokia_gotoXY. In the reset state
 * the initial place is x=0 y=0*/
void LCDNokia_writeByte(uint8_t, uint8_t);
/*it clears all the figures in the LCD*/
void LCDNokia_clear(void);
/*It is used to indicate the place for writing a new character in the LCD. The values that x can take are 0 to 84 and y can take values
 * from 0 to 5*/
void LCDNokia_gotoXY(uint8_t x, uint8_t y);
/*It allows to write a figure represented by constant array*/
void LCDNokia_bitmap(const uint8_t*);
/*It write a character in the LCD*/
void LCDNokia_sendChar(uint8_t);
/*It write a string into the LCD*/
void LCDNokia_sendString(uint8_t*);
/*It used in the initialization routine*/
void LCD_delay(void);



#endif /* LCDNOKIA5110_H_ */
