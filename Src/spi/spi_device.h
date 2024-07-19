#ifndef __SPI_DEVICE_H
#define __SPI_DEVICE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal_def.h"
#include "ir_def.h"
	 
extern int sensor_reg_read(uint8_t reg_addr, uint8_t * val);	
extern int sensor_reg_write(uint8_t reg_addr, uint8_t val);	 
	 
extern int sensor_init(PARAM_DETECT *p_config);
extern int sensor_reg_dump(uint8_t *reg_buf);
	 
extern int INT_time_set(uint8_t us);
	 

#ifdef __cplusplus
}
#endif

#endif  /* __SPI_DEVICE_H */


