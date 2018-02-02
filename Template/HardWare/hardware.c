#include "hardware.h"
#include "gd32f10x.h"
#include "spi.h"
#include "CC1101_REG.h"

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
    rcu_periph_clock_enable(RCU_ADC1);
    /* enable timer1 clock */
    rcu_periph_clock_enable(RCU_TIMER1| RCU_TIMER2| RCU_TIMER3| RCU_TIMER4);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);
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

}


/*********************************************TIMER INIT**********************************************/
//TIMER1 38K�����ز� PA1
//TIMER2 �������
//TIMER3 60K�����ز� PB7
//TIMER0 11.025KHz��������/���벶��
/*******************************************************************************************/
/*******************************************************************************************/

void timer_config(void)
{
	    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
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
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);
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
		
		    /* TIMER3 configuration */
    timer_initpara.prescaler         = 6;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 254;
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

    timer_channel_output_config(TIMER3,TIMER_CH_1,&timer_ocintpara);

    /* CH1 configuration in PWM mode1,duty cycle 25% */
    timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_1,0);
    timer_channel_output_mode_config(TIMER3,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER3,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER3);
		  timer_interrupt_enable(TIMER3,TIMER_INT_FLAG_UP);
    /* auto-reload preload enable */
    timer_disable(TIMER3);
		
		/* TIMER0 configuration */
    timer_initpara.prescaler         = 10;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 889;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0,&timer_initpara);
		
    timer_auto_reload_shadow_enable(TIMER0);
		timer_interrupt_enable(TIMER0,TIMER_INT_FLAG_UP);

    /* auto-reload preload enable */
    timer_disable(TIMER0);
}
u32 ctl_flag;

u32 i;

u32 audio_bit;
extern xQueueHandle xQueueInfraredMsg;
void TIMER2_IRQHandler(void)
{
		if (RESET != timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP))
		{
			ctl_flag++;
			if(xQueueInfraredMsg && (ctl_flag == 900 ||ctl_flag == 1300 ||ctl_flag == 1356 ||ctl_flag == 1500 ||ctl_flag == 1444))
			{
				i = ctl_flag;
				xQueueSendToBackFromISR(xQueueInfraredMsg, &i, 0);
				if(ctl_flag == 1500)
				{
					ctl_flag = 1301;
				}
			}
				timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
		}
}

void TIMER0_UP_IRQHandler(void)
{
	u8 audio[23600];
	if (RESET != timer_interrupt_flag_get(TIMER0, TIMER_INT_FLAG_UP))
	{
		timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_1,audio[audio_bit++]);
		if(audio_bit >= 2000)
		{
			audio_bit = 0;
			timer_disable(TIMER3);
			timer_disable(TIMER0);
			printf("audio over!\r\n");
		}
		timer_interrupt_flag_clear(TIMER0, TIMER_INT_FLAG_UP);
	}
}
	
/*******************************************************************************************/

/*********************************************GPIO**********************************************/
/*******************************************************************************************/
void gpio_config(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
  rcu_periph_clock_enable(RCU_AF);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
	
	/***CC1101****/
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
	gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5);
	
	SPI2_Init();
	
	
}
u8 attack_flag;
/*******************************************************************************************/


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

/*************************************����ģ��**********************************************/
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
	gpio_bit_reset(GPIOB, GPIO_PIN_7);
}

void CC_CSN_HIGH(void)
{
	gpio_bit_set(GPIOB, GPIO_PIN_7);
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
	u8 x;
	//TODO
	SPI2_Init();
	CC_CSN_LOW();
	CC_CSN_HIGH();
	CC_CSN_LOW();
	
	for( x = 0; x < 100; x ++ );
	CC1101WriteCmd( CC1101_SRES );
	
	while(CC1101ReadReg(CC1101_AGCTEST) != 0x3F)   //????  
    {  
				for( x = 0; x < 100; x ++ );
				printf("resetting\r\n");
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
	printf("PARTNUM = 0x%02x\r\n", x);
	x = CC1101ReadStatus( CC1101_VERSION );//for test, refer to the datasheet
	printf("VERSION = 0x%02x\r\n", x);	

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
    CC1101SetTRMode(RX_MODE);           // ??????,????
    RecvWaitTime = RECV_TIMEOUT;        // ?????????800ms
    
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

/*******************************************************************************************/