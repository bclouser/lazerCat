#ifndef LAZER_H
#define LAZER_H

#include "ets_sys.h"


#define LAZER_IO_MUX     PERIPHS_IO_MUX_GPIO4_U
#define LAZER_IO_NUM     4
#define LAZER_IO_FUNC    FUNC_GPIO4


void initLazer();
void turnLazerOn();
void turnLazerOff();
bool isLazerOn();


#endif