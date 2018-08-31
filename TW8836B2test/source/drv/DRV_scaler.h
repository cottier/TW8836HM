/**
  ******************************************************************************
  * @file    template.h
  * @author
  * @version V1.0.0
  * @date    31-May-2018
  * @brief   Header file of TEMPLATE.
  ******************************************************************************
  * @attention
 
  *
  ******************************************************************************
  */

/**
  ******************************************************************************
  * History:
  ******************************************************************************
  * Version 1.00
  *   Initial release
  *
  ******************************************************************************
  */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRV_SCALER_H__
#define __DRV_SCALER_H__
/* Includes ------------------------------------------------------------------*/
#include "HWdef.h"


/* Exported types ------------------------------------------------------------*/

struct DIGIT_VIDEO_TIME_TABLE_s {
	BYTE vid; 		//DMT id. if bit7 is high, we donot support.

	WORD hActive;	//horizontal resoultion
	WORD vActive;	//vertical resoultion
	BYTE vFreq;		//vertical frequency

	BYTE pol;		//hPol[7] vPol[6] IorP[0].

	WORD hTotal;	//horizontal period
	WORD hFPorch;	//horizontal Front Porch
	WORD hSync;		//horizontal Synch width
	WORD hBPorch;	//horizontal Back Porch

	WORD vTotal;	//vertical period
	BYTE vFPorch;	//vertical Front Porch
	BYTE vSync;		//vertical Synch Width
	BYTE vBPorch;	//vertical Back Porch

	char vOffset;	//-128 to 127. used for Big Panel
	//hFreq:kHz
	//PixelFreq:MHz

	//BYTE hOverScan;
	//BYTE vOverScan;
	//BYTE REMOVED__pclko_pol;
	DWORD pixelfreq;

};

struct SCALER_TIME_TABLE_s {
    WORD hActive;	//horizontal resoultion
    WORD vActive;	//vertical resoultion
    BYTE vFreq;		//vertical frequency
	BYTE vClone;	//for 59&60Hz

	BYTE vSyncWidth;
	BYTE vBPorch;

    WORD hScale;        //h scale value depend on Up/DOWN.
    WORD vScale;        //v scale value.

    WORD hLen;          //crop h active length
    BYTE hSync;
    WORD hStart;        //crop h start(Positive:sync+bporch or Negative:bporch)
    WORD hBuff;         //scaler buffer length (max 1366)

    WORD vLen;          //crop v active length 
    BYTE vSync;
    BYTE vStart;        //crop v start(Positive:sync+bporch or Negative:bporch)
    WORD vDE;           //output vDE position					

    BYTE pol;           //[7] reserved
                        //[6] BT656Enc_output is I....
                        //[5] BT656Enc		REG040[5] 1: high active. (VDCLK_POL)
                        //[4] BT656Dec		REG047[5] 1:Invert. (DTVCKP)
                        //[3] input vSync. REG041[3] 1:falling	  
                        //[2] input hSync. REG041[2] 1:falling
                        //[1] input field. REG041[1] 1:inversion
                        //[0] input clock. REG040[4] 1:Inversion 						
						
	WORD sspll2;		//base 10kHz, if 552.750MHz, write 55275.
    BYTE div;           //[7:4] pclk divider => REG0F6[4]REG0F6[2:0]
                        //[3]   pclko polarity => REG20D[4]  1:Inversion =>BK141014. use table.
                        //[2:0] pclko divider =>ocnvert=> REG20D[5]REG20D[1:0]	
};

/* Exported macro ------------------------------------------------------------*/
#define PANEL_H 800
#define PANEL_V 480

#define HPOL_P		0x40	//Horizontal Polarity Positive. Active High.   _|-|___.
#define HPOL_N		0x00	//Horizontal Polarity Negative. Active Low.    -|_|---.

#define VPOL_P		0x80
#define VPOL_N		0x00

#define INTERLACED	0x01

#define P_HN_VN		HPOL_N | VPOL_N
#define P_HP_VP		HPOL_P | VPOL_P	
#define P_HN_VP		HPOL_N | VPOL_P
#define P_HP_VN		HPOL_P | VPOL_N
#define I_HN_VN		HPOL_N | VPOL_N | INTERLACED
#define I_HP_VP		HPOL_P | VPOL_P | INTERLACED	
#define I_HN_VP		HPOL_N | VPOL_P | INTERLACED
#define I_HP_VN		HPOL_P | VPOL_N | INTERLACED
/* Exported constants --------------------------------------------------------*/
extern code struct DIGIT_VIDEO_TIME_TABLE_s CEA861_table[];
extern code struct DIGIT_VIDEO_TIME_TABLE_s TW8836_VESA_TABLE[];
extern code struct SCALER_TIME_TABLE_s SCALER_DTV_time[];

/* Exported functions --------------------------------------------------------*/
void ScalerSetHScaleReg(WORD down, WORD up);
struct DIGIT_VIDEO_TIME_TABLE_s *Find_CEA861_VideoTable(WORD hActive,WORD vActive,BYTE vFreq);
struct SCALER_TIME_TABLE_s *FindScalerTable(BYTE _InputMain,WORD hActive,WORD vActive,BYTE vFreq, BYTE vStart, BYTE vPol);

void ScalerSetLineBufferSize(WORD len);
WORD ScalerReadVDEReg(void);
void ScalerWriteVDEReg(WORD pos);
void ScalerSet_vDE_value(BYTE vStart);
void ScalerSetHScale(WORD Length)	;
void ScalerSetVScale(WORD Length)	;
WORD ScalerReadVScaleReg(void);
void ScalerWriteHDEReg(WORD pos);
void ScalerPanoramaOnOff(BYTE fOn);
WORD ScalerCalcHDE(void);
HAL_StatusTypeDef Scalerinit( void);
/** ************************************************************************* */
#endif /* __TEMPLATE_H */
/**
  *****************************************************************************
  * END OF FILE                                                               *
  *****************************************************************************
  */
