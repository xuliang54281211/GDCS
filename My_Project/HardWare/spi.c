#include "spi.h"

void SPI2_Init(void)
{
	//TODO
	spi_parameter_struct spi_init_struct;

	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_SPI0);
	
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
  gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
	
	spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
	spi_init_struct.device_mode          = SPI_MASTER;
	spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
	spi_init_struct.nss                  = SPI_NSS_SOFT;
	spi_init_struct.prescale             = SPI_PSC_8;
	spi_init_struct.endian               = SPI_ENDIAN_MSB;

	spi_init(SPI0, &spi_init_struct);
	
	spi_nss_internal_high(SPI0);
	
	spi_enable(SPI0);
	

}

u8 SPI_ExchangeByte(u8 input)
{
  spi_i2s_data_transmit(SPI0, input);
	while (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));   // ????????	
	while (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));  // ????????
	return (spi_i2s_data_receive(SPI0));
}

