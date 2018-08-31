


#include "HWconf.h"

HAL_StatusTypeDef TIM_Init( TIM_InitTypedef* TimBody)
{
	switch( TimBody->TIM_Num )
	{
		case TIM0:
		{
			SFR_TMOD |= 0x06;
			SFR_TH0 =TimBody->TIM_HValue;
			SFR_TL0 =TimBody->TIM_LValue;
		}
			break;
		case TIM1:
		{
			SFR_TMOD |= 0x60;
			SFR_TH0 =TimBody->TIM_HValue;
			SFR_TL0 =TimBody->TIM_LValue;
		}
			break;
		case TIM2:
		{
			;
		}
			break;
	  default:
			return HAL_ERROR;
	}
	
	return HAL_OK;
}

HAL_StatusTypeDef DETIM_Init( TIM_InitTypedef* TimBody)
{
	return HAL_ERROR;
}

HAL_StatusTypeDef TIM_Start( TIM_InitTypedef* TimBody)
{
	return HAL_OK;
}