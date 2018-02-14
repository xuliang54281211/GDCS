#ifndef __INFRARED_H
#define __INFRARED_H
#include "core_include.h"

#define INFRA_RECV_BUFF_SIZE 40
typedef struct{
		u32 data_array;
		u32 prev_data_array;
		u8 data_no;
		u8 use_time;
		u8 dec_len;
		u32 high_cnt;
		u32 low_cnt;
		u8 head;
		bool mut_en;
		u16 mut_time;
}IRA_data_t;
void vTaskBeAttack(void *pvParameters);



#endif
