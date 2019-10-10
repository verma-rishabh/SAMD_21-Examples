

#include "sam.h"
#include "definitions.h"


void uart_putc(char c)
{
	while (SERCOM5->USART.INTFLAG.bit.DRE!=1);
	SERCOM5->USART.DATA.reg = c;
}

void TC3_Handler(void){
	TC3->COUNT16.INTENCLR.bit.MC0=1;
	//TC3->COUNT16.INTFLAG.bit.MC0 =1;								// gives warning
	TC3->COUNT16.INTFLAG.reg |= 0b00010000;						// doesn't give warning				
	PORT->Group[LED0_PORT].OUTTGL.reg = LED0_PIN_MASK;
	TC3->COUNT16.INTENSET.bit.MC0=1;
	uart_putc(c);
	
}

void AppInit(void);			// Application Hardware/Software Initialization
void ClocksInit(void);	// Configure Clock, Wait States and synch. Bus Clocks for 48MHz operation
uint16_t TOP=65000;



int uart_has_data()
{
	if (SERCOM5->USART.INTFLAG.bit.RXC==1) {
		return 1;
	}
	return 0;
}

static char uart_getc()
{
	return SERCOM5->USART.DATA.reg;
}


uint16_t br = (uint64_t)65536 * (48000000 - 16 * 9600) / 48000000;              // IMP keep uint64_t

void InitUart(){
	
	PORT->Group[1].PINCFG[30].bit.PMUXEN=1;
	PORT->Group[1].PMUX[30>>1].bit.PMUXE=PORT_PMUX_PMUXE_D_Val;
	PORT->Group[1].PINCFG[31].bit.PMUXEN=1;
	PORT->Group[1].PMUX[31>>1].bit.PMUXO=PORT_PMUX_PMUXO_D_Val;
	PM->APBCMASK.bit.SERCOM5_=1;
	SERCOM5->USART.CTRLA.bit.ENABLE=0;
	
	
	GCLK->CLKCTRL.bit.ID=GCLK_CLKCTRL_ID_SERCOM5_CORE_Val;
	GCLK->CLKCTRL.bit.GEN=0x00;
	GCLK->CLKCTRL.bit.CLKEN=1;
	
	SERCOM5->USART.CTRLA.bit.MODE=1;
	SERCOM5->USART.CTRLA.bit.CMODE=0;
	SERCOM5->USART.CTRLB.bit.SBMODE=0;
	SERCOM5->USART.CTRLA.bit.SAMPR=0;
	SERCOM5->USART.CTRLA.bit.FORM=0x00;
	SERCOM5->USART.CTRLA.bit.RXPO=0x01;
	SERCOM5->USART.CTRLA.bit.TXPO=0x00;
	
	SERCOM5->USART.CTRLB.bit.CHSIZE=0x00;
	SERCOM5->USART.BAUD.reg=br;
	SERCOM5->USART.CTRLB.bit.RXEN=1;
	SERCOM5->USART.CTRLB.bit.TXEN=1;
	SERCOM5->USART.CTRLA.bit.ENABLE=1;
	while(SERCOM5->USART.SYNCBUSY.bit.ENABLE);
	
	

	
	
}

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
		 if (uart_has_data()) {
			 char c = uart_getc();
			 uart_putc(c);
		 }
	
	}
} // main()


void AppInit(void){
	
	ClocksInit();
	InitTimer();
	InitUart();
	//Temp();
	// Assign the LED0 pin as OUTPUT
	PORT->Group[LED0_PORT].DIRSET.reg = LED0_PIN_MASK;
	// Set the LED0 pin level, i.e. put to 3.3V -> this light off the LED
	PORT->Group[LED0_PORT].OUTCLR.reg = LED0_PIN_MASK;
	
	// Set the LED0 pin level, i.e. put to 3.3V -> this light off the LED
	
} // AppInit()





