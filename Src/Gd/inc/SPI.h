#ifndef __SPI_H
#define __SPI_H

extern unsigned char SPI_Read_Byte(unsigned char  pData);
extern void SPI_SendByte(unsigned char  pData);

extern void SPI_FLASH_CS_LOW(void);
extern void SPI_FLASH_CS_HIGH(void);

#endif
