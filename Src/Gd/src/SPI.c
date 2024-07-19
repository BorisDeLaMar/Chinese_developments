#include "SPI.h"
#include "main.h"

//user IR module spi interface,should be compeleted by user 
extern SPI_HandleTypeDef hspi1;
uint8_t SPI_Read_Byte(uint8_t pData)
{
	uint8_t temp;

	HAL_SPI_Receive(&hspi1, &temp, 1,100);	
	return temp;
}

void SPI_SendByte(uint8_t pData)
{
	HAL_SPI_Transmit(&hspi1,&pData,1,100);
}

void SPI_FLASH_CS_LOW(void)
{
	//hardware nss
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15,GPIO_PIN_RESET);//TODO
}	
void SPI_FLASH_CS_HIGH(void)
{
	//hardware nss
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15,GPIO_PIN_SET);//TODO
}	
