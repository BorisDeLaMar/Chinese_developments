#include "stm32h7xx_hal.h"
#include "string.h"

///////////////////////////////////////////////////////////////////////////////
extern I2C_HandleTypeDef hi2c2;

#define I2C_TIMEOUT     (1000)
#define MAX_I2C_TX_SIZE (64)

///////////////////////////////////////////////////////////////////////////////
#define I2C_ADDR_ADS1115 	(0x48 << 1)

/**
 *  I2C handles
 */
static int ReadI2C (uint16_t chip, uint8_t reg, uint8_t *buf, int len)
{
    int ret;
    int retry = 1000;//3;

    do {
        ret = HAL_I2C_Master_Transmit(&hi2c2, chip, (uint8_t *)&reg, 1, I2C_TIMEOUT);

    } while (ret != HAL_OK && --retry > 0);

    if (ret != HAL_OK)
        return -1;

    retry = 1000;//3;

    do {
        ret = HAL_I2C_Master_Receive(&hi2c2, chip, buf, len, I2C_TIMEOUT);

    } while (ret != HAL_OK && --retry > 0);
    
    return ret;
}



static int WriteI2C(uint16_t chip, uint8_t reg, uint8_t *buf, int len)
{
    int ret;
    int retry = 1000;//3;
    uint8_t xx[MAX_I2C_TX_SIZE + 1];

    if (len > MAX_I2C_TX_SIZE) {
        //printf("Only support %d bytes written for I2C\r\n", MAX_I2C_TX_SIZE);
        return -1;
    }

    xx[0] = reg;
    memcpy(xx + 1, buf, len);

    do {
        ret = HAL_I2C_Master_Transmit(&hi2c2, chip, xx, 1 + len, I2C_TIMEOUT);

    } while (ret != HAL_OK && --retry > 0);

    return ret;
}



/**********  ADS1115 **************/ 
int ads1115_init(void)
{
	int ret;
	uint8_t value[2];
	
	// Lo_thresh register
	value[0] = 0;
	value[1] = 0;
	ret = WriteI2C(I2C_ADDR_ADS1115, 0x02, value, 2);

	// Hi_thresh register
	value[0] = 0xff;
	value[1] = 0xff;
	ret = WriteI2C(I2C_ADDR_ADS1115, 0x03, value, 2);
	
	// config register
	value[0] = 0x43;
	value[1] = 0x80;
	ret = WriteI2C(I2C_ADDR_ADS1115, 0x01, value, 2);
	
	return ret;
}


int ads1115_convert(uint8_t chn)
{
	int ret;
	uint8_t value[2];
	
	if ( chn > 3 )
		chn = 3;
	
	// config register
	value[0] = 0xc3 + (chn << 4);
	value[1] = 0x80;
	ret = WriteI2C(I2C_ADDR_ADS1115, 0x01, value, 2);
	
	return ret;
}

int ads1115_read(uint16_t *val)
{
	int ret;
	uint8_t value[2];
	
	// conversion register
	ret = ReadI2C(I2C_ADDR_ADS1115, 0x0, (uint8_t *)(&value[0]), 2);  
	
	*val = (value[0] << 8) | value[1];
	
	return ret;
}


