
#include "HWconf.h"

void InitVGA(void )
{
	Puts("VGA is start!\n");
	Init8836AsDefault(1,1);
	Puts("\nVGA is OK!\n");
}

