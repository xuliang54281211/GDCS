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
#define ATTACK_FRE 980
/**********全局变量************/
u8	UID = 0x8f;
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
void vTaskAttack(void *pvParameters)
{
	while(1)
	{
		if(!GameOver)
		{
			if(gd_eval_key_state_get(IRQ0_IO) == RESET)
			{
				//audio_bit = 0;
			timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,94);//38Khz载波
			timer2_modulate = 1;
			/*timer_enable(TIMER2)*/;//开启红外调制
			timer_enable(TIMER3);//60KHz音频载波
			timer_enable(TIMER0);//11.025kHz调制
			printf("I'm Task222\r\n");
			vTaskDelay(ATTACK_FRE/portTICK_RATE_MS);
			}
		}
		vTaskDelay(1);
	}
}


void vTaskAudio(void *pvParameters)
{
	
}

extern u32 ctl_flag;
u8 end_zero, end_one;
void vTaskInfrared(void *pvParameters)
{
	u32 count = 300;
	u8 i = 8;
	u8 bit_count = 0;
	u8 uid_temp = UID;
	xQueueInfraredMsg = xQueueCreate(1, sizeof(u32));
	while(1)
	{
		if(xQueueReceive(xQueueInfraredMsg, &count, 0) == pdPASS)
		{
			if(count == 900)//9ms高电平
			{
				timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);
			}
			if(count == 1300)//4ms低电平
			{
				timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,94);
			}
			if(i != 0)
			{
				if((uid_temp >> (i-1)) & 0x01)
				{
					if(count == 1356)
					{
						timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);
					}
					if(count == 1516)
					{
						ctl_flag = 1301;
						i--;
						timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,94);
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
						i--;
						timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,94);
					}
				}
			}
			else
			{
				if(count >= 1801)
				{
					i = 8;
					timer2_modulate = 0;
					ctl_flag = 0;
					timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);
				}
			}
		}
	vTaskDelay(1);
	}
}

static void AppTaskCreate (void)
{
    xTaskCreate( vTaskAttack, "vTaskAttack", 64,NULL,5,NULL);
    xTaskCreate( vTaskInfrared,"infrared",64,NULL,2, NULL);
		xTaskCreate( vTaskBeAttack,"vTaskBeAttack",64,NULL,4, NULL);
		xTaskCreate( vTaskCore,"vTaskCore",64,NULL,3, NULL);
}
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
uint16_t adc_value[4];
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
		
		//CC1101_Init();
	
//读取RFID卡(UID)后开始任务
//	while(!GameBegin)
//	{
//		if(decoder_RFID())
//		{
//			GameBegin = 1;
//			CoreInit();
//		}
//	}
		
		printf("Begin adc!\r\n");
		printf("test over!\r\n");
		//printf("Begin adc!\r\n");
	//系统信息初始化
		//CoreInit();
		AppTaskCreate();
	
      /* 启动调度，开始执行任务 */
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
