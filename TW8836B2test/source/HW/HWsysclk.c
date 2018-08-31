/**
  ******************************************************************************
  * @file    HWsysclk.c
  * @author  BQsong
  * @version V0.0.1
  * @date    21-8-2018
  * @brief   Initialization clock.about pll,spi,mcu,pixel,
  ******************************************************************************
  * @attention

  *
    ******************************************************************************
  */

/**
  ******************************************************************************
  * History:
  ******************************************************************************
  * Version 0.01
  *   Initial release
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "HWconf.h"



/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
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
/** --------------------------------------------------------------------------*
  * @fn         
  * @brief      
  * @param[in]  
  * @return     None
  * --------------------------------------------------------------------------*/
static void SpiClkRecover27MSource(void)
{
	BYTE temp;

    //Clock
	temp = ReadTW88(REG4E1);        //设置SPICLK 为27Mhz
	if(temp & 0x30)
	{
		WriteTW88(REG4E1, temp & ~0x30);
	}
	temp = ReadTW88(REG4F0);        //设置MCUCLK = SPICLK DIV 1;
	if(temp & 0xF0)
	{
		WriteTW88(REG4F0, temp& ~0xF0);
	}
  Puts(" 27MHz");
}
static void Sspllinit(SSPLL num,WORD nKhz)
{
	if(SSPLL1 == num)
	{
		WriteTW88(REG0FD,0x34);// POST = 0，pump = 4，VCO=3
		WriteTW88(REG0F8,0x02);// (108000000 * FPLL)/2^17 /2^POST
		WriteTW88(REG0F9,0x00);
		WriteTW88(REG0FA,0x00);
		WriteTW88(REG0FC,ReadTW88(REG0FC)& ~0x80);
	}	
	else if(SSPLL2 == num)
	{
		WriteTW88(REG0ED,0x34);// POST = 0，pump = 4，VCO=3
		WriteTW88(REG0E8,0x02);//
		WriteTW88(REG0E9,0x00);
		WriteTW88(REG0EA,0x00);
		WriteTW88(REG0EC,ReadTW88(REG0EC)& ~0x80);
	}
}
 void Pclkinit(SSPLL num,BYTE div)
{
	if(num == SSPLL1)
	{
		WriteTW88(REG04B,ReadTW88(REG04B)|0x10); //bit4 = 1
	}
	else if(num ==SSPLL2)
	{
		WriteTW88(REG04B,ReadTW88(REG04B)&~0x10);//bit4 = 0
	}
	WriteTW88(REG0F6,0x00);                    //DIV = 0
}
HAL_StatusTypeDef SysclkInit(void )
{
	BYTE temp = 0;
	SSPLL pll;
	SpiClkRecover27MSource();            //系统时钟
	pll = SSPLL1;
	Sspllinit(pll,10800);                  //sspll1 = 108Mhz
	pll = SSPLL2;
	Sspllinit(pll,10800);                  //sspll2 = 108Mhz
	pll = SSPLL2;
  Pclkinit(pll,0);                             
	Puts("clock is ok!\n");
	return HAL_OK;
}


/**
  *****************************************************************************
  * END OF FILE                                                               *
  *****************************************************************************
  */