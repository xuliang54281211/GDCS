#include "hardware.h"
#include "gd32f10x.h"
#include "spi.h"
#include "CC1101_REG.h"

#define ON 1
#define OFF 0
u32 ctl_flag;
u32 i;
u32 audio_bit;
u32 IRAHighCnt, IRALowCnt;
u8 timer2_modulate, InfraLen, rfid_flag;
u32 Infra_recvbuf[INFRA_RECV_BUFF_SIZE];
extern xQueueHandle xQueueInfraredMsg;
/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOA |RCU_GPIOB |RCU_GPIOC| RCU_GPIOD);
    /* enable ADC clock */
    /* enable timer1 clock */
    rcu_periph_clock_enable(RCU_TIMER1| RCU_TIMER2| RCU_TIMER3| RCU_TIMER4);
rcu_periph_clock_enable(RCU_USART0);
}


/*!
    \brief      configure systick
    \param[in]  none
    \param[out] none
    \retval     none
*/
void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U)){
        /* capture error */
        while (1){
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}


void nvic_configuration(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(TIMER2_IRQn, 1, 1);
		nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(TIMER0_UP_IRQn, 1, 2);
    //nvic_irq_enable(SPI0_IRQn,1,1);
   // nvic_irq_enable(SPI2_IRQn,0,1);

}


/*********************************************TIMER INIT**********************************************/
//TIMER1 38K红外载波 PA1
//TIMER2 红外调制
//TIMER3 60K语音载波 PB7
//TIMER0 11.025KHz语音调制/输入捕获
/*******************************************************************************************/
/*******************************************************************************************/

void timer_config(void)
{
	    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
	    timer_ic_parameter_struct timer_icinitpara;

	    /* -----------------------------------------------------------------------
    TIMER1 configuration: generate 3 PWM signals with 3 different duty cycles:
    TIMER1CLK = SystemCoreClock / 108 = 1MHz

    TIMER1 channel1 duty cycle = (4000/ 16000)* 100  = 25%
    TIMER1 channel2 duty cycle = (8000/ 16000)* 100  = 50%
    TIMER1 channel3 duty cycle = (12000/ 16000)* 100 = 75%
    ----------------------------------------------------------------------- */
	
    rcu_periph_clock_enable(RCU_TIMER1);
		rcu_periph_clock_enable(RCU_TIMER2);
		rcu_periph_clock_enable(RCU_TIMER3);
		rcu_periph_clock_enable(RCU_TIMER0);
    timer_deinit(TIMER1);
		timer_deinit(TIMER2);
		timer_deinit(TIMER3);
		timer_deinit(TIMER0);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 9;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 283;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

    /* CH1,CH2 and CH3 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocintpara);

    /* CH1 configuration in PWM mode1,duty cycle 25% */
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,100);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
		  timer_interrupt_enable(TIMER1,TIMER_INT_FLAG_UP);
    /* auto-reload preload enable */
    timer_enable(TIMER1);
		
    /* TIMER2 configuration */
    timer_initpara.prescaler         = 107;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2,&timer_initpara);
		
    timer_auto_reload_shadow_enable(TIMER2);
		timer_interrupt_enable(TIMER2,TIMER_INT_FLAG_UP);

    /* auto-reload preload enable */
    timer_enable(TIMER2);
		
		    /* TIMER3 configuration */ //RFID
    timer_initpara.prescaler         = 107;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 8;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER3,&timer_initpara);

    /* CH1,CH2 and CH3 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER3,TIMER_CH_0,&timer_ocintpara);

    /* CH1 configuration in PWM mode1,duty cycle 25% */
    timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_0,4);
    timer_channel_output_mode_config(TIMER3,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER3,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER3);
		  timer_interrupt_enable(TIMER3,TIMER_INT_FLAG_UP);
    /* auto-reload preload enable */
    timer_enable(TIMER3);
		
		/* TIMER0 configuration */
    timer_initpara.prescaler         = 10;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 889;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0,&timer_initpara);
		
		/* TIMER0  configuration */
    /* TIMER0 CH0 input capture configuration */
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_FALLING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x0;
    timer_input_capture_config(TIMER0,TIMER_CH_0,&timer_icinitpara);

		
    timer_auto_reload_shadow_enable(TIMER0);
		timer_interrupt_enable(TIMER0,TIMER_INT_FLAG_UP);

    /* auto-reload preload enable */
    timer_disable(TIMER0);
}
extern u8 end_one;
extern u8 end_zero;
uint8_t Rfid_bits[128];
uint8_t rfCT;
uint8_t bRfid;
uint8_t bRftriger;
void TIMER2_IRQHandler(void)
{
		if (RESET != timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP))
		{
			if(!rfid_flag)
			{
				if(gd_eval_key_state_get(RFIDIN_IO))
				{
					if(bRftriger)
					{
						/*HIGH Level*/
						if((IRALowCnt >= 15))//一个合格的下降沿---->1
						{
							Rfid_bits[rfCT] = 0;
							if(++rfCT == 0)
								bRfid = 1;
							if(IRALowCnt >= 40)
							{
								Rfid_bits[rfCT] = 0;
								if(++rfCT == 0)
									bRfid = 1;
							}
							bRftriger = 0;
						}
					}
						IRALowCnt = 0;
						IRAHighCnt++;
						if(IRAHighCnt >= 32767)
							IRAHighCnt = 0;
				}
				else
				{
					/* Low Level */
					if(!bRftriger)
					{
						if((IRAHighCnt >= 15))//一个合格的下降沿---->1
						{
							Rfid_bits[rfCT] = 1;
							if(++rfCT == 0)
								bRfid = 1;
							if(IRAHighCnt >= 40)
							{
								Rfid_bits[rfCT] = 1;
								if(++rfCT == 0)
									bRfid = 1;
							}
							bRftriger = 1;
						}
					}
						IRAHighCnt = 0;
						
						IRALowCnt++;
						if(IRALowCnt >= 32767)
							IRALowCnt = 0;
				}
						
			}
			else
			{
				if(timer2_modulate)
				{
					ctl_flag++;
					if(xQueueInfraredMsg && (ctl_flag == 900 ||ctl_flag == 1300 ||ctl_flag == 1356 ||ctl_flag == 1516 ||ctl_flag == 1412 || ctl_flag >= 1801))
					{
						i = ctl_flag;
						xQueueSendToBackFromISR(xQueueInfraredMsg, &i, 0);
						if(ctl_flag >= 1801)
						{
							timer2_modulate = 0;
							ctl_flag = 0;
							timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);
						}
	//					if(ctl_flag >= 1516 && !end_one && !end_zero)
	//					{
	//						ctl_flag = 1301;
	//					}
					}
				}
				//TODO_Input Infra signal detect
				if(gd_eval_key_state_get(IRAIN_IO))
				{
					/* High Level */
					if(IRALowCnt >= 40)
					{
						IRALowCnt &= 0x7fff;
						Infra_recvbuf[InfraLen++] = IRALowCnt;
						if(InfraLen >= INFRA_RECV_BUFF_SIZE)
							InfraLen = 0;
					}
					IRALowCnt = 0;
					
					IRAHighCnt++;
					if(IRAHighCnt >= 32767)
						IRAHighCnt = 0;
				}
				else
				{
					/* Low Level */
					if(IRAHighCnt >= 40)
					{
						IRAHighCnt |= 0x8000;
						Infra_recvbuf[InfraLen++] = IRAHighCnt;
						if(InfraLen >= INFRA_RECV_BUFF_SIZE)
							InfraLen = 0;
					}
					IRAHighCnt = 0;
					
					IRALowCnt++;
					if(IRALowCnt >= 32767)
						IRALowCnt = 0;
				
				}
			}
			timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
		}
}
u8 IraSta;	  
u16 Dval;		//下降沿时计数器的值
u32 IraRec=0;	//红外接收到的数据	   		    
u8  IraCnt=0;	//按键按下的次数	
void TIMER0_UP_IRQHandler(void)
{
	//u8 audio[23600];
	if (RESET != timer_interrupt_flag_get(TIMER0, TIMER_INT_FLAG_UP))
	{
//		timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_1,audio[audio_bit++]);
//		if(audio_bit >= 2000)
//		{
//			audio_bit = 0;
//			timer_disable(TIMER3);
//			timer_disable(TIMER0);
//			//printf("audio over!\r\n");
//		}
		if(IraSta&0x80)								//上次有数据被接收到了
		{	
			IraSta&=~0X10;							//取消上升沿已经被捕获标记
			if((IraSta&0X0F)==0X00)IraSta|=1<<6;	//标记已经完成一次按键的键值信息采集
			if((IraSta&0X0F)<14)IraSta++;
			else
			{
				IraSta&=~(1<<7);					//清空引导标识
				IraSta&=0XF0;						//清空计数器	
			}								 	   	
		}		
	}
	if (RESET != timer_interrupt_flag_get(TIMER0, TIMER_INT_FLAG_CH0))
	{
		if(gd_eval_key_state_get(IRAIN_IO))//高电平
		{
			timer_input_polarity_config(TIMER0,TIMER_CH_0,TIMER_IC_POLARITY_FALLING);
			timer_counter_value_config(TIMER0, 0);
			IraSta |= 0x10;
		}
		else
		{
			Dval = timer_channel_capture_value_register_read(TIMER0, TIMER_CH_0);
			timer_input_polarity_config(TIMER0,TIMER_CH_0,TIMER_IC_POLARITY_RISING);
			if(IraSta & 0x10)//已收到上升沿
			{
				if(IraSta & 0x80)//已收到引导码
				{
					if(Dval > 300 && Dval < 800)
					{
						IraRec <<= 1;
						IraRec |= 0;
					}
					else if(Dval > 1400 && Dval < 1800)
					{
						IraRec <<= 1;
						IraRec |= 1;
					}
					else if(Dval > 2200 && Dval < 2600)
					{
						IraCnt++;
						IraSta &= 0xF0;
					}
					else
					{
						IraSta = 0;
					}
				}
				else if(Dval > 4200 && Dval < 4700)//标准4.5ms高电平
				{
					IraSta |= 0x80;
					IraCnt = 0;
				}
			}
			IraSta &= ~(1<<4);
		}
	}
	timer_interrupt_flag_clear(TIMER0, TIMER_INT_FLAG_UP| TIMER_INT_FLAG_CH0);
}
	
/*******************************************************************************************/

/*********************************************GPIO**********************************************/
/*******************************************************************************************/
void gpio_config(void)
{
	rcu_periph_clock_enable(RCU_GPIOA| RCU_GPIOB);
  rcu_periph_clock_enable(RCU_AF);
	gpio_init(IRAOUT_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, IRAOUT);//IRA_OUT  38K PWM TIMER1_CH1
	gpio_init(RFIDOUT_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, RFIDOUT);//RFID_OUT 125K PWM TIMER3_CH0
	
	/***CC1101****/
	//gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);//CSN
	gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_0);//GDO0
	//
	/***IRA_SYSTEM****/
	gpio_init(IRAIN_PORT, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, IRAIN);//IRA_IN
	gpio_init(KEYIN_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, KEYIN);//KEY_INPUT
	
	/***LED1, LED2***/
	gpio_init(LED_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED1);
	gpio_init(LED_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED2);
	gpio_bit_set(LED_PORT, LED1);
	gpio_bit_set(LED_PORT, LED2);
}
u8 attack_flag;
/*******************************************************************************************/
/*********************************************LED_OP**********************************************/
void LED_OP(u8 led, u8 sw)
{
	if(led == 1)
	{
		if(sw == ON)
			gpio_bit_reset(LED_PORT, LED1);
		else
			gpio_bit_set(LED_PORT, LED1);
	}
	if(led == 2)
	{
		if(sw == ON)
			gpio_bit_reset(LED_PORT, LED2);
		else
			gpio_bit_set(LED_PORT, LED2);
	}
		
}

/*********************************************EXTI_CONFIG**********************************************/
/*******************************************************************************************/
void exti_config(void)
{
	    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);
			
}

void EXTI0_IRQHandler(void)
{
	u8 i;
	if (RESET != exti_interrupt_flag_get(EXTI_0))
	{
		timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,100);
		attack_flag = 1;
		exti_interrupt_flag_clear(EXTI_0);
	}
}
/*******************************************************************************************/

/*************************************无线模块**********************************************/
u8   Cnt1ms = 0;             // 1ms????,?1ms?? 
u8   SendFlag = 0;           // =1,??????,=0???

u16  SendTime = 1;           // ??????????
u16  RecvWaitTime = 0;       // ??????                
u16  SendCnt = 0;            // ?????????                

u8 AckBuffer[ACK_LENGTH] = { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
u8 RecvCnt;
u8 PaTabel[] = { 0xc0, 0xC8, 0x84, 0x60, 0x68, 0x34, 0x1D, 0x0E};
static const u8 CC1101InitData[22][2]= 
{
  {CC1101_IOCFG0,      0x06},
  {CC1101_FIFOTHR,     0x47},
  {CC1101_PKTCTRL0,    0x05},
  {CC1101_CHANNR,      0x01},
  {CC1101_FSCTRL1,     0x06},
  {CC1101_FREQ2,       0x0F},
  {CC1101_FREQ1,       0x62},
  {CC1101_FREQ0,       0x76},
  {CC1101_MDMCFG4,     0xF6},
  {CC1101_MDMCFG3,     0x43},
  {CC1101_MDMCFG2,     0x13},
  {CC1101_DEVIATN,     0x15},
  {CC1101_MCSM0,       0x18},
  {CC1101_FOCCFG,      0x16},
  {CC1101_WORCTRL,     0xFB},
  {CC1101_FSCAL3,      0xE9},
  {CC1101_FSCAL2,      0x2A},
  {CC1101_FSCAL1,      0x00},
  {CC1101_FSCAL0,      0x1F},
  {CC1101_TEST2,       0x81},
  {CC1101_TEST1,       0x35},
  {CC1101_MCSM1,       0x3B},
};


void CC_CSN_LOW(void)
{
	gpio_bit_reset(GPIOB, GPIO_PIN_1);
}

void CC_CSN_HIGH(void)
{
	gpio_bit_set(GPIOB, GPIO_PIN_1);
}

void CC1101WriteCmd( u8 command )
{
    CC_CSN_LOW( );
    SPI_ExchangeByte( command );
    CC_CSN_HIGH( );
}
u8 CC1101ReadReg( u8 addr )
{
    u8 i;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr | READ_SINGLE);
    i = SPI_ExchangeByte( 0xFF );
    CC_CSN_HIGH( );
    return i;
}

void CC1101WriteReg( u8 addr, u8 value )
{
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr );
    SPI_ExchangeByte( value );
    CC_CSN_HIGH( );
}

void CC1101SetAddress( u8 address, ADDR_MODE AddressMode)
{
    u8 btmp = CC1101ReadReg( CC1101_PKTCTRL1 ) & ~0x03;
    CC1101WriteReg(CC1101_ADDR, address);
    if     ( AddressMode == BROAD_ALL )     {}
    else if( AddressMode == BROAD_NO  )     { btmp |= 0x01; }
    else if( AddressMode == BROAD_0   )     { btmp |= 0x02; }
    else if( AddressMode == BROAD_0AND255 ) { btmp |= 0x03; }   
		CC1101WriteReg(CC1101_PKTCTRL1, btmp);
}

void CC1101SetSYNC(u16 Sync)
{
	CC1101WriteReg(CC1101_SYNC1, 0xFF & (Sync >>8));
	CC1101WriteReg(CC1101_SYNC0, 0xFF & Sync);
}

void CC1101WriteMultiReg( u8 addr, u8 *buff, u8 size )
{
    u8 i;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr | WRITE_BURST );
    for( i = 0; i < size; i ++ )
    {
        SPI_ExchangeByte( *( buff + i ) );
    }
    CC_CSN_HIGH( );
}

u8 CC1101ReadStatus( u8 addr )
{
    u8 i;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr | READ_BURST);
    i = SPI_ExchangeByte( 0xFF );
    CC_CSN_HIGH( );
    return i;
}

void CC1101_Init(void)
{
	u32 x;
	//TODO
	SPI2_Init();
	CC_CSN_LOW();
	CC_CSN_HIGH();
	CC_CSN_LOW();
	
	for( x = 0; x < 1000; x ++ );
	CC1101WriteCmd( CC1101_SRES );
	
	while(CC1101ReadReg(CC1101_AGCTEST) != 0x3F)   //????  
    {  
				for( x = 0; x < 100; x ++ );
				//printf("resetting\r\n");
    }  
	
	for(x = 0; x < 22; x++)
	{
		CC1101WriteReg(CC1101InitData[x][0], CC1101InitData[x][1]);
	}
	CC1101SetAddress( 0x05, BROAD_0AND255 );
	
	CC1101SetSYNC( 0x8799 );
	
	CC1101WriteReg(CC1101_MDMCFG1,   0x72); //Modem Configuration
	
	CC1101WriteMultiReg(CC1101_PATABLE, PaTabel, 8 );

	x = CC1101ReadStatus( CC1101_PARTNUM );//for test, must be 0x80
	//printf("PARTNUM = 0x%02x\r\n", x);
	x = CC1101ReadStatus( CC1101_VERSION );//for test, refer to the datasheet
	//printf("VERSION = 0x%02x\r\n", x);	

}

void CC1101SetIdle( void )
{
    CC1101WriteCmd(CC1101_SIDLE);
}

void CC1101ClrTXBuff( void )
{
    CC1101SetIdle();//MUST BE IDLE MODE
    CC1101WriteCmd( CC1101_SFTX );
}

void CC1101SetTRMode( TRMODE mode )
{
    if( mode == TX_MODE )
    {
        CC1101WriteReg(CC1101_IOCFG0,0x46);
        CC1101WriteCmd( CC1101_STX );
    }
    else if( mode == RX_MODE )
    {
        CC1101WriteReg(CC1101_IOCFG0,0x46);
        CC1101WriteCmd( CC1101_SRX );
    }
}


void CC1101SendPacket( u8 *txbuffer, u8 size, TX_DATA_MODE mode )
{
    u8 address;
    if( mode == BROADCAST )             { address = 0; }
    else if( mode == ADDRESS_CHECK )    { address = CC1101ReadReg( CC1101_ADDR ); }

    CC1101ClrTXBuff( );
    
    if( ( CC1101ReadReg( CC1101_PKTCTRL1 ) & ~0x03 ) != 0 )
    {
        CC1101WriteReg( CC1101_TXFIFO, size + 1 );
        CC1101WriteReg( CC1101_TXFIFO, address );
    }
    else
    {
        CC1101WriteReg( CC1101_TXFIFO, size );
    }

    CC1101WriteMultiReg( CC1101_TXFIFO, txbuffer, size );
    CC1101SetTRMode( TX_MODE );
    while( gd_eval_key_state_get(IRQ0_IO) != 0 );
    while( gd_eval_key_state_get(IRQ0_IO) == 0 );

    CC1101ClrTXBuff( );
}


u8 RF_SendPacket(u8 *Sendbuffer, u8 length)
{
    u8 ack_flag = 0;         // =1,???????,=0???
    u8 error = 0, i=0, ack_len=0, ack_buffer[65]={ 0 }, TxBuffer[100];

    CC1101SendPacket(Sendbuffer, length, ADDRESS_CHECK);    // ????   
    //CC1101SetTRMode(RX_MODE);           // ??????,????
    //RecvWaitTime = RECV_TIMEOUT;        // ?????????800ms
    
    ack_flag = 1;
    
//    while (CC_IRQ_READ() != 0)
//	{
//		if (0 == RecvWaitTime)      { ack_flag = 0; break; }
//	}
//    
//    if (0 != ack_flag)                          // ?????????
//    {
//        while (CC_IRQ_READ() == 0);
//		ack_len = CC1101RecPacket(ack_buffer);  // ???????

//        // ????????,???????10-19
//        for (i=0, error=0; i<10; i++ )
//        {
//            if (ack_buffer[i] != (i+10))    { error=1; break; }
//							
//        }
//        
//        if ((ack_len==10) && (error==0))    { printf("Get ACK=%s\r\n", ack_buffer);return (1); } // ????  
//    }
    
    return (1);  
}

void RF_RecvHandler(void)
{
//    u8 error=0, i=0, length=0, recv_buffer[65]={ 0 };
//    
//    CC1101SetTRMode(RX_MODE);           // ??RF??????,????
//    if (0 == CC_IRQ_READ())             // ?????????????? 
//    {
//        while (CC_IRQ_READ() == 0);

//        // ????,?????
//        for (i=0; i<65; i++)   { recv_buffer[i] = 0; } 
//            
//        length = CC1101RecPacket(recv_buffer);
//       printf("recv_buffer=%s , length=%d\r\n", recv_buffer, length);
//        for (i=0, error=0; i<length; i++)
//        {
//					printf("%c ", recv_buffer[i]);
//            if (recv_buffer[i] != i)    { printf("recv[%d] error=%d\r\n",i, recv_buffer[i]);error=1; break; } // ????
//        }
//				printf("\r\n");
//        if ((length==10) && (error==0))                     // ????
//        {                            // LED??,????????
//            delay_ms(10);
//            
//            CC1101SetTRMode(TX_MODE);   // ??????
//            CC1101SendPacket(AckBuffer, ACK_LENGTH, ADDRESS_CHECK);
//            printf("get wireless!\r\n");
//						if(xQueueWirelessMsg)
//						{
//							xQueueSend(xQueueWirelessMsg, &recvWirelessMsg, 0);
//						}
//            RecvCnt++; 
//        }
//        
//        CC1101SetTRMode(RX_MODE);           // ??RF??????,????
//    }    
}

/*******************************************************************************************/
