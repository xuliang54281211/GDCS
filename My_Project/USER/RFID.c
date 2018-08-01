#include "RFID.h"
extern uint8_t Rfid_bits[128];
uint8_t Rfid_data[4];
extern uint8_t rfCT;
extern uint8_t bRftriger;
const char Start_bits[18] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
extern uint32_t MY_UID;
u8 decode_RFID(void)
{
	uint8_t i, j, k;
  //uint8_t col_parity;
  uint8_t row_parity;
  uint8_t temp;
  uint8_t data[11];
	
	for(i = 0; i < 128; i++)
	{
		for(j = 0; j < 18; j++)
		{
			if(Rfid_bits[i+j] != Start_bits[j])
				break;
		}
		if(j == 18)
		{
			i += 18; 
			for(k = 0; k < 11; k++)
			{
				row_parity = 0;
				temp = 0;
				for(j = 0; j < 5; j++)
				{
					temp <<= 1;
					if((Rfid_bits[i] == 0) && (Rfid_bits[i+1] == 1))
					{
						temp &= 0xFE;
						//temp |= 0x01;
						if(j < 4)
							row_parity += 1;
					}
					else if((Rfid_bits[i] == 1) && (Rfid_bits[i+1] == 0))
						//temp &= 0xFE;
						temp |= 0x01;
					else
					{
						return 0;
					}
					i+=2;
				}
				data[k] = (temp >> 1);
				temp &= 0x01;
				row_parity %= 2;
				if(k < 10)
				{
					if(row_parity != temp)
					{					
						return 0;
					}
				}
//				else if(temp != 0)
//				{
//					return 0;
//				}
			}
			if(k == 11)
			{
				Rfid_data[0] = (data[2] << 4) | data[3];
        Rfid_data[1] = (data[4] << 4) | data[5];
        Rfid_data[2] = (data[6] << 4) | data[7];
        Rfid_data[3] = (data[8] << 4) | data[9];
				for (i = 0; i < 128; i++)
							Rfid_bits[i] = 0;
				Rfid_bits[i] = 0;
				MY_UID = (Rfid_data[0] << 6)| (Rfid_data[1] << 4)| (Rfid_data[2] << 2)| Rfid_data[0];
				
				if (Rfid_data[1] == 0x49 && Rfid_data[2] == 0x96&& Rfid_data[3] == 0x9A)
					return 1;
				else
					return 0;
			}
		}
	}

	
}
