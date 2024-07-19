/**
  ******************************************************************************
  * @file    usbd_SDK.c
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   This file provides the HID core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                SDK Class  Description
  *          ===================================================================
  *          
  *
  *
  *
  *           
  *      
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *           
  *      
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_SDK.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"
#include "usb_device.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

int usb_connect_ok(void)
{
	if (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED) 
		return -1;
	
	return 1;
}




static uint8_t g_usb_fifo_available = 1;
int usb_SDK_write(uint8_t* data, uint32_t length)
{
	if ( g_usb_fifo_available > 0 )
	{
		g_usb_fifo_available--;
		USBD_LL_Transmit(&hUsbDeviceFS, SDK_EP_ADDR, data, length);
		
		return 0;
	}
	
	return -1;
}

__weak void usb_SDK_read_cb(uint8_t *pbuf, uint16_t len)
{	
}



static void USBD_SDK_DCMI_Rx_Handle(USBD_HandleTypeDef *pdev, uint8_t *pbuf, uint16_t len);

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_SDK 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_SDK_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_SDK_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_SDK_Private_Macros
  * @{
  */ 
                                         
/**
  * @}
  */ 




/** @defgroup USBD_SDK_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_SDK_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx);

static uint8_t  USBD_SDK_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx);

static uint8_t  USBD_SDK_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req);

static uint8_t  *USBD_SDK_GetCfgDesc (uint16_t *length);

static uint8_t  *USBD_SDK_GetDeviceQualifierDesc (uint16_t *length);

static uint8_t  USBD_SDK_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_SDK_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_SDK_EP0_RxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_SDK_EP0_TxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_SDK_SOF (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_SDK_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_SDK_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

/**
  * @}
  */ 

/** @defgroup USBD_SDK_Private_Variables
  * @{
  */ 

USBD_ClassTypeDef  USBD_SDK_ClassDriver = 
{
  USBD_SDK_Init,
  USBD_SDK_DeInit,
  USBD_SDK_Setup,
  USBD_SDK_EP0_TxReady,  
  USBD_SDK_EP0_RxReady,
  USBD_SDK_DataIn,
  USBD_SDK_DataOut,
  USBD_SDK_SOF,
  USBD_SDK_IsoINIncomplete,
  USBD_SDK_IsoOutIncomplete,      
  USBD_SDK_GetCfgDesc,
  USBD_SDK_GetCfgDesc, 
  USBD_SDK_GetCfgDesc,
  USBD_SDK_GetDeviceQualifierDesc,
};

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
/* USB SDK device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_SDK_CfgDesc[64] __ALIGN_END =
{
  0x09, 													/*bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION, 		/*bDescriptorType: Configuration */
  64, 				/*wTotalLength: Bytes returned */
  0x00,
  0x02,         									/*bNumInterfaces: 2 interface*/
  0x01,         									/*bConfigurationValue: Configuration value*/
  USBD_IDX_CONFIG_STR,         		/*iConfiguration: Index of string descriptor describing the configuration*/
  0xC0,         									/*bmAttributes: bus powered */
  0x32,         									/*MaxPower 100 mA: this current is used for detecting Vbus*/
  /* 09 */
  0x09,         									/*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,				/*bDescriptorType: Interface descriptor type*/
  0x00,         									/*bInterfaceNumber: Number of Interface*/
  0x00,         									/*bAlternateSetting: Alternate setting*/
  0x02,         									/*bNumEndpoints*/
  0xFF,         									/*bInterfaceClass*/
  0xF0,         									/*bInterfaceSubClass*/
  0x00,         									/*nInterfaceProtocol*/
  USBD_IDX_INTERFACE_STR,					/*iInterface: Index of string descriptor*/
  /* 18 */
  0x07,          									/*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, 				/*bDescriptorType:*/
  0x82,     							/*bEndpointAddress: Endpoint Address (IN)*/
  0x02,          									/*bmAttributes: Bulk*/
  LOBYTE(64), /*wMaxPacketSize*/
  HIBYTE(64),
  0x00,          									/*bInterval: ignore for Bulk transfer */
	/* 25 */
  0x07,	         									/*bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,					/*bDescriptorType: */
  0x02,  								/*bEndpointAddress: Endpoint Address (OUT)*/
  0x02,														/*bmAttributes: Bulk */
  LOBYTE(64),	/*wMaxPacketSize*/
  HIBYTE(64),
  0x00,														/*bInterval: ignore for Bulk transfer */
  /* 32 */
  0x09,         									/*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,				/*bDescriptorType: Interface descriptor type*/
  0x01,         									/*bInterfaceNumber: Number of Interface*/
  0x00,         									/*bAlternateSetting: Alternate setting*/
  0x00,         									/*bNumEndpoints*/
  0xFF,         									/*bInterfaceClass*/
  0xF0,         									/*bInterfaceSubClass*/
  0x01,         									/*nInterfaceProtocol*/
  0x06,					/*iInterface: Index of string descriptor*/
  /* 41 */
  0x09,         									/*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,				/*bDescriptorType: Interface descriptor type*/
  0x01,         									/*bInterfaceNumber: Number of Interface*/
  0x01,         									/*bAlternateSetting: Alternate setting*/
  0x02,         									/*bNumEndpoints*/
  0xFF,         									/*bInterfaceClass*/
  0xF0,         									/*bInterfaceSubClass*/
  0x01,         									/*nInterfaceProtocol*/
  0x06,					/*iInterface: Index of string descriptor*/
  /* 50 */
  0x07,          									/*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, 				/*bDescriptorType:*/
  0x81,     							/*bEndpointAddress: Endpoint Address (IN)*/
  0x02,          									/*bmAttributes: Bulk*/
  LOBYTE(64), /*wMaxPacketSize*/
  HIBYTE(64),
  0x00,          									/*bInterval: ignore for Bulk transfer */
	/* 57 */
  0x07,	         									/*bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,					/*bDescriptorType: */
  0x01,  								/*bEndpointAddress: Endpoint Address (OUT)*/
  0x02,														/*bmAttributes: Bulk */
  LOBYTE(64),	/*wMaxPacketSize*/
  HIBYTE(64),
  0x00,														/*bInterval: ignore for Bulk transfer */
	
	/* 64 */
};
  
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_SDK_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */ 


#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN static uint8_t SDK_RxBuffer[SDK_HS_MAX_PACKET_SIZE] __ALIGN_END;
	
/** @defgroup USBD_SDK_Private_Functions
  * @{
  */ 

/**
  * @brief  USBD_SDK_Init
  *         Initialize the SDK interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_SDK_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx)
{
  uint8_t ret = 0;

  /* Open EP IN */
  USBD_LL_OpenEP(pdev, SDK_EPIN_ADDR, USBD_EP_TYPE_BULK, SDK_HS_MAX_PACKET_SIZE);  
  
  /* Open EP OUT */
  USBD_LL_OpenEP(pdev, SDK_EPOUT_ADDR, USBD_EP_TYPE_BULK, SDK_HS_MAX_PACKET_SIZE);
  
	/* Prepares endpoint for reception */
	USBD_LL_PrepareReceive(pdev, SDK_EP_ADDR, SDK_RxBuffer, SDK_HS_MAX_PACKET_SIZE);
    
  return ret;
}

/**
  * @brief  USBD_SDK_Init
  *         DeInitialize the SDK layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_SDK_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx)
{
  /* Close SDK EP IN */
  USBD_LL_CloseEP(pdev, SDK_EPIN_ADDR);
  
  /* Close SDK EP OUT */
  USBD_LL_CloseEP(pdev, SDK_EPOUT_ADDR);
	
  return USBD_OK;
}

/**
  * @brief  USBD_SDK_Setup
  *         Handle the SDK specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_SDK_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req)
{
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :  
    switch (req->bRequest)
    {
      
    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL; 
    }
    
		case USB_REQ_TYPE_STANDARD:
			switch (req->bRequest)
			{  
				case USB_REQ_SET_INTERFACE :
//				printf("## USBD_SDK_Setup select interface %d\r\n", (uint8_t)(req->wValue));
				break;
			}
			break;
		
		default:
			break;
  }
	
  return USBD_OK;
}


/**
  * @brief  USBD_SDK_GetCfgDesc 
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_SDK_GetCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_SDK_CfgDesc);
  return USBD_SDK_CfgDesc;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_SDK_DeviceQualifierDescriptor (uint16_t *length)
{
  *length = sizeof (USBD_SDK_DeviceQualifierDesc);
  return USBD_SDK_DeviceQualifierDesc;
}


/**
  * @brief  USBD_SDK_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */

static uint8_t  USBD_SDK_DataIn (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum)
{
  if (epnum == SDK_EP_ADDR)
	{
		g_usb_fifo_available++;
	}
		
  return USBD_OK;
}

/**
  * @brief  USBD_SDK_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_SDK_EP0_RxReady (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_SDK_EP0_TxReady
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_SDK_EP0_TxReady (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_SDK_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_SDK_SOF (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_SDK_IsoINIncomplete
  *         handle data ISO IN Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_SDK_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_SDK_IsoOutIncomplete
  *         handle data ISO OUT Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_SDK_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_SDK_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_SDK_DataOut (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum)
{
    uint32_t RxLength;
	
	  RxLength = USBD_LL_GetRxDataSize(pdev, epnum);

    if ( epnum == SDK_EP_ADDR )
        USBD_SDK_DCMI_Rx_Handle(pdev, SDK_RxBuffer, RxLength);
		
		return USBD_OK;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_SDK_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_SDK_DeviceQualifierDesc);
  return USBD_SDK_DeviceQualifierDesc;
}

/**
  * @}
  */ 

static void USBD_SDK_DCMI_Rx_Handle(USBD_HandleTypeDef *pdev, uint8_t *pbuf, uint16_t len)
{
	usb_SDK_read_cb(pbuf, len);
	
	/* Prepares endpoint for reception */
	USBD_LL_FlushEP(pdev, SDK_EP_ADDR);
	USBD_LL_PrepareReceive(pdev, SDK_EP_ADDR, SDK_RxBuffer, SDK_HS_MAX_PACKET_SIZE);
}


/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

