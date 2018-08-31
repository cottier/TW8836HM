#ifndef __DRV_LVDS_H__
#define __DRV_LVDS_H__
#include "HWdef.h"


HAL_StatusTypeDef	LVDSRinit(void );
HAL_StatusTypeDef	LVDSTinit(void );

BYTE CheckAndSetLVDSRx(void);
BYTE ChangeLVDSRx(void);
#endif