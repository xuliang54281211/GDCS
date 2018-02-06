#include "Infrared.h"

attacker_info_t batk_array[MAX_MESSAGE];
attacker_info_t * begin;
attacker_info_t * end;
static u32 IRA_REC; 
extern u8 InfraLen;
extern u8 Infra_recvbuf[INFRA_RECV_BUFF_SIZE];

static IRA_data_t IRA;

void IRA_Init(void)
{
	u8 i;
	IRA.data_array = 0;
	IRA.head = 0;
	IRA.high_cnt =0;
	IRA.low_cnt = 0;
	IRA.dec_len = 0;
	IRA.use_time = 0;
	IRA.mut_en = FALSE;
	IRA.mut_time = 0;
	IRA.data_no = 0;
	for(i = 0; i < MAX_MESSAGE; i++)
	{
		batk_array[i].UID = 0;
		batk_array[i].Gun = 0;
	}
	begin = batk_array;
	end = batk_array;
	
}

void IRA_DecodeHandler(u16 pulse_width)
{
	if(pulse_width & 0x8000)
	{
		/*HIGH pulse*/
		if(IRA.low_cnt > 0)
		{
			if(((IRA.high_cnt > 800)&& (IRA.high_cnt < 1000)) && ((IRA.low_cnt > 300)&& (IRA.low_cnt < 500)))
			{
				
				if(IRA.head == 0)
				{
					/*find head*/
					IRA.head = 1;
					IRA.data_no = 0;
				}
				else
				{
					
				}
			}
			else if(IRA.head && (IRA.high_cnt > 56 && IRA.low_cnt > 56))
			{
				//get 0 bit
				IRA.data_array <<= 1;
				IRA.data_no++;
				if(IRA.high_cnt > IRA.low_cnt + 200000)
					IRA.head = 0;
				
			}
			else if(IRA.head && (IRA.high_cnt > 56 && IRA.low_cnt > 160))
			{
				//get 1 bit
				IRA.data_array <<= 1;
    		IRA.data_array |= 0x01; 
				IRA.data_no++;
				if(IRA.high_cnt > IRA.low_cnt + 20000)
					IRA.head = 0;
			}
		}
		if(IRA.data_no >= 31)
		{
			IRA_REC = IRA.data_array;
			IRA_REC |= 0x80000000;
			IRA.head = 0;
			IRA.data_no = 0;
			//将接收到的信息写入被打击信息数组
			end->UID = IRA_REC & 0xffff;
			end->Gun = IRA_REC &0xf0000;
			end++;
		}
		if(IRA.low_cnt > 2500)
		IRA.low_cnt = 0;
		
		IRA.high_cnt = pulse_width & 0x7f;
	}
	else
	{
		/*LOW pulse*/
		IRA.low_cnt = pulse_width;
		if(IRA.high_cnt > 2000)
			IRA.head = 0;
			
	}
}

void vTaskBeAttack(void *pvParameters)
{
	u8 len;
	IRA_Init();
	while(1)
	{
		taskENTER_CRITICAL();
		len = InfraLen;
		taskEXIT_CRITICAL();
		/**********************************************
		接收到信号，存于Infra_revbuf[]
		***********************************************/
		while(len != IRA.dec_len)
		{
			IRA_DecodeHandler(Infra_recvbuf[IRA.dec_len]);
			
			IRA.dec_len++;
			if(IRA.dec_len >= INFRA_RECV_BUFF_SIZE)
			{
				IRA.dec_len = 0;
			}
		}
		printf("I'm Task55555555\r\n");
		vTaskDelay(500);
	}
}