

#include "sam.h"
#include "definitions.h"

//static uint32_t ul_tickcount=0 ;	// Global state variable for tick count



void AppInit(void);			// Application Hardware/Software Initialization
void ClocksInit(void);	// Configure Clock, Wait States and synch. Bus Clocks for 48MHz operation



int main(void){
	

	SystemInit();
	
	/* Application hardware and software initialization */
	AppInit();

	/* Replace with your application code */
	
	while(1){
		PORT->Group[PORTA].PINCFG[PORT_PA14].bit.INEN=1;
		if(PORT->Group[PORTA].IN.reg & PORT_PA14){
			PORT->Group[LED0_PORT].OUTSET.reg = LED0_PIN_MASK;
				PORT->Group[LED0_PORT].OUTCLR.reg = PORT_PA12;

		}
		
	}
} // main()


void AppInit(void){
	
	ClocksInit();
	
	// Assign the LED0 pin as OUTPUT
	PORT->Group[LED0_PORT].DIRSET.reg = LED0_PIN_MASK;
	// Set the LED0 pin level, i.e. put to 3.3V -> this light off the LED
	PORT->Group[LED0_PORT].OUTCLR.reg = LED0_PIN_MASK;
	PORT->Group[LED0_PORT].DIRSET.reg = PORT_PA12;
	// Set the LED0 pin level, i.e. put to 3.3V -> this light off the LED
	PORT->Group[LED0_PORT].OUTSET.reg = PORT_PA12;
	
	PORT->Group[PORTA].DIRCLR.reg= PORT_PA14;
	PORT->Group[PORTA].OUTSET.reg =PORT_PA14;
	PORT->Group[PORTA].PINCFG[PORT_PA14].bit.PULLEN=1;
} // AppInit()





