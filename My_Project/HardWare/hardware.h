#ifndef __HARDWARE_H
#define __HARDWARE_H
#include "core_include.h"
/*****************CC1101********************/
#define PA_TABLE {0xc2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}

//#define TX              1       // ????
//#define RX              0       // ????

#define SEND_GAP        1000    // ???1s??????
#define RECV_TIMEOUT    800     // ????
#define ACK_LENGTH      10      // ??????        
#define SEND_LENGTH     10      // ?????????

#define PORT_CC_CSN     GPIOB
#define PIN_CC_CSN      GPIO_Pin_4

#define PORT_CC_IRQ     GPIOA
#define PIN_CC_IRQ      GPIO_Pin_2

#define PORT_CC_GDO2    GPIOA
#define PIN_CC_GDO2     GPIO_Pin_3


#define CC_IRQ_READ()   GPIO_ReadInputDataBit(PORT_CC_IRQ, PIN_CC_IRQ)

typedef enum { TX_MODE, RX_MODE } TRMODE;
typedef enum { BROAD_ALL, BROAD_NO, BROAD_0, BROAD_0AND255 } ADDR_MODE;
typedef enum { BROADCAST, ADDRESS_CHECK} TX_DATA_MODE;
/*******************************************/


void rcu_config(void);
void systick_config(void);
void nvic_configuration(void);
void gpio_config(void);
void exti_config(void);
void CC1101_Init(void);
u8 RF_SendPacket(u8 *Sendbuffer, u8 length);
void RF_RecvHandler(void);
#endif
