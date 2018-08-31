/**
  ******************************************************************************
  * @file    TEMPLATE.c
  * @author
  * @version V1.0.0
  * @date    31-May-2018
  * @brief   Source file of TEMPLATE.
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

/* Includes ------------------------------------------------------------------*/
#include "HWconf.h"

/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

#define XSCALE_DN_BASE	0x0400L
#define XSCALE_UP_BASE	0x2000L	
#define YSCALE_BASE		0x2000L
#define PXSCALE_BASE	0x0800L

#define SCALER_HDE_BASE				0x30
#define SCALER_HDELAY2_BASE			0x10
#define SCALER_MAX_LINEBUFF_SIZE	1366

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** --------------------------------------------------------------------------*
  * @fn
  * @brief
  * @param[in]
  * @return     None
  * --------------------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
* find CEA861 Video time table
*/

/** --------------------------------------------------------------------------*
  * @fn         
  * @brief      
  * @param[in]  
  * @return     None
  * --------------------------------------------------------------------------*/
struct DIGIT_VIDEO_TIME_TABLE_s *Find_CEA861_VideoTable(WORD hActive,WORD vActive,BYTE vFreq)
{
	struct DIGIT_VIDEO_TIME_TABLE_s *p;

	p = CEA861_table;
	while(p->hActive != 0xFFFF) {
		if(p->hActive==hActive || p->hActive==(hActive+1)) {
			if(p->vActive==vActive) {
				if(p->vFreq == vFreq || p->vFreq == (vFreq+1)) {
#ifdef DEBUG_BT656
					Printf("\nFind CEA861 video %dx%d%s@%02bd",p->hActive,p->vActive, p->pol & INTERLACED ? "I":"P",p->vFreq);
					Printf("  %d:%d,%d,%d  %d:%bd,%bd,%bd",
						p->hTotal,p->hFPorch, p->hSync, (WORD)p->hBPorch,  
						p->vTotal,p->vFPorch, p->vSync, p->vBPorch
						);
#endif
					return p; 
				} 
			}
		}
		p++;
	}
	//fail.., try VESA
	p = TW8836_VESA_TABLE;
	while(p->hActive != 0xFFFF) {
		if(p->hActive==hActive || p->hActive==(hActive+1)) {
			if(p->vActive==vActive) {
				if(p->vFreq == vFreq || p->vFreq == (vFreq+1)) {
#ifdef DEBUG_BT656
					Printf("\nFind VESA video %dx%d%s@%02bd",p->hActive,p->vActive, p->pol & INTERLACED ? "I":"P",p->vFreq);
					Printf("  %d:%d,%d,%d  %d:%bd,%bd,%bd",
						p->hTotal,p->hFPorch, p->hSync, (WORD)p->hBPorch,  
						p->vTotal,p->vFPorch, p->vSync, p->vBPorch
						);
#endif
					return p; 
				} 
			}
		}
		p++;
	}
	//fail
	Printf("\nFind_CEA861_VideoTable fail %dx%d@%bd", hActive,vActive,vFreq);

	return NULL;
}


//-----------------------------------------------------------------------------
/**
* description
* Horizontal Down/Up Scale register.
* Scaler do downscale first, and then do upscale.
*
* The down scaling ratio is defined as 400h / DOWN_SCALE.
*	register
*	R20A[3:0]R209[7:0]	X-Down. 12bit
*
* The up scaling ratio is defined as 2000h / UP_SCALE.
*	register
*	R204[7:0]R203[7:0]	XSCALE UP. 16bit
*/
void ScalerSetHScaleReg(WORD down, WORD up)
{
	WriteTW88(REG20A, (ReadTW88(REG20A) & 0xF0) | (down >> 8));
	WriteTW88(REG209, (BYTE)down);

	WriteTW88(REG204, (BYTE)(up>>8));		
	WriteTW88(REG203, (BYTE)up);
}
/** --------------------------------------------------------------------------*
  * @fn         
  * @brief      
  * @param[in]  
  * @return     None
  * --------------------------------------------------------------------------*/
HAL_StatusTypeDef Scalerinit( void)
{
	//input
	//Hstart,Hlen,Vstart,Vlen,VSdelay
	WriteTW88(REG042,0x34);
	WriteTW88(REG043,0x15);
	WriteTW88(REG044,0x20);
	WriteTW88(REG045,0xBF);
	WriteTW88(REG046,0x80);
	WriteTW88(REG04B,0x11);
	//scaler
	
	//output
	return HAL_OK;
}



//-----------------------------------------------------------------------------
//VA_POS	REG220[5:4]R215[7:0]		VDE pos 
//			Output DE position control relative to the internal reference in number of output lines
//VA_LEN	R217[3:0]R216[7:0]	  width
//			Output DE control in number of the output lines. A 12-bit register
/**
* Read Vertical DE register
*/
WORD ScalerReadVDEReg(void)
{
	WORD wTemp;
	wTemp = ReadTW88(REG220) & 0x30;	wTemp <<= 4;
	wTemp |= ReadTW88(REG215);
	return wTemp;
}
//-----------------------------------------------------------------------------
/**
* Write Vertical DE register
*/
void ScalerWriteVDEReg(WORD pos)
{
	//dPrintf("\n\rScalerSetVDEAndWidth pos:%bd len:%d",pos,len);
	WriteTW88(REG220, (ReadTW88(REG220) & 0xCF) | ((pos >> 4) & 0x30));
	WriteTW88(REG215, (BYTE)pos);
}


//-----------------------------------------------------------------------------
/**
* calculate & write vDEReg value.
*
* simple Formula is 	vDE = (DWORD)vStart * PANEL_V / vActive;
* but, it is unaccurate and people does not like it.
* read real vscaler value and calculate.
*/
void ScalerSet_vDE_value(BYTE vStart)
{
	WORD wTemp;
	DWORD dTemp;

	wTemp = ScalerReadVScaleReg();
	dTemp = vStart;
	dTemp <<= 13;  //*= 8192;
	dTemp /= wTemp;
	ScalerWriteVDEReg((WORD)dTemp);
	Printf("\nvStart:%bd *8192/%d => vDE:%d",vStart,wTemp,(WORD)dTemp);
}

//-----------------------------------------------------------------------------
/**
* set Horizontal DE position(DEstart) & length(active, DEwidth).
*
*	REG221[5:4]R210[7:0]	HA_POS
*		Output DE position control relative to the internal reference in number of output clock
* set the "DEstart" and "DEwidth" on Horizontal Timming Flow.
*
* This register is related with HDELAY2.
* We have to control it together.
*/
void ScalerWriteHDEReg(WORD pos)
{
	//FYI: DO NOT add debugmsg, it will makes a blink.
	//dPrintf("\n\rScalerWriteHDEReg pos:%bd",pos);
	WriteTW88(REG221, (ReadTW88(REG221) & 0xCF) | ((pos >> 4) & 0x30));
	WriteTW88(REG210, (BYTE)pos);
}


//-----------------------------------------------------------------------------
/**
* Read scaler LineBuffer output delay
*
* @see ScalerWriteLineBufferDelay
*/
BYTE ScalerReadLineBufferDelay(void)
{
	return ReadTW88(REG20B);
}
//-----------------------------------------------------------------------------
/**
* Calculate HDE value.
*
*	method
*	Buffer_Delay = REG(0x20b[7:0])
*	result = Buffer_Delay + 32
*/
WORD ScalerCalcHDE(void)
{
	WORD wTemp;
	wTemp = ScalerReadLineBufferDelay();
	return wTemp+32;
}

/**
* Write scaler LineBuffer output delay
*
* HDE is related with this delay value.
*
*	register
*	R20B
*	R20B[7:0]			HDELAY2	
*		Scaler buffer data output delay in number of pixels in relation to the H sync.
*/
void ScalerWriteLineBufferDelay(BYTE delay)
{
	WriteTW88(REG20B, delay);
}
//-----------------------------------------------------------------------------
/**
* set Scaler OutputLength that is related with the line buffer size. 
*
* max 1024.
* 
*	register
*	R20E[6:4]:R20C[7:0]		HACTIVE
*		Scaler data output length in number of pixels. 
*		A 10-bit register.==>11
*/
void ScalerSetLineBufferSize(WORD len)
{
	//TW8836B uses +1 or +2 values.
	if(len> (PANEL_H +2))
		len=PANEL_H;

	WriteTW88(REG20E, (ReadTW88(REG20E) & 0x8F) | ((len & 0x700) >> 4));
	WriteTW88(REG20C, (BYTE)len);
}
WORD ScalerGetLineBufferSize(void)
{
	WORD wTemp;
	wTemp = ReadTW88(REG20E) & 0x70;
	wTemp <<=4;
	wTemp |= ReadTW88(REG20C);
	return wTemp;
}
//-----------------------------------------------------------------------------
//internal
/**
* set Horizontal Scaler value on FULL mode
*
* @see ScalerSetHScale
*/
void ScalerSetHScale_FULL(WORD Length)	
{
	DWORD	dTemp;

	//BK130131. Now HDE need some min value, because FOSD DE.
	//LineBuffDelay also related with HDE.
	//I temporary increase it.

	ScalerWriteLineBufferDelay(SCALER_HDELAY2_BASE);
	ScalerPanoramaOnOff(OFF);

	//if SCALER_MAX_LINEBUFF_SIZE < PANEL_H, we have to use SCALER_MAX_LINEBUFF_SIZE.
	if(PANEL_H >= Length) { 
		//-----------------------								
		//UP SCALE
		//I don't want to add a garbage on the right side.
		//use a round down value, so image will be scaled up and cut on the right side.
		//for example
		//	input:720	output:800
		//
		//	720*8192/hScale=800
		//	hScale = 720*8192/800
		//	hScale = 7372.8
		//	So, we will use 7373 for hScale.
		//
		//	720*8192/7372=800.08681
		//--------------------
		dTemp = Length;
		dTemp <<= 13;	/* *0x2000L */
		dTemp /= PANEL_H;
		ScalerSetHScaleReg(0x0400, dTemp);
		ScalerSetLineBufferSize(Length);
	}
	else {
		//-----------------------								
		//DOWN SCALE
		//for example
		//	input:1920, output:800
		//
		//	1920*1024/800 = 2457.6
		//	So, I will use 2457 for hScale.
		//
		//	1920*1024/2457=800.19536
		//--------------------------
		dTemp = Length;
		dTemp <<= 10;	/* *0x400L*/
		dTemp /= PANEL_H;

		ScalerSetHScaleReg((WORD)dTemp, 0x2000);
		ScalerSetLineBufferSize(PANEL_H);
	}
}


/**
* set Horizontal Scaler value
*
* @see ScalerSetHScale_FULL
* @see ScalerSetHScale_Panorama
*/
void ScalerSetHScale(WORD Length)	
{
//	VideoAspect = GetAspectModeEE();

	//dPrintf("\n\rScalerSetHScale(%d)",Length);
	//dPrintf("\n\rVideoAspect:%bd",VideoAspect);

//#if 0
//	if((InputMain==INPUT_CVBS || InputMain==INPUT_SVIDEO)
//	&& VideoAspect == VIDEO_ASPECT_NORMAL) {
//
//		ScalerWriteLineBufferDelay(SCALER_HDELAY2_BASE);	//BK110916 test. Normal needs it
//		ScalerPanoramaOnOff(OFF);
//
//
//		//only at CVBS
//		ScalerSetHScaleReg(0x400, 0x2000);	//clear down,up scale
//		//adjust buffer output delay
//		ScalerWriteLineBufferDelay((PANEL_H - Length) / 2 +SCALER_HDELAY2_BASE);
//
//		ScalerSetLineBufferSize(Length); //BK120111
//	}
//	else if(VideoAspect == VIDEO_ASPECT_PANO)
//		ScalerSetHScale_Panorama(Length);
//	else 
//		//	 VideoAspect == VIDEO_ASPECT_FULL
//		//of VideoAspect == VIDEO_ASPECT_ZOOM
//#endif
		ScalerSetHScale_FULL(Length);
}
//YSCALE
//-----------------------------------------------------------------------------
/**
* Up / down scaling ratio control in Y-direction. 
*
* The scaling ratio is defined as 2000h / YSCALE.
* A 16-bit register. 
*
*	register
*	R206[7:0]R205[7:0]	YSCALE	
*/
void ScalerWriteVScaleReg(WORD value)
{
	WriteTW88(REG206, (BYTE)(value>>8));
	WriteTW88(REG205, (BYTE)value);
}
//-----------------------------------------------------------------------------
/**
* read Vertical Scale register value
*
* @see ScalerWriteVScaleReg
*/
WORD ScalerReadVScaleReg(void)
{
	WORD VScale;
	Read2TW88(REG206,REG205, VScale);
	return VScale;
}

//-----------------------------------------------------------------------------
/**
* set Vertical Scale
*
* output_length = input_length * 0x2000 / YSCALE_REG
* YSCALE_REG = input_length * 0x2000 / output_length
* YSCALE_REG = input_length * 0x2000 / PANEL_V
* YSCALE_REG = (input_length * 0x2000 + r) / PANEL_V. r = PANEL_V/2
*/
void ScalerSetVScale(WORD Length)
{
	//----------------
	//output = input * 8192 / PANEL_V
	//----------------
	DWORD dTemp;
#if (PANEL_V==480)
	WORD wDiv;
#endif

#if (PANEL_V==480)
	//if we need scale down, I will use 479.50 not a 480.
	if(Length > PANEL_V) {
		dTemp = Length;
		dTemp *= 8192L;
		dTemp <<=1;
		wDiv = PANEL_V;
		wDiv <<=1;
		wDiv -= 1;
		dTemp /= wDiv;
	}
	else {
		dTemp = Length;
		dTemp *= 8192L;
		dTemp /= PANEL_V;
	}
#else
	dTemp = Length;
	dTemp <<= 13; //*= 8192L;
	dTemp /= PANEL_V;

#endif
	ScalerWriteVScaleReg((WORD)dTemp);
}
//-----------------------------------------------------------------------------
/**
* set Panorama mode
*
*	register
*	R201[6]	Enable Panorama/waterglass display
*/
void ScalerPanoramaOnOff(BYTE fOn)
{
	if(fOn)	WriteTW88(REG201, ReadTW88(REG201) | 0x40);
	else    WriteTW88(REG201, ReadTW88(REG201) & ~0x40);
}


/**
* Find ScalerTable
* @param InputMain
* @param hActive
* @param vActive
* @param vFreq
* @param vStart
*	if vStart is 0, ignore vPol
* @param vPol
*	if vStart is 0, ignore vPol
* @return 
*	fail:NULL:fail
*	success: table location
*/	 
struct SCALER_TIME_TABLE_s *FindScalerTable(BYTE _InputMain,WORD hActive,WORD vActive,BYTE vFreq, BYTE vStart, BYTE vPol)
{
	struct SCALER_TIME_TABLE_s *p;	//result
	BYTE fScale;
	BYTE vDE;

	p = SCALER_DTV_time; 

	Printf("\nFindScalerTable(%bd,%dx%d@%bd vStart:%bd,vPol:%bd)", _InputMain, hActive,vActive,vFreq, vStart,vPol);

	//horizontal values are not accurate.
	// FW will use +-2 value.
	//vertical values are accurate.
	// But, sometimes, external device send an incorrect video source.
	// FW will check +1 value on vFreq search.
	while(p->hActive) {
		if(p->vActive == vActive) {
			if(p->hActive  >= (hActive-2)  
			&& p->hActive <= (hActive+2)) {	
				if((p->vFreq == vFreq)
				|| ((p->vFreq+p->vClone) == vFreq)) {
					if(vStart==0) //if vStart is 0, ignore
						return p;

					vDE = p->vBPorch;
					if(vPol) vDE += p->vSyncWidth;
					if(vDE == vStart)
						return p;
				}
			}
		}		  
		p++;
	}
	Puts(" Fail");
	return NULL;	
}
/**
  *****************************************************************************
  * END OF FILE                                                               *
  *****************************************************************************
  */
