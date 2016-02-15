
#include "lazer.h"
#include "os_type.h"
#include "osapi.h"
#include "gpio.h"

/*
Arguably we don't even need a module for this because it is so simple.
But cmon, "lazer.c" just sounds awesome!
*/

static bool lazerOn = false;

void initLazer()
{
	//Set GPIO4 to output mode
	PIN_FUNC_SELECT(LAZER_IO_MUX, LAZER_IO_FUNC);
	// Turn lazer off initially (set gpio4 to low)
	GPIO_OUTPUT_SET(GPIO_ID_PIN(LAZER_IO_NUM), 0);
	lazerOn = false;
}

void turnLazerOn()
{
	GPIO_OUTPUT_SET(GPIO_ID_PIN(LAZER_IO_NUM), 1);
	lazerOn = true;
}

void turnLazerOff()
{
	GPIO_OUTPUT_SET(GPIO_ID_PIN(LAZER_IO_NUM), 0);
	lazerOn = false;
}

bool isLazerOn()
{
	return lazerOn;
}

