
#include "W25QXX.h"
#include "spi_device.h"

#include "string.h"

extern SPI_HandleTypeDef hspi4;

#define PAGE_SIZE (256)
#define SECTOR_SIZE (0X1000)
__ALIGN_BEGIN static uint8_t Flash_buff[ PAGE_SIZE + 4 ] __ALIGN_END;

/**
  * @brief  This function reset the W25Qx.
  * @retval None
  */
static void BSP_W25Qx_Reset(void)
{
	uint8_t cmd;
	cmd = RESET_ENABLE_CMD;
	HAL_SPI_Transmit(&hspi4, &cmd, 1, W25Qx_TIMEOUT_VALUE);

	cmd = RESET_MEMORY_CMD;
	HAL_SPI_Transmit(&hspi4, &cmd, 1, W25Qx_TIMEOUT_VALUE);
}

/**
  * @brief  Reads current status of the W25Q128FV.
  * @retval W25Q128FV memory status
  */
static uint8_t BSP_W25Qx_GetStatus(void)
{
	uint8_t cmd[] = {READ_STATUS_REG1_CMD, 0};
	uint8_t rx[] = {0, 0};
	uint8_t status;
	
	HAL_SPI_TransmitReceive(&hspi4, cmd, rx, 2, W25Qx_TIMEOUT_VALUE);
	status = rx[1];
	
	/* Check the value of the register */
  if((status & W25Q128FV_FSR_BUSY) != 0)
  {
    return W25Qx_BUSY;
  }
	else
	{
		return W25Qx_OK;
	}		
}

/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @retval None
  */
static uint8_t BSP_W25Qx_WriteEnable(void)
{
	uint32_t tickstart = HAL_GetTick();

	uint8_t cmd;
	cmd = WRITE_ENABLE_CMD;
	HAL_SPI_Transmit(&hspi4, &cmd, 1, W25Qx_TIMEOUT_VALUE);
	
	/* Wait the end of Flash writing */
	while(BSP_W25Qx_GetStatus() == W25Qx_BUSY)
	{
		/* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > W25Qx_TIMEOUT_VALUE)
    {        
			return W25Qx_TIMEOUT;
    }
	}
	
	return W25Qx_OK;
}


/**
  * @brief  Erases the specified block of the QSPI memory. 
  * @param  BlockAddress: Block address to erase  
  * @retval QSPI memory status
  */
uint8_t BSP_W25Qx_Erase_Sector(uint32_t sector_num)
{
	
	uint32_t Address = sector_num * SECTOR_SIZE;
	uint8_t cmd[4];
//	if( sector_num%2==0 )
//	{
		uint32_t tickstart = HAL_GetTick();
		cmd[0] = SECTOR_ERASE_CMD;
		cmd[1] = (uint8_t)(Address >> 16);
		cmd[2] = (uint8_t)(Address >> 8);
		cmd[3] = (uint8_t)(Address);
		
		/* Enable write operations */
		BSP_W25Qx_WriteEnable();
		
		HAL_SPI_Transmit(&hspi4, cmd, 4, W25Qx_TIMEOUT_VALUE);
		
		/* Wait the end of Flash writing */
		while(BSP_W25Qx_GetStatus() == W25Qx_BUSY)
		{
			/* Check for the Timeout */
			if((HAL_GetTick() - tickstart) > W25Q128FV_SECTOR_ERASE_MAX_TIME)
			{        
				return W25Qx_TIMEOUT;
			}
		}
//	}
	return W25Qx_OK;
}

/**
  * @brief  Initializes the W25Q128FV interface.
  * @retval None
  */
uint8_t BSP_W25Qx_Init(void)
{ 
	/* Reset W25Qxxx */
	BSP_W25Qx_Reset();
	
	return BSP_W25Qx_GetStatus();
}

/**
  * @brief  Reads an amount of data from the QSPI memory.
  * @param  pData: Pointer to data to be read
  * @param  ReadAddr: Read start address
  * @param  Size: Size of data to read    
  * @retval QSPI memory status
  */
uint8_t BSP_W25Qx_Read_Sector(uint32_t sector_num, uint8_t* pData, uint32_t Size)
{	
	uint32_t ReadAddr = sector_num * SECTOR_SIZE;
	
	uint32_t cnt = Size / PAGE_SIZE;
	uint32_t left = Size % PAGE_SIZE;
	
//	uint32_t jump_cnt = 0;
		
	while(cnt > 0)
  {
		/* Configure the command */
		Flash_buff[0] = READ_CMD;
		Flash_buff[1] = (uint8_t)(ReadAddr >> 16);
		Flash_buff[2] = (uint8_t)(ReadAddr >> 8);
		Flash_buff[3] = (uint8_t)(ReadAddr);
		

		if (HAL_SPI_TransmitReceive(&hspi4, Flash_buff, Flash_buff, (PAGE_SIZE+4), W25Qx_TIMEOUT_VALUE) != HAL_OK)
		{
			return W25Qx_ERROR;
		}
		
		memcpy(pData, &Flash_buff[4], PAGE_SIZE);
		
		ReadAddr += PAGE_SIZE;
    pData += PAGE_SIZE;
		cnt--;
		
		#if 0
		jump_cnt++;
		if( jump_cnt == 8 )
		{
			ReadAddr += PAGE_SIZE * 8;
			jump_cnt = 0;
		}
		#endif
	}
	
	if ( left !=0 )
	{
		/* Configure the command */
		Flash_buff[0] = READ_CMD;
		Flash_buff[1] = (uint8_t)(ReadAddr >> 16);
		Flash_buff[2] = (uint8_t)(ReadAddr >> 8);
		Flash_buff[3] = (uint8_t)(ReadAddr);
		
		if (HAL_SPI_TransmitReceive(&hspi4, Flash_buff, Flash_buff, (left+4), W25Qx_TIMEOUT_VALUE) != HAL_OK)
		{
			return W25Qx_ERROR;
		}
		
		memcpy(pData, &Flash_buff[4], left);
	}

	return W25Qx_OK;
}

/**
  * @brief  Writes an amount of data to the QSPI memory.
  * @param  pData: Pointer to data to be written
  * @param  WriteAddr: Write start address
  * @param  Size: Size of data to write.    
  * @retval QSPI memory status
  */
uint8_t BSP_W25Qx_Program_Sector(uint32_t sector_num, uint8_t* pData, uint32_t Size)
{
	uint32_t WriteAddr = sector_num * SECTOR_SIZE;
	uint32_t tickstart = HAL_GetTick();
	
	BSP_W25Qx_Erase_Sector(sector_num);
	
	uint32_t cnt = Size / PAGE_SIZE;
	uint32_t left = Size % PAGE_SIZE;
  /* Perform the write page by page */
  while(cnt > 0)
  {
		/* Configure the command */
		Flash_buff[0] = PAGE_PROG_CMD;
		Flash_buff[1] = (uint8_t)(WriteAddr >> 16);
		Flash_buff[2] = (uint8_t)(WriteAddr >> 8);
		Flash_buff[3] = (uint8_t)(WriteAddr);
		memcpy(&Flash_buff[4], pData, PAGE_SIZE);

		/* Enable write operations */
		BSP_W25Qx_WriteEnable();

    /* Transmission of the data */
    if (HAL_SPI_Transmit(&hspi4, Flash_buff, (PAGE_SIZE+4), W25Qx_TIMEOUT_VALUE) != HAL_OK)
    {
      return W25Qx_ERROR;
    }
    
    	/* Wait the end of Flash writing */
		while(BSP_W25Qx_GetStatus() == W25Qx_BUSY)
		{
			/* Check for the Timeout */
			if((HAL_GetTick() - tickstart) > W25Qx_TIMEOUT_VALUE)
			{        
				return W25Qx_TIMEOUT;
			}
		}
    
    /* Update the address and size variables for next page programming */
    WriteAddr += PAGE_SIZE;
    pData += PAGE_SIZE;
		cnt--;
  } 
	
	if ( left !=0 )
	{
		/* Configure the command */
		Flash_buff[0] = PAGE_PROG_CMD;
		Flash_buff[1] = (uint8_t)(WriteAddr >> 16);
		Flash_buff[2] = (uint8_t)(WriteAddr >> 8);
		Flash_buff[3] = (uint8_t)(WriteAddr);
		memcpy(&Flash_buff[4], pData, left);

		/* Enable write operations */
		BSP_W25Qx_WriteEnable();

    /* Transmission of the data */
    if (HAL_SPI_Transmit(&hspi4, Flash_buff, (left+4), W25Qx_TIMEOUT_VALUE) != HAL_OK)
    {
      return W25Qx_ERROR;
    }
    
    	/* Wait the end of Flash writing */
		while(BSP_W25Qx_GetStatus() == W25Qx_BUSY)
		{
			/* Check for the Timeout */
			if((HAL_GetTick() - tickstart) > W25Qx_TIMEOUT_VALUE)
			{        
				return W25Qx_TIMEOUT;
			}
		}
	}

	return W25Qx_OK;
}



/**
  * @brief  Reads an amount of data from the QSPI memory.
  * @param  pData: Pointer to data to be read
  * @param  ReadAddr: Read start address
  * @param  Size: Size of data to read    
  * @retval QSPI memory status
  */
uint8_t BSP_W25Qx_Read_Sector1(uint32_t sector_num, uint8_t* pData, uint32_t Size)
{	
	uint32_t ReadAddr = sector_num * SECTOR_SIZE;
	
	uint32_t cnt = Size / PAGE_SIZE;
	uint32_t left = Size % PAGE_SIZE;
	
//	uint32_t jump_cnt = 0;
		
	while(cnt > 0)
  {
		/* Configure the command */
		Flash_buff[0] = READ_CMD;
		Flash_buff[1] = (uint8_t)(ReadAddr >> 16);
		Flash_buff[2] = (uint8_t)(ReadAddr >> 8);
		Flash_buff[3] = (uint8_t)(ReadAddr);
		

		if (HAL_SPI_TransmitReceive(&hspi4, Flash_buff, Flash_buff, (PAGE_SIZE+4), W25Qx_TIMEOUT_VALUE) != HAL_OK)
		{
			return W25Qx_ERROR;
		}
		
		memcpy(pData, &Flash_buff[4], PAGE_SIZE);
		
		ReadAddr += PAGE_SIZE;
    pData += PAGE_SIZE;
		cnt--;
		
		#if 0
		jump_cnt++;
		if( jump_cnt == 8 )
		{
			ReadAddr += PAGE_SIZE * 8;
			jump_cnt = 0;
		}
		#endif
	}
	
	if ( left !=0 )
	{
		/* Configure the command */
		Flash_buff[0] = READ_CMD;
		Flash_buff[1] = (uint8_t)(ReadAddr >> 16);
		Flash_buff[2] = (uint8_t)(ReadAddr >> 8);
		Flash_buff[3] = (uint8_t)(ReadAddr);
		
		if (HAL_SPI_TransmitReceive(&hspi4, Flash_buff, Flash_buff, (left+4), W25Qx_TIMEOUT_VALUE) != HAL_OK)
		{
			return W25Qx_ERROR;
		}
		
		memcpy(pData, &Flash_buff[4], left);
	}

	return W25Qx_OK;
}

/**
  * @brief  Reads a block of data from the FLASH
  * @param  pBuffer   : pointer to the buffer that receives the data read from the FLASH
  * @param  ReadAddr  : FLASH's internal address to read from
  * @param  NumByteToRead : number of bytes to read from the FLASH
  * @retval None    
  */
uint8_t  SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
		uint32_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	
		uint32_t SectorNum=0;
	
	  SectorNum = ReadAddr/SECTOR_SIZE;
	
		
    Addr = ReadAddr % SECTOR_SIZE;
	
    count = SECTOR_SIZE - Addr;
    NumOfPage =  NumByteToRead / SECTOR_SIZE;
    NumOfSingle = NumByteToRead % SECTOR_SIZE;
	
    /* WriteAddr is SPI_FLASH_PageSize aligned  */
    if (Addr == 0)
    {   
        /* NumByteToWrite < SPI_FLASH_PageSize */
        if (NumOfPage == 0) 
        {
						BSP_W25Qx_Read_Sector1(SectorNum, pBuffer, NumByteToRead );
            //SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            while (NumOfPage--)
            {
								BSP_W25Qx_Read_Sector1(SectorNum, pBuffer, SECTOR_SIZE );
                //SPI_FLASH_PageWrite(pBuffer, WriteAddr, PAGE_SIZE);
                SectorNum++;
                pBuffer += SECTOR_SIZE;
            }
						
						if (NumOfSingle != 0)
            {
							//SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
							BSP_W25Qx_Read_Sector1(SectorNum, pBuffer, NumOfSingle );
						}
        }
    }
    else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
    {
        if (NumOfPage == 0)
        {
            /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
            if (NumOfSingle > count) 
            {
                temp = NumOfSingle - count;
                //SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
							  BSP_W25Qx_Read_Sector1(SectorNum, pBuffer, count );
                //WriteAddr +=  count;
								SectorNum++;
                pBuffer += count;
                //SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
								BSP_W25Qx_Read_Sector1(SectorNum, pBuffer, temp );
            }
            else
            {
                //SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
								BSP_W25Qx_Read_Sector1(SectorNum, pBuffer, NumByteToRead );
            }
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            NumByteToRead -= count;
            NumOfPage =  NumByteToRead / SECTOR_SIZE;
            NumOfSingle = NumByteToRead % SECTOR_SIZE;

            //SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
					  BSP_W25Qx_Read_Sector1(SectorNum, pBuffer, count );
					
            //WriteAddr +=  count;
						SectorNum++;
					
            pBuffer += count;

            while (NumOfPage--)
            {
                //SPI_FLASH_PageWrite(pBuffer, WriteAddr, PAGE_SIZE);
								BSP_W25Qx_Read_Sector1(SectorNum, pBuffer, SECTOR_SIZE );
                //WriteAddr +=  PAGE_SIZE;
								SectorNum++;
                pBuffer += SECTOR_SIZE;
            }

            if (NumOfSingle != 0)
            {
                //SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
								BSP_W25Qx_Read_Sector1(SectorNum, pBuffer, NumOfSingle );
            }
        }
    }
	return 0;
}








