

#include "sam.h"
#include "definitions.h"	




void AppInit(void);			
void ClocksInit(void);	
void PortInit(void);


int main(void){
	AppInit();
	while(1){
		// Check the status of BUTTON
		if(!(PORT->Group[BUTTON_PORT].IN.reg & BUTTON_PIN_MASK)){
			//Switch OFF LED0 and ON LED1
			PORT->Group[LED0_PORT].OUTSET.reg = LED0_PIN_MASK;
				PORT->Group[LED1_PORT].OUTCLR.reg = LED1_PIN_MASK;

		}
		else{
			//Switch ON LED0 and OFF LED1
			PORT->Group[LED0_PORT].OUTCLR.reg = LED0_PIN_MASK;
			PORT->Group[LED1_PORT].OUTSET.reg = LED1_PIN_MASK;
		}
		
	}
} 


void AppInit(void){
	//Initialize clock to run at 48MHz
	ClocksInit();
	//Initialize IO Pins 
	PortInit();
	
	
} 

void PortInit(void){
	// Set LED0 pin as Out and switch it ON
	PORT->Group[LED0_PORT].DIRSET.reg = LED0_PIN_MASK;
	PORT->Group[LED0_PORT].OUTCLR.reg = LED0_PIN_MASK;
	
	// Set LED1 pin as Out and switch it OFF
	PORT->Group[LED1_PORT].DIRSET.reg = LED1_PIN_MASK;
	PORT->Group[LED1_PORT].OUTSET.reg = LED1_PIN_MASK;
	
	// Set BUTTON pin as IN and enable Pull up resistor 
	PORT->Group[BUTTON_PORT].DIRCLR.reg= BUTTON_PIN_MASK;
	PORT->Group[BUTTON_PORT].OUTSET.reg = BUTTON_PIN_MASK;
	PORT->Group[BUTTON_PORT].PINCFG[BUTTON_PIN_NUMBER].bit.PULLEN=1;
	// Enable input buffer
	PORT->Group[BUTTON_PORT].PINCFG[BUTTON_PIN_NUMBER].bit.INEN=1;
}



