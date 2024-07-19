#include "main.h"
#include "SensorConfig.h"
#include "Varible.h"

extern SPI_Reg_Data SPI_Reg_Data120[21];

//≈‰÷√INT
void INT_Configuration(uint32_t Val) 
{

}

void SensorInit(void)
{
	Sensor_Reg_Initial(SPI_Reg_Data120);
}

void ReConfig(void)
{

}
