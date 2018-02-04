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
extern u8 attack_flag;
extern u32 audio_bit;
void vTaskAttack(void *pvParameters)
{
	while(1)
	{
			if(gd_eval_key_state_get(IRQ0_IO) == RESET)
			{
				//audio_bit = 0;
			timer_enable(TIMER1);//38Khz载波
			timer_enable(TIMER2);//红外调制
			timer_enable(TIMER3);//60KHz音频载波
			timer_enable(TIMER0);//11.025kHz调制
			printf("I'm Task222\r\n");
				vTaskDelay(ATTACK_FRE/portTICK_RATE_MS);
			}
		vTaskDelay(1);
	}
}

void vTaskBeAttack(void *pvParameters)
{
	while(1)
	{
		
		/**********************************************
		接收到信号，存于Infra_revbuf[]
		***********************************************/
		if(infra_recflag)
		{
			infra_recbuf = 0;
			
		}
		printf("I'm Task55555555\r\n");
		vTaskDelay(500);
	}
}

void vTaskAudio(void *pvParameters)
{
	
}
u8	UID = 0x58;
extern u32 ctl_flag;
void vTaskInfrared(void *pvParameters)
{
	u8 bit_count = 0;
	u32 count = 300;
	u8 uid_temp = UID;
	xQueueInfraredMsg = xQueueCreate(1, sizeof(u32));
	while(1)
	{
		if(xQueueReceive(xQueueInfraredMsg, &count, 0) == pdPASS)
		{
			if(count == 900)//9ms高电平
				timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);
			if(count == 1300)//4ms低电平
				timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,100);
			if(bit_count >= 7)
			{
				bit_count = 0;
				count = 0;
				uid_temp =UID;
				timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);
				timer_disable(TIMER2);
				timer_disable(TIMER1);
				ctl_flag = 0;
				printf("send a message!\r\n");
				
			}
			if(uid_temp & 0x01)
			{
				if(count == 1356)
					timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);
				if(count == 1500)
				{
					timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,100);
					uid_temp = uid_temp >> 1;
					bit_count++;
					count = 1300;
				}
			}
			else
			{
				if(count == 1444)
					timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);
				if(count == 1500)
				{
					timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,100);
					uid_temp = uid_temp >> 1;
					bit_count++;
					count = 1300;
				}
			}

		}
	vTaskDelay(1);
	}
}
void vTaskCore(void *pvParameters)
{
	while(1)
	{
		
	printf("I'm Task33333\r\n");
	vTaskDelay(1000);
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
	
    /* ADC configuration */
   // adc_config();
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
