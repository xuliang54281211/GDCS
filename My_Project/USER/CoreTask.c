#include "core_include.h"
#include "CoreTask.h"
Self_info_t Self_info;
report_info_t report_info;
attacker_info_t my_attacker_info, attacker_info;
u8 SysInfo[128] = {0};
u8 GameOver;
uint32_t MY_UID;

xQueueHandle xQueueCoreMsg;
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
	report_info.MY_UID = MY_UID;
	report_info.ATK_UID = 0;
	report_info.ATK_GUN = 0;
	
	//My Infra send info
	my_attacker_info.UID  = MY_UID;
	my_attacker_info.Gun = MYGUN;
	
	//other attacker info
	attacker_info.UID = 0;
	attacker_info.Gun = 0;

	begin = end = batk_array;
	
	GameOver = 0;
}

void org_report(u32 uid, u32 gun)
{
	report_info_t report_info;
	report_info.ATK_UID = uid;
	report_info.MY_UID = uid;
	switch(gun)
	{
		case USP:
			report_info.health -= USP;
			break;
		case MP5:
			report_info.health -= MP5;
			break;
		case M4:
			report_info.health -= M4;
			break;
		case AK47:
			report_info.health -= AK47;
			break;
		case AWP:
			report_info.health -= AWP;
			break;
	}
	if(report_info.health <= 0)
	{
		GameOver = 1;
	}
	report_info.health = 75;
	RF_SendPacket((u8 *)&report_info, sizeof(report_info_t));

}
void vTaskCore(void *pvParameters)
{
	attacker_info_t atk_info;
	xQueueCoreMsg = xQueueCreate(1, sizeof(u32));
	while(1)
	{
		//
		//被击中信息检测
		while(begin != end)
		{
			//扣除生命
			if(Self_info.health >= (begin->Gun))
				Self_info.health -= begin->Gun;
			else
				Self_info.health = 0;
			if(0 == Self_info.health)
			{
				GameOver = 1;//游戏结束 
				/*后续动作...*/
				//TODO
			}
			
			
			//组织report包
			report_info.health = Self_info.health;
			report_info.MY_UID = MY_UID;
			report_info.ATK_UID = begin->UID;
			report_info.ATK_GUN = begin->Gun;
			begin++;
			if(begin > &batk_array[MAX_MESSAGE])
			{
				begin  = batk_array;
			}
			if (RF_SendPacket((u8 *)&report_info, sizeof(report_info_t)))
			{
				
			}
		}
	vTaskDelay(1);
	}
}
