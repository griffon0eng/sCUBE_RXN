//###########################################################################
//
// FILE:	DSP2833x_Sci.c
//
// TITLE:	DSP2833x SCI Initialization & Support Functions.
//
//###########################################################################
// $TI Release: 2833x/2823x Header Files and Peripheral Examples V133 $
// $Release Date: June 8, 2012 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

//---------------------------------------------------------------------------
// InitSci: 
//---------------------------------------------------------------------------
// This function initializes the SCI(s) to a known state.
//
//Uint32 baud;															//191202 통신속도 38400으로 변경

void InitSci(void)
{
	// Initialize SCI-A:

	//tbd...
 	

	// Initialize SCI-B:

	//tbd...
  
      // Initialize SCI-C:

      //tbd...
}	

//---------------------------------------------------------------------------
// Example: InitSciGpio: 
//---------------------------------------------------------------------------
// This function initializes GPIO pins to function as SCI pins
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.  
// 
// Caution: 
// Only one GPIO pin should be enabled for SCITXDA/B operation.
// Only one GPIO pin shoudl be enabled for SCIRXDA/B operation. 
// Comment out other unwanted lines.

void InitSciGpio()
{
   InitSciaGpio();
#if DSP28_SCIB   
   InitScibGpio();
#endif // if DSP28_SCIB  
#if DSP28_SCIC
   InitScicGpio();
#endif // if DSP28_SCIC
}

void InitSciaGpio()
{
   EALLOW;

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.  
// This will enable the pullups for the specified pins.

	GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;    // Enable pull-up for GPIO28 (SCIRXDA)
	GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;	   // Enable pull-up for GPIO29 (SCITXDA)

/* Set qualification for selected pins to asynch only */
// Inputs are synchronized to SYSCLKOUT by default.  
// This will select asynch (no qualification) for the selected pins.

	GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (SCIRXDA)

/* Configure SCI-A pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SCI functional pins.

	GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;   // Configure GPIO28 for SCIRXDA operation
	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;   // Configure GPIO29 for SCITXDA operation
	
    EDIS;
    /* SCI 설정 */
    SciaRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
								 // No parity,8 char bits,
								 // async mode, idle-line protocol
    SciaRegs.SCICTL1.all =0x0003;  // enable RX, internal SCICLK,
								 // Disable RX ERR, SLEEP, TXWAKE, TX

//통신속도 38400으로 변경 191202
    /*baud = ((Uint32) 37500000 / (38400*8) - 1 );
    SciaRegs.SCIHBAUD = (baud&0xFF00) >> 8;
    SciaRegs.SCILBAUD = (baud&0x00FF);*/
    SciaRegs.SCIHBAUD = 0x1;       // 9600 bps
    SciaRegs.SCILBAUD = 0xE7;        // 243.1525 = 19200bps  // 0xE7 = 9606bps 약 9600bps 0.06%오차
  /*통신속도 구하는 공식   :  속도 = LSPLCK/(SCIHBAUD & SCILBAUD +1)X 8
  						 9600	585966        487.305       +1 X 8   */
    SciaRegs.SCICTL1.bit.SWRESET = 1;     // Relinquish SCI from Reset
}

#if DSP28_SCIB 
void InitScibGpio()
{
   EALLOW;
	
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.  
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;    // Enable pull-up for GPIO14 (SCITXDB)
    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;    // Enable pull-up for GPIO15 (SCIRXDB)

/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;  // Asynch input GPIO15 (SCIRXDB)

/* Configure SCI-B pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SCI functional pins.
// Comment out other unwanted lines.

  GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 2;   // Configure GPIO14 for SCITXDB operation
  GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 2;   // Configure GPIO15 for SCIRXDB operation
	
    EDIS;
    /* SCI 설정 */
    ScibRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
  								 // No parity,8 char bits,
  								 // async mode, idle-line protocol
    ScibRegs.SCICTL1.all =0x0003;  // enable RX, internal SCICLK,
  								 // Disable RX ERR, SLEEP, TXWAKE, TX

    ScibRegs.SCIHBAUD = 0x1;       // 9600 bps
    ScibRegs.SCILBAUD = 0xE7;

    ScibRegs.SCICTL1.bit.SWRESET = 1;     // Relinquish SCI from Reset
}
#endif // if DSP28_SCIB 

#if DSP28_SCIC
void InitScicGpio()
{
   EALLOW;

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.  
// This will enable the pullups for the specified pins.

	GpioCtrlRegs.GPBPUD.bit.GPIO62 = 0;    // Enable pull-up for GPIO62 (SCIRXDC)
	GpioCtrlRegs.GPBPUD.bit.GPIO63 = 0;	   // Enable pull-up for GPIO63 (SCITXDC)

/* Set qualification for selected pins to asynch only */
// Inputs are synchronized to SYSCLKOUT by default.  
// This will select asynch (no qualification) for the selected pins.

	GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 3;  // Asynch input GPIO62 (SCIRXDC)

/* Configure SCI-C pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SCI functional pins.

	GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 1;   // Configure GPIO62 for SCIRXDC operation
	GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 1;   // Configure GPIO63 for SCITXDC operation
	
    EDIS;
    /* SCI 설정 */ //190830 NewMB
        ScicRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
    								 // No parity,8 char bits,
    								 // async mode, idle-line protocol
        ScicRegs.SCICTL1.all =0x0003;  // enable RX, internal SCICLK,
    								 // Disable RX ERR, SLEEP, TXWAKE, TX

        ScicRegs.SCIHBAUD = 0x1;       // 9600 bps
        ScicRegs.SCILBAUD = 0xE7;        // 243.1525 = 19200bps  // 0xE7 = 9606bps 약 9600bps 0.06%오차
      /*통신속도 구하는 공식   :  속도 = LSPLCK/(SCIHBAUD & SCILBAUD +1)X 8
      						 9600	585966        487.305       +1 X 8   */
        ScicRegs.SCICTL1.bit.SWRESET = 1;     // Relinquish SCI from Reset
}
#endif // if DSP28_SCIC 
//----------------------------------------------------------------------------------
/*char scia_Rx(void)
{
	char data;
	while(!(SciaRegs.SCIRXST.bit.RXRDY));
	data = SciaRegs.SCIRXBUF.bit.RXDT;	 // Read data
	return data;
}

void scia_Tx(char data)
{
	while(!(SciaRegs.SCICTL2.bit.TXRDY));
    SciaRegs.SCITXBUF = data & 0x00FF;
}

void string_Tx(const char * msg)
{
	char ch;
	if(msg)
		while(0 != (ch = *(msg++))){
			scia_Tx(ch);
		}
}*/

	
//===========================================================================
// End of file.
//===========================================================================
