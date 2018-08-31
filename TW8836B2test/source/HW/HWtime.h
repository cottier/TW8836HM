

#ifndef __HWTIME_H__
#define __HWTIME_H__

#include "HWdef.h"
#define TIM0      0
#define TIM1      1
#define TIM2      2
#define TIM_MAX   2


#define T8BITDIV32     (0U)
#define T16BIT         (1U)
#define T8BIT_RELOAD 	 (2U)
#define T8BIT          (3U)

#define TIM0_MODE_POS   (0)
#define TIM1_MODE_POS   (4)
#define TIM0_MODE_MASK  (0x3U)
#define TIM1_MODE_MASK  (0x3U)
#define TIM0_MODE_16BIT ()



#define TIMEMODE      (0)
#define COUNTMODE     (1)

typedef struct {
	BYTE TIM_Num;
	WORD TIM_Prescaler;
	BYTE TIM_Mode;           //
	BYTE  TIM_TorC;
	BYTE  TIM_Gate;
	BYTE TIM_Priority;       //
	BYTE TIM_InterrruptOnOff;     //en de
	BYTE TIM_HValue;
	BYTE TIM_LValue;
}TIM_InitTypedef;

HAL_StatusTypeDef TIM_Init( TIM_InitTypedef* );//初始化
HAL_StatusTypeDef DETIM_Init( TIM_InitTypedef*); //反初始化
HAL_StatusTypeDef TIM_Start( TIM_InitTypedef* );//启动
#endif