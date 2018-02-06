#include "core_include.h"
#include "CoreTask.h"
Self_info_t Self_info;
report_info_t report_info;
attacker_info_t my_attacker_info, attacker_info;
u8 SysInfo[128] = {0};
u8 GameOver;
u32 MYUID;


extern attacker_info_t batk_array[MAX_MESSAGE];
extern attacker_info_t * begin;
extern attacker_info_t * end;
void CoreInit(void)
{
	//TODO
	Self_info.Gun = MYGUN;
	Self_info.health = 100;
	Self_info.level = 1;
	
	//report info
	report_info.health = Self_info.health;
	report_info.MY_UID = MYUID;
	report_info.ATK_UID = 0;
	
	//My Infra send info
	my_attacker_info.UID  = MYUID;
	my_attacker_info.Gun = MYGUN;
	
	//other attacker info
	attacker_info.UID = 0;
	attacker_info.Gun = 0;
	
	GameOver = 1;
	
}
void vTaskCore(void *pvParameters)
{
	while(1)
	{
		while(begin != end)
		{
			//扣除生命
			if(Self_info.health >= (begin->Gun))
				Self_info.health -= begin->Gun;
			else
				Self_info.health = 0;
			if(0 == Self_info.health)
			{
				GameOver = 0;//游戏结束 
				/*后续动作...*/
			}
			//组织report包
			report_info.health = Self_info.health;
			report_info.MY_UID = MYUID;
			report_info.ATK_UID = begin->UID;
			if (RF_SendPacket((u8 *)&report_info, sizeof(report_info_t)))
			{
				
			}
			
			begin++;
		}
		
	printf("I'm Task33333\r\n");
	vTaskDelay(10);
	}
}
