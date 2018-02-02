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

/************hardware************/
#include "systick.h"
#include "timer.h"
#include "gpio.h"
#include "CoreTask.h"
/***********System Struct**********/
#define MYGUN 1

typedef struct
{
   u32 time;
   u8 health;
   u8 MY_UID[8];
	 u8 ATK_UID[8];
   u8 id_info;
}beattacked_info_t;

typedef enum
{
  attacker = 0,
  beacked,
}identity;

typedef enum
{
   USP = 0x1,
   MP5,
   M4,
   AK47,
   AWP,
}Gun;

typedef struct
{
  u8 Gun;
	u8 health;
  u8 level;
}Sys_info_t;

#endif

