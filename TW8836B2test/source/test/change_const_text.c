#include "HWconf.h"

const unsigned char TEXT[]="\
	0x40, 0x01,\
  0x41, 0x01,\
  0x42, 0x23\
";
void ChangeText()
{
	Puts("\nchange start!\n\n");
	Puts(TEXT);
}
//struct SCALER_TIME_TABLE_s * FindScalerTable(BYTE q, BYTE hActive,BYTE vActive, BYTE vFreq,BYTE vStart,vPol)
//{
//	return 0;
//}