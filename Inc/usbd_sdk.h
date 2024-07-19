/**
  ******************************************************************************
  * @file    usbd_SDK_core.h
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   Header file for the usbd_SDK_core.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/ 
#ifndef __USB_SDK_CORE_H
#define __USB_SDK_CORE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_TEMPLATE
  * @brief This file is the header file for usbd_SDK_core.c
  * @{
  */ 


/** @defgroup USBD_SDK_Exported_Defines
  * @{
  */ 
	 
/**
 *  Below are endpoints usage:
 *    0x01/0x81:      camera payload / cmd;  prefix is SDK
 *    0x02/0x82:      SDK SDK authentication, not used for android; prefix is SDK2
 */
#define SDK_EP_ADDR                   0x01
#define SDK2_EP_ADDR                   0x02

#define SDK_EPIN_ADDR                 (0x80 | SDK_EP_ADDR)
#define SDK_EPOUT_ADDR                (0x00 | SDK_EP_ADDR)

#define SDK2_EPIN_ADDR                 (0x80 | SDK2_EP_ADDR)
#define SDK2_EPOUT_ADDR                (0x00 | SDK2_EP_ADDR)

#define SDK_HS_MAX_PACKET_SIZE        64//512

#define USB_SDK_CONFIG_DESC_SIZ       64


#define SDK_USBD_VID                     0x0525
#define SDK_USBD_PID                     0xa4a0
#define SDK_USBD_LANGID_STRING           1033
#define SDK_USBD_MANUFACTURER_STRING     "Wuhan Guide Infrared Co., Ltd."
#define SDK_USBD_PRODUCT_STRING          "MobIR Air"
#define SDK_USBD_CONFIGURATION_STRING    "SDK Configure"
#define SDK_USBD_INTERFACE_STRING        "SDK Interface"




extern char SDK_USBD_SERIALNUMBER_STRING[15];
/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */

/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 

extern USBD_ClassTypeDef  USBD_SDK_ClassDriver;
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */ 
/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif  /* __USB_SDK_CORE_H */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

