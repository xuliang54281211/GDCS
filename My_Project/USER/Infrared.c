#include "Infrared.h"

attacker_info_t batk_array[MAX_MESSAGE];
attacker_info_t * begin;
attacker_info_t * end;
static u32 IRA_REC; 
extern u8 InfraLen;
extern u32 Infra_recvbuf[INFRA_RECV_BUFF_SIZE];

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
u32 temp;
void IRA_DecodeHandler(u32 pulse_width)
{
	temp = pulse_width;
	if(!(pulse_width & 0x8000))
	{
		/*LOW pulse*/
		if(IRA.high_cnt > 0)
		{
			if(((IRA.low_cnt > 800)&& (IRA.low_cnt < 1000)) && ((IRA.high_cnt > 300)&& (IRA.high_cnt < 500)))
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
			else if((IRA.low_cnt > 450)&& (IRA.low_cnt < 750))
			{
				if(IRA.head == 1)//�������
				{
					IRA.head = 0;
					IRA.data_array |= 0x80000000;
				}
				else
					IRA.data_array = 0;
			}
			else if(IRA.head && (((IRA.low_cnt > 45) &&(IRA.low_cnt <75))  && (IRA.high_cnt > 45 && IRA.high_cnt < 75)))
			{
				//get 0 bit
				IRA.data_array <<= 1;
				IRA.data_no++;
				if(IRA.low_cnt > IRA.high_cnt + 200000)
					IRA.head = 0;
				
			}
			else if(IRA.head && (((IRA.low_cnt > 45) &&(IRA.low_cnt <75))  && (IRA.high_cnt > 130 && IRA.high_cnt < 210)))
			{
				//get 1 bit
				IRA.data_array <<= 1;
    		IRA.data_array |= 0x01; 
				IRA.data_no++;
				if(IRA.low_cnt > IRA.high_cnt + 20000)
					IRA.head = 0;
			}
		}
//		if(IRA.data_no >= 31)
//		{
//			IRA_REC = IRA.data_array;
//			IRA_REC |= 0x80000000;
//			IRA.head = 0;
//			IRA.data_no = 0;
//			//�����յ�����Ϣд�뱻�����Ϣ����
//			end->UID = IRA_REC & 0xffff;
//			end->Gun = IRA_REC &0xf0000;
//			end++;
//		}
		if(IRA.high_cnt > 2500)
		IRA.high_cnt = 0;
		
		IRA.low_cnt = pulse_width;
	}
	else
	{
		/*HIGH pulse*/
		IRA.high_cnt = pulse_width & 0x7fff;
		if(IRA.low_cnt > 2000)
			IRA.head = 0;
			
	}
}
extern u8 IraSta;
extern u32 IraRec;
extern u8 IraCnt;
u8 Remote_Scan(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(IraSta&(1<<6))//�õ�һ��������������Ϣ��
	{ 
	    t1=IraRec>>24;			//�õ���ַ��
	    t2=(IraRec>>16)&0xff;	//�õ���ַ���� 
 	    if(t1==((u8)~t2))//����ң��ʶ����(ID)����ַ 
	    { 
	        t1=IraRec>>8;
	        t2=IraRec; 	
	        if(t1==(u8)~t2)sta=t1;//��ֵ��ȷ	 
		}   
		if((sta==0)||((IraSta&0X80)==0))//�������ݴ���/ң���Ѿ�û�а�����
		{
		 	IraSta&=~(1<<6);//������յ���Ч������ʶ
			IraCnt=0;		//�����������������
		}
	}  
    return sta;
}

void vTaskBeAttack(void *pvParameters)
{
	u32 uid, gun;
	u8 len;
	IRA_Init();
	while(1)
	{
		taskENTER_CRITICAL();
		len = InfraLen;
		taskEXIT_CRITICAL();
		/**********************************************
		���յ��źţ�����Infra_revbuf[]
		***********************************************/
		while(len != IRA.dec_len)
		{
			IRA_DecodeHandler(Infra_recvbuf[IRA.dec_len]);
			if(IRA.data_array & 0x80000000)//�յ�һ������
			{
				uid = IRA.data_array & 0xffff;
				gun = (IRA.data_array >> 16) & 0xff;
				IRA.data_array = 0;
			}
			IRA.dec_len++;
			if(IRA.dec_len >= INFRA_RECV_BUFF_SIZE)
			{
				IRA.dec_len = 0;
			}
		}

		//printf("I'm Task55555555\r\n");
		vTaskDelay(10);
	
	}
}
