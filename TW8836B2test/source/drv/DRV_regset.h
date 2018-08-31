#ifndef __DRV_REGSET_H__
#define __DRV_REGSET_H__

typedef struct REG_IDX_DATA_s {
	WORD idx;
	BYTE value;
} REG_IDX_DATA_t;

void Init8836AsDefault(BYTE , BYTE );
void RegReset(void );
#endif