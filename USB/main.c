

#include "sam.h"
#include "definitions.h"



void TC3_Handler(void){
	TC3->COUNT16.INTENCLR.bit.MC0=1;
	TC3->COUNT16.INTFLAG.bit.MC0 =1;								// gives warning
	//TC3->COUNT16.INTFLAG.reg |= 0b00010000;						// doesn't give warning				
	PORT->Group[LED0_PORT].OUTTGL.reg = LED0_PIN_MASK;
	TC3->COUNT16.INTENSET.bit.MC0=1;
}

void AppInit(void);			// Application Hardware/Software Initialization
void ClocksInit(void);	// Configure Clock, Wait States and synch. Bus Clocks for 48MHz operation
uint16_t TOP=65000;
void InitTimer(){		
	TC3->COUNT16.CTRLA.bit.ENABLE=0;
	GCLK->CLKCTRL.bit.ID=GCLK_CLKCTRL_ID_TCC2_TC3_Val;
	GCLK->CLKCTRL.bit.GEN=0x00;
	GCLK->CLKCTRL.bit.CLKEN=1;
	PM->APBCMASK.bit.TC3_=1;
	TC3->COUNT16.INTENSET.bit.MC0=1;	
	TC3->COUNT16.CTRLA.bit.PRESCALER=0x07;
	TC3->COUNT16.CC[0].reg=TOP;
	TC3->COUNT16.INTENSET.bit.MC0=1;
	TC3->COUNT16.CTRLA.bit.ENABLE = 1;
	

	// Wait until TC3 is enabled
	while(TC3->COUNT16.STATUS.bit.SYNCBUSY == 1);
	NVIC_SetPriority(TC3_IRQn,3);
	/* Enable TC3 NVIC Interrupt Line */
	NVIC_EnableIRQ(TC3_IRQn);
	void __enable_irq(void);
}



int main(void){
	

	SystemInit();
	
	/* Application hardware and software initialization */
	AppInit();

	/* Replace with your application code */
	
	while(1){
	
	}
} // main()


void AppInit(void){
	
	ClocksInit();
	InitTimer();
	//Temp();
	// Assign the LED0 pin as OUTPUT
	PORT->Group[LED0_PORT].DIRSET.reg = LED0_PIN_MASK;
	// Set the LED0 pin level, i.e. put to 3.3V -> this light off the LED
	PORT->Group[LED0_PORT].OUTCLR.reg = LED0_PIN_MASK;
	
	// Set the LED0 pin level, i.e. put to 3.3V -> this light off the LED
	
	 pin_mux(PIN_USB_DM);
	 pin_mux(PIN_USB_DP);
	 usb_init();
	 usb_attach();
	 NVIC_SetPriority(USB_IRQn, 0xff);
	
} // AppInit()





