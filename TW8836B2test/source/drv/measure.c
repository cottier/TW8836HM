/**
 * @file
 * measure.c 
 * @author Brian Kang
 * @version 1.0
 * @section LICENSE
 *	Copyright (C) 2011~2012 Intersil Corporation
 * @section DESCRIPTION
 *	video measure module 
 *
 * measure step
 * ============
 *
 *	Step1: Assign a dummy large window.
 *	Step2: Assign a option parameters.
 *	Step3: Start measure. If it successes, do next step.
 *	Step4: Find out the PC mode, and decide the index.
 *	Step5: Change a filter bandwidth as 0.
 *	Step6: Assign LLPLL value that is related with Htotal pixel, and then wait until it has a stable state.
 *	Step7: Change a filter bandwidth as 7.
 *	Step8: Adjust the polarity.
 *	Step9: Find a phase value, or use a EEPROM value.
 *	       After step9, do not call the MeasStartMeaasure again. FW will use a measured value.
 *	Step10: Read the measured value and adjust the input crop, the scale rate, and the scaler output.
 *	Step11: Check and find the PC mode again. If PC mode is different, retry all steps.
*/
/*
Disclaimer: THIS INFORMATION IS PROVIDED 'AS-IS' FOR EVALUATION PURPOSES ONLY.  
INTERSIL CORPORATION AND ITS SUBSIDIARIES ('INTERSIL') DISCLAIM ALL WARRANTIES, 
INCLUDING WITHOUT LIMITATION FITNESS FOR A PARTICULAR PURPOSE AND MERCHANTABILITY.  
Intersil provides evaluation platforms to help our customers to develop products. 
However, factors beyond Intersil's control could significantly affect Intersil 
product performance. 
It remains the customers' responsibility to verify the actual system performance.
*/
#include "HWconf.h"
#include "measure.h"

//================== Input Measurement ====================================
//===============================================
// Measure 1 START
//===============================================
WORD RGB_hStart; 
BYTE RGB_vDE;


//=============================================================================
// Measurement Page:5
//=============================================================================

//-----------------------------------------------------------------------------
/**
* set Measure window
*
* @param x: Horizontal start
* @param y: Vertical Start
* @param w: Horizontal Width
* @param h: Vertical Height
*
*	register
*	R500[2:0]R501[7:0]	Measurement Window Horizontal Start [10:0]
*	R502[3:0]R503[7:0]	Measurement Window Horizontal length [11:0]
*	R504[2:0]R505[7:0]	Measurement Window Vertical Start [10:0]
*	R506[2:0]R507[7:0]	Measurement Window Vertical Length [10:0]
*/
void MeasSetWindow( WORD x, WORD y, WORD w, WORD h )   //
{	
	//TW8836 needs minimum 2 Horizontal start position
	if(x < 2)
		x = 2;

	Write2TW88(REG500_MEAS_HSTART_MSB, REG501_MEAS_HSTART_LSB, x);		
	Write2TW88(REG502_MEAS_HLEN_MSB,   REG503_MEAS_HLEN_LSB,   w);

	Write2TW88(REG505_MEAS_VSTART_MSB, REG505_MEAS_VSTART_LSB, y);		
	Write2TW88(REG506_MEAS_VLEN_MSB,   REG507_MEAS_VLEN_LSB,   h);
}

//-----------------------------------------------------------------------------
// Measure Options
//-----------------------------------------------------------------------------
//R508[]		Measurement Input Selection, Measurement Start Register
//R509[]		Measurement Option, Input Change Detection Register
//R50A[]		Measurement Option Register
//R50B[]		Measurement Option Register - Threshold value for input active region detection


//-----------------------------------------------------------------------------
/**
* set field
*
*	register
*	R508[3:2]	Field Select for input Measure.
*				00b=Odd field only. 01b:Even field only, 10b or 11b=both field
*/
//void MeasSetField(BYTE field)
//{
//	WriteTW88(REG508, ReadTW88(REG508) & 0xF3 | (field << 2) );	// Note:DO not turn on the start
//}

#ifdef UNCALLED_CODE
//-----------------------------------------------------------------------------
//register
//	R508[1]	Lock the data while reading out
void MeasLockData(BYTE fLock)
{
	WriteTW88(REG508, ReadTW88(REG508) & 0xFD | (fLock << 1) );	// Note:DO not turn on the start
}
#endif


//-----------------------------------------------------------------------------
/**
* Start Measurement. 
*
* It will update the measure result register.
*	register
*	R508[0]
* use max 500ms
* @return
*	0:success
*	1:failed
*/
#if 0
BYTE MeasStartMeasure(void)
{
	volatile BYTE val;
	BYTE i;

	WriteTW88(REG508, ReadTW88(REG508) | 1 );	// Measure start

	// WaitMeasurementDataReady
	for(i=0; i<50; i++) {			// make more delay
		delay1ms(10);	   			// 10ms delay 
		val = ReadTW88(REG508);
		if((val & 0x01) == 0)
			return ERR_SUCCESS;
	}
	//dPuts("\n\rStartMeasure failed");
	return ERR_FAIL;		// means timeout
}
#endif

#if 1
void Meas_StartMeasure(void)
{
	WriteTW88(REG508, ReadTW88(REG508) | 1 );	// Measure start    
}
BYTE Meas_IsMeasureDone(BYTE cnt)
{
	BYTE i;
	volatile BYTE val;

	for(i=0; i<cnt; i++) {			// make more delay
		val = ReadTW88(REG508);
		if((val & 0x01) == 0)
			return ERR_SUCCESS;
		delay1ms(10);	   			// 10ms delay 
	}
	return ERR_FAIL;		// means timeout
}
#endif


//-----------------------------------------------------------------------------
//desc
//	The default uses 27MHz for HPeriod Register (R524[7:0]R525[7:0]).
//	If User enable R509[7], the HPeriod register value will be based on the input clock.
//@param
//	ON:Use a input clock for HPeriod register
//return 
//	OLD value, ON or OFF
//register
//	R509[7]	: Hidden. Use input clock when it read HPeriod. 
#if 0
BYTE MeasSetInputClkForHPeriodReg(BYTE fOn)
{
	BYTE val;

	val = ReadTW88(REG509);
	if(fOn)	WriteTW88(REG509, val | 0x80);
	else 	WriteTW88(REG509, val & ~0x80);

	return (val & 0x80 ? ON:OFF);
}
#endif
//-----------------------------------------------------------------------------
/**
* get 27MHz flag
*/
#if 0
BYTE MeasGetInputClkForHPeriodReg(void)		//meas_get_inputclk_for_hperiod_reg
{
	if(ReadTW88(REG509) & 0x80)	return 1;
	else						return 0;
}
#endif

#ifdef UNCALLED_SEGMENT
//-----------------------------------------------------------------------------
//desc
//register
//	R509[6:4]	Noise mask bits for each of the 3 LSB input signals
void MeasSetNoiseMask(BYTE mask)
{
	WriteTW88(REG509, (ReadTW88(REG509) & 0x8F) | (mask << 4));	
}
#endif

//-----------------------------------------------------------------------------
/**
* set Error Tolerance value
*
*	register
*	R509[3:1]
* @param value - 100b = 32 counts
*/
void MeasSetErrTolerance(BYTE value)
{
	WriteTW88(REG509, (ReadTW88(REG509) & 0xF1) | (value << 1));	//up to 32 counts
}

//-----------------------------------------------------------------------------
/**
* turn on the En.Changed Detection.
*
* If FW call MeasStartMeasure when R509[0]==0, 
* Chip captures the measure value and uses it as a reference value for SYNC interrupt.
* SYNC intr is related with R002[2] and R004[2:1]
*
*	step
*	MeasEnableChangedDetection(OFF);
*	MeasStartMeasure() with proper setting. It will save the reference value.
*	MeasEnableChangedDetection(ON);
*	wait INTR
*
*	register
*	R509[0]	
*/
void MeasEnableChangedDetection(BYTE on)
{
	if(on)	WriteTW88(REG509, ReadTW88(REG509) | 0x01);
	else    WriteTW88(REG509, ReadTW88(REG509) & ~0x01);
}


//-----------------------------------------------------------------------------
/**
* enable DE Measure
*
*	register
*	R50A[7]		Enable edge adjustment
*	R50A[3]		Enable Luminance measurement
*	R50A[2:1]	Noise filter selection for luminance measurement
*	R50A[0]		De Measurement Enable
*/
void MeasEnableDeMeasure(BYTE fOn)
{
	if(fOn)	WriteTW88(REG50A, ReadTW88(REG50A) | 0x01 );
	else	WriteTW88(REG50A, ReadTW88(REG50A) & 0xFE );
}

//-----------------------------------------------------------------------------
/**
* set measure threshold as
*
*	register
*	R50B[7:0]
*/
//void MeasSetThreshold(BYTE thold)
//{
//	WriteTW88(REG50B, thold ); 
//}


//-----------------------------------------------------------------------------
// Measure Phase
//-----------------------------------------------------------------------------

//R510 R211 R512 R513	Phase_R Registers
//R514 R215 R516 R517	Phase_G Registers
//R518 R219 R51A R51B	Phase_B Registers

#ifdef UNCALLED_SEGMENT
//-----------------------------------------------------------------------------
//desc
//	note:we use high 24bit.
//register
//	R510[7:0]	Phase_R Byte 3
//	R511[7:0]	Phase_R Byte 2
//	R512[7:0]	Phase_R Byte 1
//	R513[7:0]	Phase_R Byte 0
DWORD MeasReadPhaseR(void)
{
	DWORD PhaseR;
	//Read4TW88(REG510,REG511,REG512,REG513, PhaseR);
	Read3TW88(REG510,REG511,REG512, PhaseR);
	return PhaseR;
}
//-----------------------------------------------------------------------------
//desc
//	note:we use high 24bit.
//register
//	R514[7:0]	Phase_G Byte 3
//	R515[7:0]	Phase_G Byte 2
//	R516[7:0]	Phase_G Byte 1
//	R517[7:0]	Phase_G Byte 0
DWORD MeasReadPhaseG(void)
{
	DWORD PhaseG;
	//Read4TW88(REG514,REG515,REG516,REG517, PhaseG);
	Read3TW88(REG514,REG515,REG516, PhaseG);
	return PhaseG;
}
//-----------------------------------------------------------------------------
//desc
//	note:we use high 24bit.
//register
//	R518[7:0]	Phase_B Byte 3
//	R519[7:0]	Phase_B Byte 2
//	R51A[7:0]	Phase_B Byte 1
//	R51B[7:0]	Phase_B Byte 0
DWORD MeasReadPhaseB(void)
{
	DWORD PhaseB;
	//Read4TW88(REG518,REG519,REG51A,REG51B, PhaseB);
	Read3TW88(REG518,REG519,REG51A, PhaseB);
	return PhaseB;
}
#endif //..UNCALLED_SEGMENT

#ifdef SUPPORT_PC
//-----------------------------------------------------------------------------
/**
* Read RGB Phase value
*
* Phase R/G/B have a seperate registers.
* But, FW needs only a total value.
*/
DWORD MeasReadPhaseRGB(void)
{
	DWORD PhaseRGB;
	DWORD Phase;
	Read3TW88(REG510,REG511,REG512, Phase);	PhaseRGB = Phase;	//PhaseR
	Read3TW88(REG514,REG515,REG516, Phase);	PhaseRGB+= Phase;	//PhaseG
	Read3TW88(REG518,REG519,REG51A, Phase);	PhaseRGB+= Phase;	//PhaseB.
	return PhaseRGB;
}
#endif


//-----------------------------------------------------------------------------
//R51C	minimum R
//R51D	minimum G
//R51E	minimum B
//R51F	maximum R
//R520	maximum G
//R521	maximum B
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//internal
/**
* read the measured VPeriod value.
*
*	register
*	R522[7:0]R523[7:0]	Vertical Period Register
*/
WORD MeasGetVPeriod( void )
{
#if 0 //for test
	WORD	period;
	BYTE i;

	Printf("\n\rtest MeasGetVPeriod:");
	for(i=0; i < 20; i++) {
		Meas_StartMeasure();
		Meas_IsMeasureDone(50);
		Read2TW88(REG522,REG523, period);
		Printf(" %d",period); 
		delay1ms(10);
	}

	return( period );
#else
	WORD	period;

	Read2TW88(REG522,REG523, period);
	return( period );
#endif
}

//-----------------------------------------------------------------------------
/**
* read the measured HPeriod value.
*
* If R509[7]==1, use 27MHz.
* If R509[7]==0, use input clock. default
*
*	register
*	R524[7:0]R525[7:0]	Horizontal Period Registers
*/
WORD MeasGetHPeriodReg( void )
{
	WORD	period;

	Read2TW88(REG524,REG525, period);
	return( period );
}

//-----------------------------------------------------------------------------
/**
* get Horizontal Total.
*
* use 27MHz to get a HPeriod value if R509[7]==0.	 
* please, rename it as meas_get_havtive...htotal is confuse with hperiod.
*/
WORD MeasGetHTotal(BYTE hPol)
{
	DWORD dTemp;
	WORD sspll;
	WORD hTotal;

	if(hPol) {
		hTotal = MeasGetVsyncRisePos();
	}
	else {
		hTotal = MeasGetHPeriodReg();
		//if(MeasGetInputClkForHPeriodReg()==0) 
		{
			dTemp = Sspll2GetFreq();
			Printf("\n\rhPeriod:%d dTemp:%ld",hTotal, dTemp);
			sspll = dTemp / 1000000;
			dTemp = hTotal;
			hTotal = dTemp * 27 / sspll;
			Printf("\n\rsspll:%d",sspll);
			hTotal += 1;	//adjust...BUG 
		}
		//else {
		//	//only work at 27MHz source. for example:480P,480i
		//	hTotal += 1;	//adjust...BUG 
		//}
	}
	return hTotal;
}



#ifdef UNCALLED_SEGMENT
//-----------------------------------------------------------------------------
//desc
//
//return
//	KHz
BYTE MeasGetHFreq(void)
{
	WORD freq;
	DWORD PCLK;
	WORD period;

	period = MeasGetHPeriodReg();
	if(ReadTW88(REG509) & 0x80) {		//If R509[7]==1, use input clock.
		PCLK = Sspll2GetFreq();
	}
	else {
		PCLK = 27000000L;
	}

	freq = (WORD)(PCLK / period);
	//dPrintf("\n\rHFreq:%d.%d PCLK:%ld period:%d",freq/1000, freq%1000,PCLK, period);
	return (BYTE)(freq/1000); 
}
#endif

//-----------------------------------------------------------------------------
/**
* get Hsync Rise to Fall Registers value
*							 
*	register
*	R526 R527	Hsync Rise to Fall Registers
*/
WORD MeasGetHSyncRiseToFallWidth(void)
{
	WORD wTemp;
	Read2TW88(REG526,REG527, wTemp);
	return wTemp;
}

//-----------------------------------------------------------------------------
/**
* get "Hsync Rise to Horizontal Active End" value
*
*	register
*	R528 R529 Hsync Rise to Horizontal Active End
*/
WORD MeasGetHSyncRiseToHActiveEnd(void)
{
	WORD wTemp;
	Read2TW88(REG528,REG529, wTemp);
	return wTemp;
}


//-----------------------------------------------------------------------------
/**
* get "Vsync High Width Registers" value
*
*	register
*	R52A R52B		Vsync High Width Registers
*/
WORD MeasGetVSyncRiseToFallWidth(void)
{
	WORD wTemp;
	Read2TW88(REG52A,REG52B, wTemp);
	return wTemp;
}

//-----------------------------------------------------------------------------
/**
* get "Vsync Rise Position Registers" value
*
*	register
*	R52C R52D 	Vsync Rise Position Registers
*/
WORD MeasGetVsyncRisePos(void)
{
	WORD wTemp;
	Read2TW88(REG52C,REG52D, wTemp);
	return wTemp;	
}


//-----------------------------------------------------------------------------
/**
* Get Horizontal Active value
*
* extern
*	InputMain
*
*	register
*	R52E R52F 	Horizontal Active Starting Pixel Position I Registers
*	R530 R531	Horizontal Active Starting Pixel Position II Registers
*	R532 R533	Horizontal Active Ending Pixel Position I Registers
*	R534 R535	Horizontal Active Ending Pixel Position II Register
*/
WORD MeasGetHActive( WORD *start )
{
	WORD	period, end;

	Read2TW88(REG52E, REG52F, period);
	Read2TW88(REG530, REG531, *start);

	if ( *start < period )	//select big
		*start = period;

	Read2TW88(REG532, REG533, period);
	Read2TW88(REG534, REG535, end);
	if ( end > period )		//select small
		end = period;

	period = end - *start + 1;

	return( period );
}
#if 0
//-----------------------------------------------------------------------------
BYTE MesaGetHPol(void)
{
	WORD SyncRiseToFall;
	WORD hStart;
	WORD hActive;
	BYTE hPol;

	SyncRiseToFall = MeasGetHSyncRiseToFallWidth();
	hActive = MeasGetHActive( &hStart );

	//get HPol
	if(SyncRiseToFall > hActive)	hPol = 0;	//high at active
	else							hPol = 1;
	return hPol;
}
#endif

//return hTotal value
#if defined(SUPPORT_DVI)
//-----------------------------------------------------------------------------
/** 
* get divider value for DVI
*/
WORD MeasGetDviDivider(void)
{
#if 0
	WORD	end;
	Read2TW88(REG534, REG535, end);

	return (end + (end/8));
#else
	//if FW use DE measure, we can get a correct value.
	WORD SyncRiseToFall;
	WORD hStart;
	WORD hTotal,hActive;
	BYTE hPol;

	SyncRiseToFall = MeasGetHSyncRiseToFallWidth();
	hActive = MeasGetHActive( &hStart );

	//get HPol
	if(SyncRiseToFall > hActive)	hPol = 0;	//high at active
	else							hPol = 1;

	//get HTotal
	hTotal = MeasGetHTotal(hPol);
	if(hTotal < SyncRiseToFall)	{
		//special case at 2880x240p
		if(MeasGetVsyncRisePos() < SyncRiseToFall)	
			hTotal += 1;
		hTotal <<= 1;
	}
	return hTotal;	
#endif
}
#endif

#if 0 //def SUPPORT_COMPONENT
//-----------------------------------------------------------------------------
/**
* get HAvtive2 value
*/
WORD MeasGetHActive2( void )
{
	WORD	start;

	Read2TW88(REG530, REG531, start);
	return start;
}
#endif

//-----------------------------------------------------------------------------
/**
* get Vertical Active value
*
* VActive is a VStart position.
* Get the start position and the length for the vertical active.
*
*	register
*	R536 R537		Vertical Active Starting Line I Registers
*	R538 R539		Vertical Active Starting Line II Registers
*	R53A R53B		Vertical Active Ending Line I Registers
*	R53C R53D		Vertical Active Ending Line II Registers
*/
WORD MeasGetVActive( WORD *start )
{
	WORD	period, end;

	Read2TW88(REG536,REG537, period);
	Read2TW88(REG538,REG539, *start);
	if ( *start < period )	//select big
		*start = period;

	Read2TW88(REG53A,REG53B, period);
	Read2TW88(REG53C,REG53D, end);
	if ( end < period )		//select big
		end = period;

	period = end - *start + 1;
	return( period );
}
#if 0 //defined(SUPPORT_COMPONENT)
//-----------------------------------------------------------------------------
/**
* get VActive2 value
*/
WORD MeasGetVActive2(void)
{
	WORD temp16;
	Read2TW88(REG538,REG539, temp16);
	return temp16;
}
#endif

//-----------------------------------------------------------------------------
//	R540 - Liminance Value - Minimum Register
//	R541 - Liminance Value - Maximum Register
//	R542 - Liminance Value - Average Register
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//internal
/**
* get VPeriod value on 27MHz
*
*	register
*	R543 R544 R545 Vertical Period in 27 MHz Registers
*/
DWORD	MeasGetVPeriod27( void )
{
	DWORD	period;
	Read3TW88(REG543, REG544, REG545, period);	//Vertical Period in 27 MHz Registers
	return( period );
}

//-----------------------------------------------------------------------------
/**
* get Vertical Frequency
*/
BYTE MeasGetVFreq(void)
{
	DWORD	vperiod;
	BYTE	vfreq;

#if 1  //BK131022
	DWORD dTemp;
	vperiod = MeasGetVPeriod27();
	//add 0.05 to convert 59.99 to 60.
	//                    59.93 to 59.  
	dTemp = vperiod / 50;   //434847
	dTemp += 27000000L;
	dTemp /= vperiod;
	vfreq = (BYTE)dTemp;
#else
	vperiod = MeasGetVPeriod27();
	vfreq = 27000000L / vperiod;
#endif
	//dPrintf("\n\rVFreq:%bd",vfreq);
	return vfreq;
}

#if defined(SUPPORT_PC)
//-----------------------------------------------------------------------------
/**
* rounddown Vertical Frequcncy value
*
* PC wants to detect, 56Hz, 60Hz, ..
*/
BYTE MeasRoundDownVFreqValue(BYTE vfreq)
{
	if      ( vfreq < 58 ) vfreq = 56;	//<58
	else if ( vfreq < 61 ) vfreq = 60;	//58~60
	else if ( vfreq < 71 ) vfreq = 70;	//61~70
	else if ( vfreq < 73 ) vfreq = 72;	//71~72
	else if ( vfreq < 77 ) vfreq = 75;	//73~76
	else if ( vfreq < 87 ) vfreq = 85;	//77~86
	//else { vfreq will not be changed. }
	return vfreq;
}
#endif

//===============================================
// Meas Leve 1
//===============================================

//===============================================
// Measure 1 START
//===============================================


#ifdef SUPPORT_PC
//-----------------------------------------------------------------------------
/**
* calculate Phase value
*
* RGBBufferPtr[] use 4*32 (128) bytes. 
* SPI_Buffer[SPI_BUFFER_SIZE] have to bigger or equal then 128 bytes.
*/
void AutoTunePhase(void)
{
	BYTE	i, j;
	DWORD	RGBmin;
	DWORD	*RGBBufferPtr;
	BYTE 	OldPhase;

	RGBBufferPtr = (DWORD *)SPI_Buffer;

	OldPhase=aRGB_GetPhase();

	for(i=0; i<32; i++) {						// full scan PHASE
		aRGB_SetPhase(i, 0);
		delay1ms(5);
	
		Meas_StartMeasure();
		if(Meas_IsMeasureDone(50)) {
			aRGB_SetPhase(OldPhase, 0);
			return;
		}
		RGBBufferPtr[i]  = MeasReadPhaseRGB();

		//dPrintf("\n\rPhase %02bd: %08ld", i, RGBBufferPtr[i]);
	}
	
	//search minimum value.
	RGBmin = RGBBufferPtr[0];
	j = 0;
	for ( i=1; i<32; i++ ) {
		if ( RGBmin > RGBBufferPtr[i] ) {
			RGBmin = RGBBufferPtr[i];
			j = i;
		}
	}
	//dPrintf("\n\rRGBmin = %ld, phaseMin = %bd", RGBmin, j);

	//compare the min value with the 180 degree value.
	j = ( j + 16 ) % 32;
	Printf(" phaseMax = %bd", j);

	RGBmin = RGBBufferPtr[j] / ( RGBBufferPtr[j] - RGBmin );
	//dPrintf("\n\rRGB diff = %ld", RGBmin);

	if ( RGBmin > 10 ) {
		Puts("\n\rdifference is lower than 10%, cannot find correct phase");
		aRGB_SetPhase(OldPhase,0);	//restore Old Phase value.
	}
	else {
		//select big.
		aRGB_SetPhase(j, 0);
	}
}
#endif

#ifdef SUPPORT_PC
//-----------------------------------------------------------------------------
/**
* calculate color value
*/
void AutoTuneColor(void)
{
	BYTE	MeasR, MeasG, MeasB;
	WORD	GainR, GainG, GainB;
	WORD	nGainR, nGainG, nGainB;

	nGainR = nGainG = nGainB = 0xf0;
	GainG = aRGB_ReadGChannelGainReg();
	GainB = aRGB_ReadBChannelGainReg();
	GainR = aRGB_ReadRChannelGainReg();
	if(nGainG != GainG
	|| nGainB != GainB
	|| nGainR != GainR) 
		aRGB_SetChannelGainReg(nGainG,nGainB,nGainR);

	do {
		GainR = nGainR;
		GainG = nGainG;
		GainB = nGainB;	
		Meas_StartMeasure();
		if(Meas_IsMeasureDone(50)) {
			return;
		}
		MeasG = ReadTW88(REG51F );
		MeasB = ReadTW88(REG520 );
		MeasR = ReadTW88(REG521 );
		nGainR = GainR + (WORD)(0xFF - MeasR);
		nGainG = GainG + (WORD)(0xFF - MeasG);
		nGainB = GainB + (WORD)(0xFF - MeasB);
		if ( nGainR > 0x1FF ) nGainR = 0x1FF;
		if ( nGainG > 0x1FF ) nGainG = 0x1FF;
		if ( nGainB > 0x1FF ) nGainB = 0x1FF;

		if(nGainG != GainG
		|| nGainB != GainB
		|| nGainR != GainR) 
			aRGB_SetChannelGainReg(nGainG,nGainB,nGainR);

		//dPrintf("\n\rMeasure: R: 0x%02bx, G: 0x%02bx, B: 0x%02bx", MeasR, MeasG, MeasB );
		//dPrintf("\n\rNew Gains: R:%d, G:%d, B:%d", nGainR, nGainG, nGainB );
	} while (( nGainR != GainR ) || ( nGainG != GainG ) || ( nGainB != GainB)) ;

	if (( nGainR == 0x1FF ) && ( nGainG == 0x1FF ) && ( nGainB == 0x1FF )) 
		ePrintf("\n\rMeasurement block has PROBLEM!!!!");
}
#endif

#ifdef SUPPORT_PC
//-----------------------------------------------------------------------------
/**
* check VPulse
*/
#if 0  //I know it needs if you assign the incorrect polarity.
       //Now, FW can assign the correct polarity, so I don't need it.
BYTE MeasCheckVPulse(void)
{
	BYTE i;
	WORD HStart, HActive, VPulse;
	BYTE VPulseErr;

	HActive = MeasGetHActive( &HStart );				//h_active_start h_active_perios
	for(i=0; i < 100; i++) {
		VPulseErr = 0;
		Meas_StartMeasure();
		if(Meas_IsMeasureDone(50)) {
			return 1;
		}
		VPulse = MeasGetVSyncRiseToFallWidth();
		if((HActive/2) < VPulse) {
			//incorrect vpulse.	  BK110830 NG
			VPulseErr = 1;
			//if(HLen == HLenOld) 
			{
				//dPrintf("\n\rVErr @%bd %d",i, VPulse);
				delay1ms(10);
			}
		}
		else
			return 0;
	}
	return 1;
}
#endif
#endif

//===============================================
// Measure 1 END
//===============================================


//-----------------------------------------------------------------------------
/**
* check the measure value again
*/
#if defined(SUPPORT_DVI)
void CheckMeasure(void)
{
	WORD SyncRiseToFall,HRiseToActEnd;
	WORD hStart,vStart;
	WORD hTotal,hSync,hBPorch,hActive,hFPorch;
	BYTE hPol;
	WORD vTotal,vSync,vBPorch,vActive,vFPorch;
	BYTE vPol;
	BYTE special;


	special = 0;
	//---------------------
	//horizontal
	//---------------------
	SyncRiseToFall = MeasGetHSyncRiseToFallWidth();
	HRiseToActEnd = MeasGetHSyncRiseToHActiveEnd();

	//get hActive
	hActive = MeasGetHActive( &hStart );

	//get HPol
	if(SyncRiseToFall > hActive)	hPol = 0;	//high at active
	else							hPol = 1;

	//get HTotal
	hTotal = MeasGetHTotal(hPol);
	if(hTotal < SyncRiseToFall)	{
		//special case at 2880x240p
		if(MeasGetVsyncRisePos() < SyncRiseToFall)	
			hTotal += 1;
		hTotal <<= 1;
		special = 1;
	}

	//get hFPorch hSync hBPorch
	if(hPol==0) {

		hFPorch = SyncRiseToFall - HRiseToActEnd -1;
		hSync = hTotal - SyncRiseToFall -1;

		if(special==1)
			hSync += 2;

		hBPorch = HRiseToActEnd - hActive;
	}
	else {
		hFPorch = hTotal - HRiseToActEnd;
		hSync = SyncRiseToFall -1;
		hBPorch = HRiseToActEnd - hActive - hSync;
	}

	//---------------------
	//vertical
	//---------------------
	SyncRiseToFall = MeasGetVSyncRiseToFallWidth();

	//get vActive
	vActive = MeasGetVActive( &vStart );

	//get vTotal
	vTotal = MeasGetVPeriod();

	//get vPol
	if(SyncRiseToFall > vActive)	vPol = 0;	//high at active
	else							vPol = 1;

	//get vFPorch vSync vBPorch
	if(vPol==0) {
		vSync = vTotal - SyncRiseToFall;
		vBPorch = vStart;
		vFPorch = vTotal - vActive - vSync - vBPorch;
	}
	else {
		vSync = SyncRiseToFall;
		vBPorch = vStart - vSync;
		vFPorch = vTotal - vActive - vSync - vBPorch;
	}
	Printf("\n\rMeasure");
	Printf("\n\r\tH total:%d Sync:%d BPorch:%d Active:%d FPorch:%d Pol:%bd",hTotal,hSync,hBPorch,hActive,hFPorch,hPol);
	Printf("\n\r\tV total:%d Sync:%d BPorch:%d Active:%d FPorch:%d Pol:%bd",vTotal,vSync,vBPorch,vActive,vFPorch,vPol);
	Printf("\n\rhStart:%d vStart:%d",hStart,vStart);
}
#endif

void Measure_VideoTiming(void)
{
	WORD Period,FPorch,Sync,BPorch,Active;
	BYTE Pol;   //1:Active Low, 0:Active High (Low on Active area), 8836 prefer 0. 
	WORD Start; //if(Pol==1) Start=BPorch, else Start=Sync+BPorch
	BYTE bTemp;
	WORD wTemp;

	Puts("\nVertical ");

	bTemp = MeasGetVFreq();
	Printf("vFreq:%bd ",bTemp);

	Period = MeasGetVPeriod();
	Printf("vPeriod:%d ",Period);

	Active = MeasGetVActive(&Start);
	Start--;	//TW8836 adjust
	Printf("vActive:%d, vStart:%d ", Active, Start);

	wTemp = MeasGetVSyncRiseToFallWidth();
	if(wTemp > Active) 	Pol=1;
	else				Pol=0;
	Printf("vPol:%bd ",Pol);

	if(Pol==0) {
		Sync = wTemp;
		BPorch = Start-Sync;
		FPorch = Period - Active - Sync - BPorch;
	}
	else {
		BPorch = Start;
		FPorch = wTemp - BPorch - Active;
		Sync = Period - wTemp;
	}
	Printf("vFPorch:%d vSync:%d vBPorch:%d", FPorch,Sync,BPorch);
	
	Puts("\nHorizontal ");
	//we have a HPeriod, but, it use a PCLK, not a input clock.	Useless.
	//So, too difficult to calculate.
	//If do measure with REG508[4]=1, hSync register returns hPeriod.
	//But, it also need one more measure, that uses more than 16.6mS.
	//If hPol is active low, just read hStart and put to InputCrop.
	//On TW8836, we do not need to know hPeriod and hSync.

	Active = MeasGetHActive(&Start);
	Start+=4;	//TW8836 adjust
	Printf("hActive:%d, hStart:%d ", Active, Start);

	wTemp = MeasGetHSyncRiseToFallWidth();
	if(wTemp > Active) 	Pol=1;
	else				Pol=0;
	Printf("hPol:%bd ",Pol);

	if(Pol==0) {
		Sync = wTemp;
		BPorch = Start - Sync;
		//Read hPeriod from "VSync Rise Position in 1 H"
		Period = MeasGetVsyncRisePos();
		FPorch = Period - Active - Sync - BPorch;
	}
	else {
		BPorch = Start;
		FPorch = wTemp - BPorch - Active;
		//Unknown Period and Sync value....
		Period = Sync = 0;
#if 1	//If you want to know Period & Sync, you have to use two measures.
		WriteTW88(REG508, ReadTW88(REG508) | 0x10);
		Meas_StartMeasure();
		Meas_IsMeasureDone(50);
		Period = MeasGetHSyncRiseToFallWidth();
		Sync = Period - Active - BPorch - FPorch;

		WriteTW88(REG508, ReadTW88(REG508) & ~0x10);
		//If you want...
		//Meas_StartMeasure(); //for next read.
		//Meas_IsMeasureDone(50);
#endif
	}
	Printf("hFPorch:%d hSync:%d hBPorch:%d hPeriod:%d", FPorch,Sync,BPorch, Period);
}


