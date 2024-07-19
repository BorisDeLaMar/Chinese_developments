#ifndef __IR_DEF_H
#define __IR_DEF_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "stm32h7xx_hal.h"

#define MUTILPEL_BUFFER_NUM (2)

#define WIDTH (120)
#define HEIGHT (92)

#define USB_HEADER_SIZE ( WIDTH * 2 )

#define PIX_SIZE ( WIDTH * HEIGHT )
#define FRAME_SIZE ( WIDTH * HEIGHT * 2 )

	 
#define ONE_PACKAGE_SIZE	(FRAME_SIZE + USB_HEADER_SIZE)


typedef struct
{
	uint16_t header;
	uint16_t version;
	uint16_t width;
	uint16_t height;
	uint16_t name[4];
	uint16_t temper_shutter_start;
	uint16_t temper_shutter_realtime;
	uint16_t temper_len_realtime;
	uint16_t temper_dectect;
	uint16_t shutter_status;
	uint16_t nuc_status;
	uint16_t save_status;
	uint16_t detect_page;
}PARAM_STATUS;

typedef struct
{
	uint16_t Detect_gear;
	uint16_t INT_value;
	uint16_t RES_low;
	uint16_t RES_high;
	uint16_t GFID_low;
	uint16_t GFID_high;
	uint16_t NUC_low;
	uint16_t NUC_high;
	uint16_t reserved[8];
	uint16_t detect_table[16];
}PARAM_DETECT;


typedef struct
{
	uint8_t  CenterPointX;
	uint8_t  CenterPointY;
	int16_t CenterPointtTem;
	uint8_t  MaxPointX;
	uint8_t  MaxPointY;
	int16_t MaxPointtTem;
	uint8_t  MinPointX;
	uint8_t  MinPointY;
	int16_t MinPointTem;	
}TemInfo;

enum
{
	NTC_LENS = 0,
	NTC_SHUTTER,
	NTC_VTEMP,
	NTC_NONE,
};


extern PARAM_STATUS *pParamStatus;
extern PARAM_STATUS *pParamStatus1;

extern uint16_t *pParamUser;
extern uint16_t *pParamUser1;

extern TemInfo*pTempInfo;


#define PARAM_STATUS_LEN 	(32)
#define PARAM_DETECT_LEN 	(64)
#define PARAM_USER_LEN 		(32)
#define PARAM_EXPERT_LEN 	(96)
#define PARAM_MAX_LEN 	  (96)

#define PARAM_STATUS_OFFSET (0)
#define PARAM_DETECT_OFFSET (PARAM_STATUS_OFFSET + PARAM_STATUS_LEN)
#define PARAM_USER_OFFSET 	(PARAM_DETECT_OFFSET + PARAM_DETECT_LEN)



#define PAGE_USER_START 	(273)
#define PAGE_SN_START 		(489)

#define PAGE_LOW_TEMP_PARAM (360)            
#define PAGE_HIGH_TEMP_PARAM (400)

#define BYTE_PER_PAGE (0x1000)


#define ARM_VERSION (40)
#define DEVICE_NAME "ZM01B"


extern int SetDectectValue(uint16_t offset, uint16_t value);
extern int params_init(uint8_t *buffer);
extern void shutter_open(void);
extern void shutter_close(void);

#ifdef __cplusplus
}
#endif

#endif  /* __IR_DEF_H */
