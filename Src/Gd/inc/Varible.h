#ifndef __VARIBLE_H
#define __VARIBLE_H
#include <stdint.h>

#include "Sensor.h"
#include "MeasureInterface.h"
#include "interface.h"

extern uint8_t shutter_id,history_shutter_id;
extern uint8_t ImageReady;	

extern int16_t*pImage;
extern SPI_Reg_Data SPI_Reg_Data120[21];

extern Loop_Data Loop_Data120;
extern MeasureParamC measureParam120;
extern IRInfo IRInfo120;

extern ShutterPara Para120;
extern uint8_t MeasureMem[42*1024];	
extern uint8_t AlgMem[132*1024];
extern AlgPara AlgPara120;
extern unsigned char manual_type ;
#endif
