#include "HWconf.h"
#include "interrupt.h"


void timer0_int(void) interrupt 1 using 0	//10ms
{
	Count10MS++;
}


void timer1_int(void) interrupt 3 using 1	//1ms
{
	tic_pc++;
	Count1MS++;
}

/**
* UART0 interrupt service routine
* 
* enable SFR_ES0
*/
void uart0_int(void) interrupt 4 using 2
{
	BYTE	count;

#ifdef DEBUG_ISR
	MCU_INT_STATUS |= 0x0010;
	INTR_counter[4]++;
#endif

	if( SFRB_RI ) {					//--- Receive interrupt ----
		SFRB_RI = 0;
		//if ( SFR_UART0FIFO & 0x80 ) {		// is it use fifo?
			count = SFR_UART0FIFO & 0x1F;
			if ( count & 0x10) {
				SFR_UART0FIFO = 0x90;		// overflowed, buffer clear
#ifdef DEBUG_UART
				UART0_OVERFLOW_counter++;
#endif
			}
			else {
#ifdef DEBUG_UART
				if(UART0_MAX_counter < count)
					UART0_MAX_counter = count;
#endif
				while (count) {
					RS_buf[RS_in++] = SFR_SBUF;
					if( RS_in>=RS_BUF_MAX ) RS_in = 0;
					count--;
				};
			}
		//}
		//else {
		//	RS_buf[RS_in++] = SFR_SBUF;
		//	if( RS_in>=RS_BUF_MAX ) RS_in = 0;
		//}
	}

	if( SFRB_TI ) {					//--- Transmit interrupt ----
		SFRB_TI = 0;
		RS_Xbusy=0;
	}
}

