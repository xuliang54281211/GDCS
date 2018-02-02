#include "CoreTask.h"
Sys_info_t Sys_info;
beattacked_info_t beattacked_info;
u8 SysInfo[128] = {0};
void CoreInit(void)
{
	//TODO
	Sys_info.Gun = MYGUN;
	Sys_info.health = 100;
	Sys_info.level = 1;
	
	//send info
	beattacked_info.health = Sys_info.health;
	beattacked_info.time = 0;
	beattacked_info.id_info = 0;
	
}
