#ifndef _CORE_INCLUDE_H
#define _CORE_INCLUDE_H
/*
*********************************************************************************************************
*                                         标准库
*********************************************************************************************************
*/
#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>
/**********OS**********/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"
#include "event_groups.h"

/*******hardware*********/
#include "gd32f10x.h"
#include "gd32f10x_eval.h"
/*
*********************************************************************************************************
*                                           宏定义
*********************************************************************************************************
*/

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
#include <string.h>
/************hardware************/
#include "systick.h"
#include "timer.h"
#include "gpio.h"
#include "CoreTask.h"
#include "Infrared.h"
#include "hardware.h"
#include "RFID.h"
/***********System Struct**********/
#define MYGUN 1
#define MAX_MESSAGE 10
#define TESTUID 0x14123a
typedef struct
{
   u32 MY_UID;
	 u32 ATK_UID;
	 u32 ATK_GUN;
	 u8 health;
   u8 id_info;
}report_info_t;

typedef struct
{
   u32 UID;
   u8 Gun;
}attacker_info_t;

typedef enum
{
  attacker = 0,
  beacked,
}identity;

typedef enum
{
   USP = 15,
   MP5 = 20,
   M4 = 25,
   AK47 = 35,
   AWP = 100,
}Gun;

typedef struct
{
  u8 Gun;
	u8 health;
  u8 level;
}Self_info_t;


#endif

