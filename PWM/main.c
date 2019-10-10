

#include "sam.h"
#include "definitions.h"



void TC3_Handler(void){
	TC3->COUNT16.INTENCLR.bit.MC0=1;
	//TC3->COUNT16.INTFLAG.bit.MC0 =1;								// gives warning
	TC3->COUNT16.INTFLAG.reg |= 0b00010000;						// doesn't give warning				
	PORT->Group[LED0_PORT].OUTTGL.reg = LED0_PIN_MASK;
	TC3->COUNT16.INTENSET.bit.MC0=1;
}

void AppInit(void);			// Application Hardware/Software Initialization
void ClocksInit(void);	// Configure Clock, Wait States and synch. Bus Clocks for 48MHz operation
uint16_t TOP=65000;
void InitTimer(){		
	PORT->Group[0].PINCFG[9].bit.PMUXEN=1;
	PORT->Group[0].PMUX[13>>1].bit.PMUXE=PORT_PMUX_PMUXO_E_Val;
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

void InitTimer1(){
	PORT->Group[1].PINCFG[10].bit.PMUXEN=1;
	PORT->Group[1].PMUX[10>>1].bit.PMUXE=PORT_PMUX_PMUXE_E_Val;
	TC5->COUNT16.CTRLA.bit.ENABLE=0;
	GCLK->CLKCTRL.bit.ID=GCLK_CLKCTRL_ID_TC4_TC5_Val;
	GCLK->CLKCTRL.bit.GEN=0x00;
	GCLK->CLKCTRL.bit.CLKEN=1;
	PM->APBCMASK.bit.TC5_=1;
	TC5->COUNT16.INTENSET.bit.MC0=1;
	TC5->COUNT16.CTRLA.bit.PRESCALER=0x00;
	TC5->COUNT16.CC[0].reg=9600;
	TC5->COUNT16.CC[1].reg=4800;
	TC5->COUNT16.INTENSET.bit.MC0=1;
	TC5->COUNT16.CTRLA.bit.ENABLE = 1;
	

	// Wait until TC5 is enabled
	NVIC_SetPriority(TC5_IRQn,3);
	/* Enable TC5 NVIC Interrupt Line */
	NVIC_EnableIRQ(TC5_IRQn);
	void __enable_irq(void);
}

void init_extint(void){
	PM->APBAMASK.bit.EIC_ =1;
	GCLK->CLKCTRL.bit.ID=GCLK_CLKCTRL_ID_EIC_Val;
	GCLK->CLKCTRL.bit.GEN=0x00;
	GCLK->CLKCTRL.bit.CLKEN=1;
	
	PORT->Group[0].DIRCLR.reg=PORT_PA02;
	 PORT->Group[0].PINCFG[2].reg = PORT_PINCFG_PULLEN;
	 PORT->Group[0].PINCFG[2].reg = PORT_PINCFG_PMUXEN;
	 PORT->Group[0].PMUX[1].reg = PORT_PMUX_PMUXE_A;
	 EIC->INTENSET.reg = EIC_INTENSET_EXTINT2;
	 EIC->CONFIG[0].bit.SENSE2 = EIC_CONFIG_SENSE2_BOTH_Val;
	 EIC->CTRL.reg |= EIC_CTRL_ENABLE;
	NVIC_SetPriority(EIC_IRQn,3);
	 NVIC_EnableIRQ(EIC_IRQn);
	
		
}


uint16_t t;
void EIC_Handler() {
	PORT->Group[LED0_PORT].OUTSET.reg = LED0_PIN_MASK;
	PORT->Group[LED0_PORT].OUTCLR.reg = PORT_PA12;
	/*
	if(!(PORT->Group[1].IN.reg& PORT_PB10 ) ){
		PORT->Group[LED0_PORT].OUTCLR.reg = LED0_PIN_MASK;
	}
	else{
		PORT->Group[LED0_PORT].OUTSET.reg = LED0_PIN_MASK;
	}
	*/
		EIC->INTENCLR.bit.EXTINT2=1;
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
	//InitTimer();
	//InitTimer1();
	init_extint();
	//Temp();
	// Assign the LED0 pin as OUTPUT
	PORT->Group[LED0_PORT].DIRSET.reg = LED0_PIN_MASK;
	// Set the LED0 pin level, i.e. put to 3.3V -> this light off the LED
	PORT->Group[LED0_PORT].OUTCLR.reg = LED0_PIN_MASK;
	PORT->Group[LED0_PORT].DIRSET.reg = PORT_PA12;
	// Set the LED0 pin level, i.e. put to 3.3V -> this light off the LED
	PORT->Group[LED0_PORT].OUTSET.reg = PORT_PA12;
	
	// Set the LED0 pin level, i.e. put to 3.3V -> this light off the LED
	
} // AppInit()





