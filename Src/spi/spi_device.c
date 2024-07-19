#include "stm32h7xx_hal.h"
#include "string.h"
#include "spi_device.h"
#include "ir_def.h"

///////////////////////////////////////////////////////////////////////////////
extern SPI_HandleTypeDef hspi1;

#define SPI_TIMEOUT     (1000)

int sensor_reg_read(uint8_t reg_addr, uint8_t * val)
{
	int ret;
	
	uint8_t tx[4] = {0xBC, 0x00, 0x00, 0x00};
	uint8_t rx[4] = {0x00, 0x00, 0x00, 0x00};
	
	tx[1] = reg_addr;
	
	ret = HAL_SPI_TransmitReceive(&hspi1, tx, rx, 4, SPI_TIMEOUT);
	
	*val = rx[3];
	
	return ret;
}

int sensor_reg_write(uint8_t reg_addr, uint8_t val)
{
	if(reg_addr==0x19)
	{
		val =  val & 0xfc;
	}
	else if(reg_addr==0x1a)
	{
		val =  val | 0x08;
	}
	
	int ret;
	
	uint8_t tx[3] = {0xAF, 0x00, 0x00};
	
	tx[1] = reg_addr;
	tx[2] = val;
	
	ret = HAL_SPI_Transmit(&hspi1, tx, 3, SPI_TIMEOUT);
	
	return ret;
}

int sensor_init(PARAM_DETECT *p_config)
{
	int ret;

	INT_time_set(p_config->INT_value);
#if 0
	p_config->detect_table[6] = 0x00c9;
	p_config->detect_table[7] = 0x0080;	
	p_config->detect_table[8] = 0x001d;
	p_config->detect_table[9] = 0x0030;
	p_config->detect_table[10] = 0x0008;
	p_config->detect_table[11] = 0x0030;
#endif
	
	int i;
	for ( i=0; i<12; i++ )
	{
		ret = sensor_reg_write((i+0x10), (p_config->detect_table[i]&0xff));
	}
		
	
	sensor_reg_write(0x22, 0x82);	//4M/25Hz/(92+4)
	sensor_reg_write(0x23, 0x06);
	
	return ret;
}

int sensor_reg_dump(uint8_t *reg_buf)
{
	int ret;
	int i;

	uint8_t reg_addr_table[] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0xf0, 0xf1};
	
	for ( i=0; i<sizeof(reg_addr_table); i++ )
	{		
		ret = sensor_reg_read(reg_addr_table[i], &reg_buf[i]);	
	}
		
	return ret;
}



int INT_time_set(uint8_t us)
{
	int cont = us * 4;
	uint8_t temp;
	
	temp = cont & 0x000000ff;
	sensor_reg_write(0x20, temp);
	temp = (cont & 0x00000f00) >> 8;
	sensor_reg_write(0x21, temp);
		
	
	return 0;
}



