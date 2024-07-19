#ifndef __SENSOR_H
#define __SENSOR_H
#include "SPI.h"

#include "Shutter.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define  IDLE         0
#define  SHUTTER      1
#define  NUC_LOOP     2
#define  GFID_LOOP    3
#define  SEL_LOOP     4

typedef struct  
{
	const unsigned char Addr;
	unsigned char Reg_Value;
}SPI_Reg_Data;


typedef struct  
{
	unsigned short GFID_High;
	unsigned short GFID_Low;
	
	unsigned short RES_High;
	unsigned short RES_Low;
	
	unsigned short NUC_High;
	unsigned short NUC_Low;
	
	unsigned char  ONTime;
	unsigned char  OFFTime;
	unsigned char  HoldTime;
	unsigned char Is_Shutter;
	unsigned short us_avg_shutter;
	unsigned  char IsFirstNUC;
}Loop_Data;

extern  short*NUCbyTwoPoint(unsigned short*pus_src_data);
extern unsigned int GetVersion(void);
extern unsigned short* GetParamAddr(void);
extern void Sensor_Config(unsigned char addr,unsigned char dat);
extern void SensorWriteReg(unsigned char addr,unsigned char dat);
extern unsigned char SensorReadReg(unsigned char addr);
extern void  Sensor_Reg_Initial(SPI_Reg_Data*Reg_Data120);

extern unsigned char  Shutter_State(unsigned short*pus_data,unsigned char Shutter_id,SPI_Reg_Data*Reg_Data120,Loop_Data*Loop_Data120);
//À„∑®¿‡
extern void MemoryAlloc(void);

extern void DebugPrint(char*formt,...);

#endif
