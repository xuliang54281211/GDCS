#ifndef __SPI_H
#define __SPI_H
#include "hardware.h"
#include "gd32f10x.h"

void SPI2_Init(void);
u8 SPI_ExchangeByte(u8 input);


#endif
