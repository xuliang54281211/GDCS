#ifndef OSINCLUDE_H
#define OSINCLUDE_H
#include "sys.h"
/**********STDLIB******/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/**********OS**********/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"
#include "event_groups.h"

/*********Hardware********/
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "w25qxx.h"	 
#include "CC1101.H" 
#include "exti.h"
#include "timer.h"

typedef struct
{
	u8 keyvalue;
}KeyMsg_t;
typedef struct
{
	u8 wirevalue;
}WirelessMsg_t;
#endif
