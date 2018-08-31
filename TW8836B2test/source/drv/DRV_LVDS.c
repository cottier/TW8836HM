
#include "HWconf.h"

HAL_StatusTypeDef	LVDSRinit( void)
{
	WriteTW88(REG648, 0x03);
	WriteTW88(REG649, 0x00);
	WriteTW88(REG64A, 0x00);
	WriteTW88(REG64B, 0x34);
	WriteTW88(REG64C, 0x40);
	WriteTW88(REG64D, 0x17);
	WriteTW88(REG64E, 0x00);
	return HAL_OK;
}
HAL_StatusTypeDef LVDSTinit( void)
{
	WriteTW88(REG2E4,0x21); //bit[6:4]=2 bit[2:0] = 1;6:6:6 output
	
	WriteTW88(REG640,0x04);
	WriteTW88(REG641,0x00);
	WriteTW88(REG642,0x00);
	WriteTW88(REG643,0xC0);
	WriteTW88(REG644,0x00);
	return HAL_OK;
}

/**
* set DTV Field Detection Register.
* looking for ODD field.
* param
*	register
*	R054[7:4]	End Location
*	R054[3:0]	Start Location
* example:
*	DtvSetFieldDetectionRegion(ON, (end<<4) | start)
*
* 	480i 		384 < 492 < 512		vs_lead
*	1080i@50 	960 < 1320 < 1408 	vs_lead
*	1080i@60	960 < 1100 < 1408 	vs_lead
*
* To verify, 
*	select ODD; REG508[3:2].
*	repeat below two steps
*		start measure.
*		check vPeriod(REG522:REG523) and VS_Position(REG52C:REG52D);
*			vPeriod will have big value, and VS_Position will have (vTotal/2). 	
*/
void DtvSetFieldDetectionRegion(BYTE fOn, BYTE r054)
{
	if(fOn) {
		WriteTW88(REG050, ReadTW88(REG050) | 0xA0 );	// set Det field by WIN. use vs_lead.
		WriteTW88(REG054, r054 );						// set window
	}
	else {
		WriteTW88(REG050, ReadTW88(REG050) & ~0x80 );	// use VSync/HSync Pulse
	}
}
/**
* set DTV Sync Polarity
* @param	hPol	1:ActiveLow
* @param	vPol	1:ActiveLow
*
*	register
*	R050[2]	hPol	1:AvtiveLow(Invert)
*	R050[1]	vPol	1:ActiveLow(Invert)
*
*	This register does not work on Measure. 
*   Only works with scaler.
*
*/
void DtvSetSyncPolarity(BYTE hPol, BYTE vPol)
{
	BYTE value;

	value = ReadTW88(REG050) & ~0x06;
	if(hPol)	value |= 0x04;	//H Active Low		
	if(vPol)	value |= 0x02;	//V Active Low		
	WriteTW88(REG050, value);	
}

struct DTV_FIELD_WIN_s {
	WORD start;
	WORD end;
};

static void	HDMISetOutput(WORD HActive, WORD VActive, BYTE	vDE )
{
	BYTE	hDE;
	WORD VScale;
	DWORD dTemp;

	ScalerSetVScale(VActive);
	ScalerSetHScale(HActive);

	//=============HDE=====================
	hDE = ScalerCalcHDE();
#ifdef DEBUG_DTV
	dPrintf("\n\r\tH-DE start = %bd", hDE);
#endif


	//=============VDE=====================

	VScale = ScalerReadVScaleReg();

//#ifdef DEBUG_DTV
//	dPrintf("\n\r\tV-DE start = %bd", vDE);
//#endif
	//pal need to add 0.5. The real value is 23.5.
	dTemp = vDE;
	dTemp <<= 1;
	if(VActive==288) //is pal
		dTemp+=1;
	dTemp *= 8192L;
	dTemp /= VScale;
	dTemp >>= 1;
	vDE = dTemp;

//#ifdef DEBUG_DTV
//	dPrintf("=> %bd", vDE);
//#endif

	//---------------------UPDATE-----------------------------

	ScalerWriteHDEReg(hDE);
	ScalerWriteVDEReg(vDE);

	//BKTODO: aspect will overwrite it. let me solve later.
	if(HActive <= PANEL_H)
		ScalerSetLineBufferSize(HActive+1);
	else
		ScalerSetLineBufferSize(PANEL_H+1);
}



code struct DTV_FIELD_WIN_s DtvFieldWin[] = {
/*0*/ {32,64},		/*1*/ {64,128},		/*2*/ {128,256},	/*3*/ {192,384},
/*4*/ {256,512},	/*5*/ {320,640},	/*6*/ {384,768},	/*7*/ {448,896},
/*8*/ {512,1024},	/*9*/ {576,1152},	/*A*/ {640,1280},	/*B*/ {704,1408},
/*C*/ {768,1536},	/*D*/ {832,1664},	/*E*/ {896,1792},	/*F*/ {960,1920}
};
BYTE DtvFindFieldWindow(WORD base)
{
	BYTE i;
	BYTE start,end;
	struct DTV_FIELD_WIN_s *p;

	//search Start
	p = &DtvFieldWin[15];
	i=15;
	while(i) {
		if(p->start < base)
			break;
		p--;
		i--;
	}
	start = i;
	//search End
	p = DtvFieldWin;
	for(i=0; i<16; i++) {
		if(p->end > base)
			break;
		p++;
	}
	end = i;

	Printf("\nDtvFindFieldWindow(%d) %d~%d", base, DtvFieldWin[start].start, DtvFieldWin[end].end); 
	return ((end << 4) | start);
}

void InputSetCrop( WORD x, WORD y, WORD w, WORD h )
{
	WriteTW88(REG040_INPUT_CTRL_I, (ReadTW88(REG040_INPUT_CTRL_I) & 0x3F) | ((x & 0x300)>>2) );
	WriteTW88(REG045, (BYTE)x);
	WriteTW88(REG043, (BYTE)y);

	WriteTW88(REG042, ((h&0xF00) >> 4)|(w >>8) );
	WriteTW88(REG044, (BYTE)h);
	WriteTW88(REG046, (BYTE)w);
	//dPrintf("\n\rInput Crop Window: x = %d, y = %d, w = %d, h = %d", x, y, w, h );
}


BYTE CheckAndSetDtvSignal(void)
{
	BYTE ret;
	BYTE i;
	BYTE vFreq;
	WORD hActive,vActive;
	WORD hStart,vStart;

	WORD 
	hPeriod,vPeriod;
	WORD hFPorch,vFPorch;
	WORD hSync,vSync;
	WORD hBPorch,vBPorch;
		
	struct DIGIT_VIDEO_TIME_TABLE_s *pCEA861;
	struct SCALER_TIME_TABLE_s *pScaler;

	BYTE hPol,vPol;
	WORD wTemp0,wTemp1;
	DWORD dTemp,pclk;

	/*If someone changed meas, recover it*/
	MeasSetWindow( 0, 0, 0xfff, 0xfff );//set dummy window. 1600x600
	WriteTW88(REG50A, ReadTW88(REG50A) | 0x01);	//Enable DE measure
	WriteTW88(REG508, ReadTW88(REG508) & ~0x10);

	/* check video signal */
	for(i=0; i < 5; i++) {
		Meas_StartMeasure();
		ret=Meas_IsMeasureDone(50);
		if(ret==0)
			break;
		delay1ms(10);
		Puts("."); //I am alive...
	}
	if(ret) {
		/*if measure fail, it measn no signal...*/
		Puts(" meas=> NoSignal");
		return ERR_FAIL;
	}
	/* wait until DTV has a stable image. */
	wTemp0 = MeasGetHActive( &hStart );
	wTemp1 = MeasGetVActive( &vStart );
	for(i=0; i < 20; i++) {
		delay1ms(10);
		Meas_StartMeasure();
		ret=Meas_IsMeasureDone(50);
		hActive = MeasGetHActive( &hStart );
		vActive = MeasGetVActive( &vStart );
		if(wTemp0==hActive && wTemp1==vActive)
			break;
		wTemp0 = hActive;
		wTemp1 = vActive;
		Puts("*"); //I am alive...
	}
	/* read measured registers */
	hSync = MeasGetHSyncRiseToFallWidth();
	vSync = MeasGetVSyncRiseToFallWidth();
	hPeriod = MeasGetVsyncRisePos();
	vPeriod = MeasGetVPeriod();  
	vFreq = MeasGetVFreq();
	Puts("DTV\n");
	//---------------------
	//DTV measure adjust. hStart+4, vStart-1.
	//If pol is active high, hStart is Sync+BPorch
	//If pol ia active low,  hStart is BPorch
	//---------------------
	hStart += 4;
	vStart -= 1;

	/*Find CEA861 or VESA table*/
	pCEA861 = Find_CEA861_VideoTable(hActive,vActive,vFreq);
	Printf("no!!!!");
	if(pCEA861) {
		//PrintCEAVideoTimeTable(pCEA861);
		/* If meas has a differnet value, use CEA861 value.
	     for example
			if(hActive==1151)	hActive = 1152;
			if(hActive==1279)	hActive = 1280;
			if(hActive==1439)	hActive = 1440;
			if(hActive==1599)	hActive = 1600;
			if(hActive==1919)	hActive = 1920;
		*/
		hActive = pCEA861->hActive;
	}

	/*check Sync polarity.*/
	/*Firmware likes the active high polarity.*/
	if ( hSync > hActive )	hPol = 0;	//active low.  Negative
	else					hPol = 1;	//active high. Positive
	if ( vSync > vActive )	vPol = 0;	//active low.  Negative
	else					vPol = 1;	//active high. Positive

	/*set DTV hPol and vPol polarity.*/
	//These start values are BPorch or Sync+BPorch that depend on the polarity.
	//so FW do not use Dtv polarity.
	DtvSetSyncPolarity(0,0);

	/*set field window for Interlaced*/
	if(pCEA861) {
		if(pCEA861->pol & INTERLACED) {
			i = DtvFindFieldWindow(pCEA861->hTotal / 2);
			DtvSetFieldDetectionRegion(ON, i);
			Meas_StartMeasure();
			Meas_IsMeasureDone(50);
			Puts("DTV");
		}
	}

	/*Find vFPorch,vSync,vBPorch.*/
	if(vPol==0) {
		vSync = vPeriod - MeasGetVSyncRiseToFallWidth();
		vBPorch = vStart;
		vFPorch = vPeriod - vSync - vBPorch - vActive;
	}
	else {
		vSync = MeasGetVSyncRiseToFallWidth();
		vBPorch = vStart - vSync;
		vFPorch = vPeriod - vSync - vBPorch - vActive;
	}
	/*Find hFPorch,hSync,hBPorch and hPeriod.*/
	if(hPol==0) {
		hBPorch = hStart;
		hFPorch = MeasGetHSyncRiseToFallWidth() - MeasGetHSyncRiseToHActiveEnd();
		WriteTW88(REG508, ReadTW88(REG508) | 0x10);
		Meas_StartMeasure();
		Meas_IsMeasureDone(50);
		hPeriod = MeasGetHSyncRiseToFallWidth();
		hSync = hPeriod - hFPorch - hBPorch - hActive;
		
		//BKTODO141009 remove it
		WriteTW88(REG508, ReadTW88(REG508) & ~0x10);
		Meas_StartMeasure();
		Meas_IsMeasureDone(50);
	}
	else {
		hSync = MeasGetHSyncRiseToFallWidth();
		hBPorch = hStart - hSync;
		hPeriod = MeasGetVsyncRisePos();    
		hFPorch = hPeriod - hBPorch - hSync - hActive;
	}
	/*Find an input pixel clock.*/
	//	InputPixelClock = hPeriod * pclk / MeasGetHPeriodReg();
	wTemp0 = MeasGetHPeriodReg();
	dTemp = Sspll2GetFreq();
	pclk   = PclkGetFreq(dTemp);
	pclk /= 1000;
	dTemp = hPeriod;
	dTemp *= pclk;
	dTemp /= wTemp0;

	//print out the result
	Printf("\n%dx%d@%bd", hActive,vActive,vFreq);
	Printf(" hStart:%d vStart:%d", hStart, vStart);

	Printf(" %s", hPol ? "HP" : "HN");
	Printf("_%s", vPol ? "VP" : "VN");

	Printf(" %d,%d,%d,%d",
		hPeriod, hFPorch, hSync, hBPorch);
	Printf(" %d,%d,%d,%d",
		vPeriod, vFPorch, vSync, vBPorch);
	Printf("\n pclk:%ld SSPLL:%ld", dTemp,pclk);

	//if (( pclk / InputPixelClock) > 2), use 2 for hStart crop value. 
	wTemp1= wTemp0 / hPeriod;
	if(wTemp1 > 2) 	i = 2;
	else 			i = 1;
	Printf(" hOffset:%bd",i);	

	//If input has a pixel repeat, some HDMI Rx chip send a double horizontal values.
	if(hActive==1440) {
		if(vActive==240 || vActive==288) {
			hActive >>= 1;
			hStart >>=1;
			Printf("\n=>hActive:%d =>hCropStart:%d",hActive, hStart);
		}
	}

	//-------------------------------------------
	// input crop
	InputSetCrop(hStart-i, vStart-vPol, hActive+2, vActive+2);
	HDMISetOutput( hActive, vActive,  vStart );

	//Find the scaler table and then overwrite.
	//because,...
	pScaler = FindScalerTable(8, hActive,vActive,vFreq, vStart,vPol);
	if(pScaler != NULL);
	//	OverWriteScalerWithTable(pScaler, hPol, vPol);

	return ERR_SUCCESS;
}




//=============================================================================
// Change to LVDSRx
//=============================================================================



/**
* TW8836 EVB1.0 uses a HDMI output for LVDS Tx chip input.
*
* video path
* HDMI=>LVDS Tx chip =>TW8836 LVDSRx => Scaler => Panel
*/
BYTE CheckAndSetLVDSRx(void)
{
//	dPuts("\n\rCheckAndSetLVDS START");
	return CheckAndSetDtvSignal();
}

BYTE ChangeLVDSRx(void)
{
	BYTE ret;

	//
	// Check and Set Input crop, scale rate, scaler output time.
	//
	ret = CheckAndSetLVDSRx();		
 	if(ret==0) {
		//success
		return 0;
	}
	//------------------
	// NO SIGNAL
	// Prepare NoSignal Task...

	//VInput_gotoFreerun(0);
	return(2);
}