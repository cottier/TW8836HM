/****************************************************************
* 
*
*
*
*
*****************************************************************/

#include "HWdef.h"
#include "HWconf.h"
#include "main.h"

WORD Count10MS = 0,Count1MS = 0;
unsigned char flag_Tog = 1;

//-----------------------------------------------------------------------------
// Freerun value				                                               
//-----------------------------------------------------------------------------
XDATA WORD global_Freerun_hTotal;
XDATA WORD global_Freerun_vTotal;


static HAL_StatusTypeDef McuInit(void);
void main()
{
	unsigned int temp = 1;
	SFRB_EA   = 0;			// Global Interrupt CLOSE
	if(OK != InitCpu())while(1);
//	InitCore( );
	//---------- Enable global Interrupt ----------
	SFRB_EA   = 1;			// Global Interrupt EN
		
	McuInit();
	Puts("init is ok!\n");
	while(1)
	{
		if( Count1MS > 1000 )
		{
			Count1MS = 1;
			WriteTW88(REG094, ((~ReadTW88(REG094))&0x02));   //LED翻转
      Printf("test:  %d\n",temp++); 
		}
	}
}
//  McuInit()            内核外设初始化
//    +--SysclkInit()
//	  +--SpiDmaInit()
//	  +--TimeInit()
//	  +--GpioInit()
HAL_StatusTypeDef McuInit(void)
{
	
	SysclkInit();
	
	return HAL_OK;
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
