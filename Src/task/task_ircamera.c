#include "stm32h7xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"

#include "spi_device.h"

#include "ir_def.h"

#include "W25QXX.h"
#include "Varible.h"
#include "MeasureInterface.h"
#include "Sensor.h"

extern DCMI_HandleTypeDef hdcmi;
extern DMA_HandleTypeDef hdma_memtomem_dma1_stream1;

 static uint8_t dcmi_buff[ ONE_PACKAGE_SIZE * MUTILPEL_BUFFER_NUM ] ;
 static uint8_t X_buff[ FRAME_SIZE ] ;

__ALIGN_BEGIN static uint8_t update_buff[ BYTE_PER_PAGE ] __ALIGN_END;
__ALIGN_BEGIN static uint8_t upload_buff[ 51200*2 + 1*2 ] __ALIGN_END;
__ALIGN_BEGIN static uint8_t y8_buff[ 120*90 ] __ALIGN_END;

uint8_t new_param[PARAM_MAX_LEN];

static volatile uint16_t frame_count = 0;
static uint8_t g_usb_start = 0;
static uint8_t g_upload_start = 0;
static uint16_t g_upload_length = 0;
static uint16_t g_upload_pageID = 0;
//static uint16_t g_upload_offset = 0;

static uint8_t g_update_start = 0;
static uint16_t g_update_pageID = 0;
static uint16_t g_update_length = 0;
static uint16_t g_update_recv_len = 0; 

static uint8_t g_paramline_start = 0;


static uint8_t g_shutter_close = 0;

uint8_t g_nuc_start = 0;
uint8_t g_colorbar_index = 1;

extern int usb_SDK_write(uint8_t* data, uint32_t length);
//extern int usb_connect_ok(void);




/* USB protocol */	
void usb_SDK_read_cb(uint8_t *pbuf, uint16_t len)
{
	if ( g_update_start == 1 )
	{
		memcpy(update_buff + g_update_recv_len, pbuf, len);
		g_update_recv_len += len;
	}
	else
	{
		char cmd[32];
		char value[32];

		sscanf((char *)pbuf, "%[^=]", cmd);
		int ii = strlen(cmd) + 1;
		memcpy( value, (pbuf + ii), (len - ii) );

		if ( strcmp(cmd, "StartX") == 0 )
		{
			if ( g_usb_start == 0 )
			{
				g_usb_start = 1;
				
			}
		}
		else if ( strcmp(cmd, "StopX") == 0 )
		{
			g_usb_start = 0;
		}

		else if ( strcmp(cmd, "Shutter") == 0 )
		{
			if( manual_type == IDLE)
				manual_type = SHUTTER;
		}
		else if ( strcmp(cmd, "NucShutter") == 0 )
		{
			if( manual_type == 0)
			{
				manual_type = NUC_LOOP;
			}
	
		}
		
		else if ( strcmp(cmd, "GetArmParam") == 0 )
		{
			uint16_t *pvalue = (uint16_t *)(&value[0]);
			
			uint16_t pageID = *pvalue;
//			uint16_t offset = *(pvalue+1);
			uint16_t length = *(pvalue+2);
			
//			if ( length <= BYTE_PER_PAGE )
			if ( length <= 51201 )
			{
				g_usb_start = 0;
				g_upload_start = 1;
				g_upload_length = length;
				g_upload_pageID = pageID;
//				g_upload_offset = offset;
			}
		}
		else if ( strcmp(cmd, "SetParamDetect") == 0 )
		{
			uint16_t *pvalue = (uint16_t *)(&value[0]);
			
			uint16_t offset = *(pvalue);
			uint16_t value = *(pvalue+1);
			
			SetDectectValue(offset, value);
		}
		else if ( strcmp(cmd, "SaveArmParam") == 0 )
		{
			uint16_t *pvalue = (uint16_t *)(&value[0]);
			
			uint16_t pageID = *pvalue;
			uint16_t length = *(pvalue+1);
			
			if ( length <= BYTE_PER_PAGE )
			{
				g_usb_start = 0;
				g_update_start = 1;
				g_update_length = length;
				g_update_recv_len = 0;
				g_update_pageID = pageID;
				pParamStatus->save_status = 1;
				pParamStatus1->save_status = 1;
			}
			
		}
		else if ( strcmp(cmd, "GetParamLine") == 0 )
		{
				g_usb_start = 0;
				g_paramline_start = 1;
		}
	}
}

uint16_t MeasurePeriod=0;
/* StartIRCameraTask function */
void StartIRCameraTask(void const * argument)
{

	/* init code for extern FLASH */
	BSP_W25Qx_Init();

	/* read param from internal ROM */
	params_init(&dcmi_buff[0]);

	/* sensor config */
	Sensor_Reg_Initial(SPI_Reg_Data120);
	/* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
	
	/* Start the camera capture */
	HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)(X_buff), (FRAME_SIZE / 4));
	
	osDelay(1000);
	
	/* Infinite loop */
  for(;;)
  {
	
		if ( frame_count == 0 )
		{
			osDelay(1);
		}
		else if ( frame_count > 1 )
		{
			frame_count = 0;	
		}
		else if( frame_count == 1)
		{				
			
			uint8_t *pBuff_dcmi = (uint8_t *) ( &dcmi_buff[ ONE_PACKAGE_SIZE * 0] );
		  	
			uint8_t *pBuff_x16 = pBuff_dcmi + USB_HEADER_SIZE+120*2*2;
			
			if(history_shutter_id==NUC_LOOP&&shutter_id==SHUTTER) 
			{
				HAL_DCMI_Stop(&hdcmi);
				HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)(X_buff), (FRAME_SIZE / 4));
			}
			
			if( shutter_id != IDLE )
			{								
				shutter_id=Shutter_State((uint16_t*)pBuff_x16,shutter_id,SPI_Reg_Data120,&Loop_Data120);
			}		
		
			history_shutter_id=shutter_id;
				
			if( manual_type != IDLE ) // do if manual shutter operation
			{
				
				DealShutterInterface(measureParam120.realTimeTfpa,measureParam120.realTimeTfpa, \
														measureParam120.realTimeTlen,measureParam120.realTimeTs,manual_type);
				manual_type = IDLE;
			}
			else
			{
				DealShutterInterface(measureParam120.realTimeTfpa,measureParam120.realTimeTfpa, \
														measureParam120.realTimeTlen,measureParam120.realTimeTs,manual_type);
			}		
			
			if( shutter_id == IDLE )	
			{				
				pImage=NUCbyTwoPoint((unsigned short*)pBuff_x16);
				
				InfraredProcessInterface((unsigned short *)pImage);//pImage:Y16 data
				
				MeasurePeriod++;
				if( MeasurePeriod >= 20 && shutter_id == IDLE && history_shutter_id == IDLE )
				{
					GetTempInfo( 0,0,120,90,( short* )pImage,Loop_Data120.us_avg_shutter,&IRInfo120 ,&measureParam120 );
					MeasurePeriod=0;
				}
				
				y16MappingADInterface(y8_buff, (short *)pImage,measureParam120.fliter[measureParam120.MeasureDegree].m_usExpectBright,measureParam120.fliter[measureParam120.MeasureDegree].m_usExpectContrast);
				
				//y8_to_rgb(y8_buff, (uint16_t *)pImage, g_colorbar_index);//pImage:RGB565 data
				
				memcpy(pBuff_x16, pImage, 120*90*2);
			}
			
			if ( g_nuc_start == 1 )
			{				
				if( shutter_id==IDLE )
					shutter_id = NUC_LOOP;
				
				g_nuc_start = 0;
			}
			
			if( shutter_id==IDLE )
			{
				pParamStatus->nuc_status = 0;
				pParamStatus1->nuc_status = 0;
				
				pParamStatus->shutter_status = 0;
				pParamStatus1->shutter_status = 0;
			}
			
		  if ( g_shutter_close != 0 )
			{
				if ( g_shutter_close == 2 )
				{
					if( shutter_id==IDLE )
					{
						g_shutter_close = 1;
						shutter_id = SHUTTER;
					}
					else
					{
						g_shutter_close = 0;
					}
				}
				else if ( g_shutter_close == 1 )
				{
					pParamStatus->shutter_status = 1;
					pParamStatus1->shutter_status = 1;
					g_shutter_close = 0;
				}
			}
			
			
			#if 1
			if ( g_upload_start == 1 )
			{
				g_upload_start = 0;

				BSP_W25Qx_Read_Sector(g_upload_pageID, upload_buff, g_upload_length);		
				if(g_upload_pageID == 490)	//2.0 ir-module
				{
					upload_buff[0] = 2;
				}
				int ret = usb_SDK_write((uint8_t *)(upload_buff), g_upload_length);
				
				if ( g_upload_length % 64 == 0)
				{
					osDelay(10);
					usb_SDK_write((uint8_t *)(upload_buff), 0);
				}
			}
			#endif
			
			else if ( g_paramline_start == 1 )
			{
				g_paramline_start = 0;
				usb_SDK_write((uint8_t *)pBuff_dcmi, USB_HEADER_SIZE);
			}
			
			else if ( g_update_start == 1 ) //烧写上位机数据至FALSH
			{
				if ( g_update_recv_len == g_update_length )
				{

					g_update_recv_len = 0;
					g_update_start = 0;
					BSP_W25Qx_Program_Sector(g_update_pageID, update_buff, g_update_length);
					
					pParamStatus->save_status = 0;
					pParamStatus1->save_status = 0;
			
				}
			}
			else if ( g_usb_start == 1 )
			{			
				
				if( shutter_id == IDLE )
				{
					pTempInfo->CenterPointX=IRInfo120.PostXCursor;
					pTempInfo->CenterPointY=IRInfo120.PostYCursor;
					pTempInfo->CenterPointtTem=IRInfo120.CursorValue*100;
					
					pTempInfo->MaxPointX=IRInfo120.PostXMax;
					pTempInfo->MaxPointY=IRInfo120.PostYMax;
					pTempInfo->MaxPointtTem=IRInfo120.MaxValue*100;
					
					pTempInfo->MinPointX=IRInfo120.PostXMin;
					pTempInfo->MinPointY=IRInfo120.PostYMin;
					pTempInfo->MinPointTem=IRInfo120.MinValue*100;
					
					usb_SDK_write(pBuff_dcmi, ONE_PACKAGE_SIZE);
				}
			}
			
			frame_count = 0;

			osDelay(1);
		
		}	
		
  }
}

//-----------------------------------------------------------------------DMA
void DMA_Copy(uint32_t DstAddress, uint32_t SrcAddress, uint32_t DataLength)
{
	DMA_HandleTypeDef     DmaHandle;
 
  DmaHandle.Instance = DMA2_Stream0;
	DmaHandle.Init.Request = DMA_REQUEST_MEM2MEM;
	
	DmaHandle.Init.Direction = DMA_MEMORY_TO_MEMORY;
	DmaHandle.Init.PeriphInc = DMA_PINC_ENABLE;
	DmaHandle.Init.MemInc = DMA_MINC_ENABLE;
	DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	DmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	DmaHandle.Init.Mode = DMA_NORMAL;
	DmaHandle.Init.Priority = DMA_PRIORITY_HIGH;
	DmaHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	DmaHandle.Init.MemBurst = DMA_MBURST_SINGLE;              /* Memory burst                     */
  DmaHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;           /* Peripheral burst */
  //*##-4- Initialize the DMA stream ##########################################*/
  if (HAL_DMA_Init(&DmaHandle)== HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
		if(HAL_DMA_Start(&DmaHandle,SrcAddress,DstAddress,DataLength)== HAL_OK)
		{
			HAL_DMA_PollForTransfer(&DmaHandle, HAL_DMA_FULL_TRANSFER, 200);
		}
  }	 
	
}

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	uint8_t *pBuff_dcmi = (uint8_t *) ( &dcmi_buff[ ONE_PACKAGE_SIZE * 0] );
	uint8_t *pBuff_dst = pBuff_dcmi + USB_HEADER_SIZE;
	if(frame_count==0)
	{
		//memcpy((void*)pBuff_dst, (void*)(X_buff), FRAME_SIZE);
		DMA_Copy((uint32_t)pBuff_dst, (uint32_t)(X_buff), FRAME_SIZE);
		frame_count =1;
		//HAL_DMA_Start_IT(&hdma_memtomem_dma1_stream1, (uint32_t)X_buff, (uint32_t)(pBuff_dst), FRAME_SIZE/4);
	}
}


void DMA_DMAXferCplt(DMA_HandleTypeDef *hdma)
{
		//frame_count++;
}

