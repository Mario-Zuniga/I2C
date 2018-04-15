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

#ifndef SOURCES_DELAY_H_
#define SOURCES_DELAY_H_

#include "stdint.h"
//#include "DataTypeDefinitions.h"

void delay(uint16_t delay);
//void clearBuffer(uint8_t*,uint_t8);


void delay_msOrus(uint16_t time, uint32_t systemClock, uint8_t msOrus);
void delay_cycles(uint32_t cycles);



#endif /* SOURCES_DELAY_H_ */
