#include "stm32h7xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"

#include "spi_device.h"
#include "ir_def.h"

#include "W25QXX.h"

void shutter_close()
{
	HAL_GPIO_WritePin(DRV8837_SLEEP_GPIO_Port, DRV8837_SLEEP_Pin, GPIO_PIN_SET);
	
	HAL_GPIO_WritePin(SHUTTER_INTA_GPIO_Port, SHUTTER_INTA_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SHUTTER_INTB_GPIO_Port, SHUTTER_INTB_Pin, GPIO_PIN_RESET);
	
	osDelay(10);
	HAL_GPIO_WritePin(DRV8837_SLEEP_GPIO_Port, DRV8837_SLEEP_Pin, GPIO_PIN_RESET);
}

void shutter_open()
{
	HAL_GPIO_WritePin(DRV8837_SLEEP_GPIO_Port, DRV8837_SLEEP_Pin, GPIO_PIN_SET);
	
	HAL_GPIO_WritePin(SHUTTER_INTA_GPIO_Port, SHUTTER_INTA_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SHUTTER_INTB_GPIO_Port, SHUTTER_INTB_Pin, GPIO_PIN_SET);
	
	osDelay(10);
	HAL_GPIO_WritePin(DRV8837_SLEEP_GPIO_Port, DRV8837_SLEEP_Pin, GPIO_PIN_RESET);
}

int SetDectectValue(uint16_t offset, uint16_t value)
{
	uint16_t *p_temp;

	*(p_temp + offset) = value;
	*(p_temp + offset) = value;
	
	if ( ( offset >= 0x10 ) && ( offset <= 0x1B ) )
	{
		sensor_reg_write(offset, value);

	}
	else if ( offset == 0x01 )
	{
		INT_time_set(value);
	}
	else if ( offset == 0x0 )
	{
	}
	
	return 0;
}

PARAM_STATUS *pParamStatus;
PARAM_STATUS *pParamStatus1;

uint16_t *pParamUser;
uint16_t *pParamUser1;

TemInfo*pTempInfo;

uint16_t g_param_init_done = 0;
uint16_t g_start_tmp_read_done = 0;

char SDK_USBD_SERIALNUMBER_STRING[15];

extern uint8_t new_param[PARAM_MAX_LEN];

int params_init(uint8_t *buffer)
{	
	pParamStatus = (PARAM_STATUS *)(buffer + PARAM_STATUS_OFFSET);
	pParamStatus1 = (PARAM_STATUS *)(buffer + ONE_PACKAGE_SIZE + PARAM_STATUS_OFFSET);
	
	pParamUser = (uint16_t *)(buffer + PARAM_USER_OFFSET);
	pParamUser1 = (uint16_t *)(buffer + ONE_PACKAGE_SIZE + PARAM_USER_OFFSET);
	
	g_param_init_done = 1;	
	
	pTempInfo=(TemInfo*)(pParamUser+6);
	//fill user line
	BSP_W25Qx_Read_Sector(PAGE_USER_START, new_param, PARAM_USER_LEN);
	memcpy(pParamUser, (uint8_t *)new_param, PARAM_USER_LEN);
	memcpy(pParamUser1, pParamUser, PARAM_USER_LEN);
	
	//wait for startup temperature read done
	while ( g_start_tmp_read_done == 0 )
	{
		osDelay(100);
	}
	
	//fill param line changed by mcu
	//memset(pParamStatus, 0, sizeof(PARAM_STATUS));
	pParamStatus->header = 0xaa55;
	pParamStatus->version = ARM_VERSION;
	pParamStatus->width = WIDTH;
	pParamStatus->height = HEIGHT;
	char str_name[] = DEVICE_NAME;
	memset(pParamStatus->name, 0, 8);
	memcpy(pParamStatus->name, str_name, sizeof(str_name));
	pParamStatus->save_status = 0;
	pParamStatus->detect_page = 0;
	memcpy(pParamStatus1, pParamStatus, PARAM_STATUS_LEN);
	
	//SN number
	BSP_W25Qx_Read_Sector(PAGE_SN_START, new_param, 14);
	memset(SDK_USBD_SERIALNUMBER_STRING, 0, sizeof(SDK_USBD_SERIALNUMBER_STRING));
	memcpy(SDK_USBD_SERIALNUMBER_STRING, (uint8_t *)new_param, 14);
	
	if ( SDK_USBD_SERIALNUMBER_STRING[0] != 'Z' )
	{
		memset(SDK_USBD_SERIALNUMBER_STRING, 0, sizeof(SDK_USBD_SERIALNUMBER_STRING));
		SDK_USBD_SERIALNUMBER_STRING[0] = 'Z';
		SDK_USBD_SERIALNUMBER_STRING[1] = 'M';
		SDK_USBD_SERIALNUMBER_STRING[2] = '0';
		SDK_USBD_SERIALNUMBER_STRING[3] = '1';
		SDK_USBD_SERIALNUMBER_STRING[4] = 'B';
		SDK_USBD_SERIALNUMBER_STRING[5] = '1';
		SDK_USBD_SERIALNUMBER_STRING[6] = '9';
	}
	
	return 0;
}


	


