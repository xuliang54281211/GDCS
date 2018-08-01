/*!
    \file  main.c
    \brief TIMER trigger injected channel of ADC demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f10x.h"
#include "main.h"
#include "gd32f10x_eval.h"
#include "core_include.h"
#include "hardware.h"
#define ATTACK_FRE 480
/**********ȫ�ֱ���************/

u8	GameBegin;

/******************************/



void rcu_config(void);
void adc_config(void);
xQueueHandle xQueueKeyMsg = NULL;
xQueueHandle xQueueInfraredMsg = NULL;
void led_spark(void)
{
    static __IO uint32_t timingdelaylocal = 0U;

    if(timingdelaylocal){

        if(timingdelaylocal < 500U){
            gd_eval_led_on(LED1);
        }else{
            gd_eval_led_off(LED1);
        }

        timingdelaylocal--;
    }else{
        timingdelaylocal = 1000U;
    }
}
extern u8 timer2_modulate;
extern u8 attack_flag;
extern u32 audio_bit;
extern u8 GameOver;
u8 SendBUF[] = "Hello my CS!";
void vTaskAttack(void *pvParameters)
{
	while(1)
	{
		if(!GameOver)
		{
			
			if(gd_eval_key_state_get(KEYIN_IO) == RESET)
			{
				vTaskDelay(20/portTICK_RATE_MS);
				if(gd_eval_key_state_get(KEYIN_IO) == RESET)
				{
					
					#if 1
			timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,94);//38Khz�ز�
			timer2_modulate = 1;
			/*timer_enable(TIMER2)*/;//�����������
			//timer_enable(TIMER3);//60KHz��Ƶ�ز�
			timer_enable(TIMER0);//11.025kHz����
			vTaskDelay(ATTACK_FRE/portTICK_RATE_MS);
					#endif
					
//			if (RF_SendPacket(SendBUF, strlen((char *)SendBUF)))
//			{
//				LED_OP(1, 0);
//			}
				}
			}
		}
		vTaskDelay(1);
	}
}


void vTaskAudio(void *pvParameters)
{
	
}

extern u32 ctl_flag;
extern uint32_t MY_UID;
u8 end_zero, end_one;
u32 sendbuf[20] = {0};
void vTaskInfrared(void *pvParameters)
{
	u32 sendbufi = 0;
	u32 j = 0;
	u32 count = 300;
	u8 codei = 32;
	u8 bit_count = 0;
	u32 uid_temp = (MY_UID << 2) | MYGUN;
	xQueueInfraredMsg = xQueueCreate(1, sizeof(u32));
	while(1)
	{
		if(xQueueReceive(xQueueInfraredMsg, &count, 0) == pdPASS)
		{
			//taskENTER_CRITICAL();
			if(count == 900)//9ms�ߵ�ƽ
			{
				timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);
			}
			if(count == 1300)//4ms�͵�ƽ
			{
				timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,94);
				LED_OP(1, 1);
			}
			
			if(codei != 0) 
			{
				if((uid_temp >> (codei-1)) & 0x01)
				{
					if(count == 1356)
					{
						timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);
					}
					if(count == 1516)
					{
						ctl_flag = 1301;
						codei--;
						timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,94);
						sendbuf[sendbufi] <<= 1;
						sendbuf[sendbufi] |= 0x01;
						j++;
					}
				}
				else
				{
					if(count == 1356)
					{
						timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);
					}
					if(count == 1412)
					{
						ctl_flag = 1301;
						codei--;
						timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,94);
						sendbuf[sendbufi] <<= 1;
						j++;
					}
				}
			}
			else
			{
				if(count >= 1801)
				{
					sendbufi++;
					if(sendbufi >= 20)
					{
						sendbufi = 0;
					}
					j = 0;
					codei = 32;
					uid_temp = TESTUID;
					
//					timer2_modulate = 0;
//					//ctl_flag = 0;
//					
//					timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);
				}
			}
		//taskEXIT_CRITICAL();
		}
		vTaskDelay(1);
	}
}

static void AppTaskCreate (void)
{
    xTaskCreate( vTaskAttack, "vTaskAttack", 64,NULL,6,NULL);
    xTaskCreate( vTaskInfrared,"infrared",64,NULL,5, NULL);
	xTaskCreate( vTaskBeAttack,"vTaskBeAttack",64,NULL,7, NULL);
	xTaskCreate( vTaskCore,"vTaskCore",64,NULL,4, NULL);
}
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
uint16_t adc_value[4];

extern uint8_t bRfid;
extern uint8_t Rfid_data[4];
extern uint8_t rfid_flag;
int main(void)
{
    /* system clocks configuration */
    rcu_config();
    /* systick configuration */
    //systick_config(); 
		gd_eval_com_init(EVAL_COM1);	
    /* GPIO configuration */
    gpio_config();
		exti_config();

	
		nvic_configuration();
    /* TIMER configuration */
    timer_config();
		LED_OP(2, 1);
		//CC1101_Init();
//��ȡRFID��(UID)��ʼ����
//	while(!GameBegin)
//	{
//		if(1 == decode_RFID())
//		{
//			LED_OP(2, 0 );
//			LED_OP(1, 1);
			rfid_flag = 1;
//			GameBegin = 1;
//		}
//	}
		

	//ϵͳ��Ϣ��ʼ��
		CoreInit();
		AppTaskCreate();
	
      /* �������ȣ���ʼִ������ */
      vTaskStartScheduler();

 
    while(1);

}





void ADC_CMP_IRQHandler(void)
{
	

}
/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{

}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));

    return ch;
}
