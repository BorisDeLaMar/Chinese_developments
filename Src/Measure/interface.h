#ifndef  __INTERFACE_H_
#define  __INTERFACE_H_
#include <stdint.h>
typedef struct {

	uint16_t m_us;      // input image  Height
	uint16_t m_usWidth; // input image  Width
	
	/** Ëã·¨¿ª¹Ø 	0-off  1- on **/
  uint8_t uWeightTemporalFilterSwitch;
	uint8_t uGrayFilterSwitch;
	uint8_t uLaplaceSharpenSwitch; //reservered
	uint8_t uLinearDrtSwitch;      //reservered
	
	short*g_indexAni;       //reservered
	short*g_indexTime;      //reservered
	uint8_t m_ucGrayStd_l;  //TemporalFilter param for low degree,should adjust by real condition,range from 5 to 30
	uint8_t m_ucGrayStd_h;  //TemporalFilter param for high degree,should adjust by real condition,range from 5 to 30
	uint8_t m_ucSpatialGrayStd; //uGrayFilterSwitch param,,range from 5 to 20
}AlgPara;


extern void AlgInit(AlgPara AlgParaValue);

extern void  ALG_ALLOC_AssignMemory(void * p, uint32_t NumBytes); //Memory alloc to run ALGlib

extern void InfraredProcessInterface(uint16_t *uspSrcImage);
extern void LinearDrtInterface(uint8_t *ucpDstImage, uint16_t *uspSrcImage);

extern void gdNeighborResizeImage(unsigned char  *pSrc, unsigned char  *pDst, int nWidth, int nHeight, int nZoomWidth, int nZoomHeight);
extern void bilinera_interpolation(unsigned char *in_array, int height, int width, unsigned char *out_array, int out_height, int out_width);

extern void YToRGB565(unsigned char * pSrc,unsigned short * pDst,unsigned char index);
extern void y16MappingADInterface( unsigned char *grayData, short *y16Data,unsigned short Bright,unsigned short Contrast);
extern void MyNUCbyTwoPoint(unsigned short *pus_dst_data, unsigned short *pus_src_data, unsigned short *pus_gain_mat, unsigned short *pus_high_base, unsigned short *pus_low_base, unsigned short *pus_shutter_base, int n_width, int n_height, int b_get_k_gain);
extern void y8_to_rgb(uint8_t *y8_buf, uint16_t *rgb_buf, uint8_t color_bar_index);
#endif

