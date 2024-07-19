#ifndef __MEASUREINERFACE_H
#define __MEASUREINERFACE_H

#include <stdint.h>

//²âÎÂ·ÖµµÎ»²ÎÊý
typedef struct MEASURE_Fitler_PARAM 
{
	int kj;                     //user should avoid  to update
	short b;                    // kf and b is used to adjust tem in 0.5m distance,user should set distance in 0.5m(5) before to adjust  kf and b
	short b2;                   // kf2 and b2 is used to adjust tem in 1.2m distance,user should set distance in 1.2m(12) before to adjust  kf2 and b2
	short kf;    
	short kf_2;  
	short kf_2_distance;       //user should avoid  to update,keep 12
	short kf_2_base_distance; //user should avoid  to update,keep 5
	int ks;                  //user should avoid  to update,keep 0
	 
	int k0;                  //user should avoid  to update,keep 0
	int k1;                  //user should avoid  to update,keep 0
	int k2;                  //user should avoid  to update,keep 0
	int k3;                  //user should avoid  to update,keep 0
	int k4;                  //user should avoid  to update,keep 0
	int k5;                  //user should avoid  to update,keep 0 
	//END
	short tref;//????????  //user should avoid  to update,keep 0

	float fCompensateA1;  //user should avoid  to update
	float fCompensateA2;  //user should avoid  to update
	float fCompensateB1;  //user should avoid  to update
	float fCompensateB2;  //user should avoid  to update
	float fCompensateC1;  //user should avoid  to update
	float fCompensateC2;  //user should avoid  to update
	unsigned short m_usExpectBright;   //image Bright set by user when use DRT,can adjust by real condition
	unsigned short m_usExpectContrast; //image Contrast set by user when use DRT,can adjust by real condition
	unsigned short DistanceCorrectionOn; 
}Msr_Flt_PRM;


typedef struct MEASURE_PARAM_PUBLIC {
	short realTimeTs;//ÊµÊ±¿ìÃÅÎÂ,should update by user   
	short realTimeTfpa; // ÊµÊ±½¹ÎÂ,should update by user
	short realTimeTlen; //ÊµÊ±¾µÍ²ÎÂ,should update by user
	short lastShutterTs;  //ÉÏ´Î¿ìÃÅÎÂ,user should avoid  to update
	short lastShutterTfpa;//ÉÏ´Î????,user should avoid  to update
	short lastShutterTlen;//,user should avoid  to update
	short currentGear;       // only update by sdk
	short MeasureRealGear;  //Êµ¼ÊµµÎ»,1,2,only update by sdk,user only can get 
	short MeasureDegree;   //ÒªÇó²âÎÂµµÎ»,1,2,3,user should avoid to update it 

	short emiss;      //·¢ÉäÂÊ,zoom 100,,can set by user range from 0 to 100 
	short hum;        //reservered
	short distance;   //¾àÀë,zoom 10,can set by user range fron 5 to 12,other value maybe no use
	short reflectT;    //·´ÉäÎÂ¶,È          ,can set by user,typical value 23
	short transmissivity; //reservered

	Msr_Flt_PRM fliter[2]; 

}MeasureParamC;


typedef struct{
	unsigned short Width;           //image Width
	unsigned short height;          //image height   
	unsigned short PostXMin;        //Min Temp x postion,only update by sdk
	unsigned short PostYMin;        //Min Temp y postion,only update by sdk
	unsigned short PostXMax;        //Max Temp x postion,only update by sdk
	unsigned short PostYMax;        //Max Temp y postion,only update by sdk  
	unsigned short PostXCursor;     //user point  x postion,can update by user 
	unsigned short PostYCursor;     //user point  y postion,can update by user
	float MinValue;                 //min Temp output by sdk
	float MaxValue;                 //max Temp output by sdk
	float CursorValue;              //user point Temp output by sdk
} IRInfo;

typedef struct{
	uint32_t CompensateNUCVtemp; //*1000      nuc shuttter control,should adjust by real condition
	uint32_t CompensateShuttterVtemp; //*1000 normal shuttter control,should adjust by real condition
	uint32_t AutoShutterInterval;  //³¡Êý     autotime normal shutter control,should adjust by real condition
	uint32_t MC_Value;   //MCU MC value output to mco1
	float LowKj;   // low degree kj param,should adjust by real condition
	float HighKj;  // high degree kj param,should adjust by real condition
}ShutterPara;

extern void  MEASURE_ALLOC_AssignMemory(void * p, uint32_t NumBytes);  //Memory alloc to run Measurelib
extern void InitMeasurePara(ShutterPara*Para);  
extern char SetMeasureDegree(uint8_t degree);  
extern void DealShutterInterface( short jpmT, short realTimeJpmTemp,short realTimelensTemp,short realTimeShutterTemp,unsigned char ManualType);
extern void GetTempInfo(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,short*yData,short us_avg_shutter,IRInfo*info,MeasureParamC*measureParam);
extern uint64_t GetMeasureLibVersion(void);
extern float GetPointTempByY16(unsigned short Y16Temp,MeasureParamC*measureParam);
#endif