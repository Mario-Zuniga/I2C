

#include "Delay.h"


void delay_cycles(uint32_t cycles)
{
	__asm (	"ldr r1, = 0");
	__asm (	"ldr r2, = 0xffffffff");
	__asm (	"LOOP:");
	__asm (	"adds	r0, r0, r2 ");
	__asm ("cmp		r1, r0 ");
	__asm ("bne		LOOP ");

}



void delay_msOrus(uint16_t time, uint32_t systemClock, uint8_t msOrus)
{
	uint32_t numberOfClocks = 0;

	float timeFloat  = 0;

	if(1 == msOrus)
	{
		timeFloat = (time/1000000.0);
	}
	else
	{
		timeFloat = (time/1000.0);
	}


	timeFloat = 1/timeFloat;

	numberOfClocks = systemClock/timeFloat;
	numberOfClocks = numberOfClocks >> 2;

	delay_cycles(numberOfClocks);
}
