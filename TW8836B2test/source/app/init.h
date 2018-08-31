#ifndef __INIT_H__
#define __INIT_H__

#include "HWdef.h"

#define RS_BUF_MAX 	32
extern DATA 	BYTE	RS_buf[];
extern DATA 	BYTE	RS_in;
extern DATA	BYTE	RS_out;
extern bit		RS_Xbusy;
extern DATA 	WORD   	tic_pc;


typedef struct{
	unsigned char num;
	char * ADD;
}CMDCTL_T;
void UARTtoCMD(void );
unsigned char InitCpu();


void delay1ms(WORD );
void RS_tx(BYTE );


#endif