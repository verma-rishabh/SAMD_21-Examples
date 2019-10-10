

#include "sam.h"
#include "definitions.h"
#define BUF_SIZE 4
#define SLAVE_ADDR 0

uint8_t tx_buf[BUF_SIZE] = {1, 2, 3};
uint8_t rx_buf[BUF_SIZE];
uint8_t i;
volatile int rx_done = 0;


//static uint32_t ul_tickcount=0 ;	// Global state variable for tick count



void AppInit(void);			// Application Hardware/Software Initialization
void ClocksInit(void);	// Configure Clock, Wait States and synch. Bus Clocks for 48MHz operation
void i2cInit(void);
void i2cRead(void);	


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
	i2cInit();
	
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


void i2cInit(){
	
	PM->APBCMASK.bit.SERCOM1_=1;
	PORT->Group[0].PINCFG[17].bit.PMUXEN=1;
	PORT->Group[0].PMUX[17>>1].bit.PMUXO=PORT_PMUX_PMUXO_C_Val;
	PORT->Group[0].PINCFG[18].bit.PMUXEN=1;
	PORT->Group[0].PMUX[18>>1].bit.PMUXO=PORT_PMUX_PMUXE_C_Val;
	
	GCLK->CLKCTRL.bit.ID=GCLK_CLKCTRL_ID_SERCOM1_CORE_Val;
	GCLK->CLKCTRL.bit.GEN=0;
	GCLK->CLKCTRL.bit.CLKEN=1;
	
	SERCOM2->I2CM.CTRLA.bit.ENABLE=0;
	SERCOM2->I2CM.CTRLA.bit.MODE=0x05;
	SERCOM2->I2CM.CTRLA.bit.SCLSM=0x01;
	SERCOM2->I2CM.CTRLA.bit.SPEED=0x00;
	SERCOM2->I2CM.CTRLA.bit.SDAHOLD=0x03;
	SERCOM2->I2CM.CTRLB.bit.SMEN=0x01;
	SERCOM2->I2CM.BAUD.bit.BAUD=52;
	SERCOM2->I2CM.CTRLA.bit.ENABLE=1;
	while(SERCOM2->I2CM.SYNCBUSY.bit.SYSOP);
	SERCOM2->I2CM.STATUS.bit.BUSSTATE=0x01;
	SERCOM2->I2CM.INTENSET.bit.MB=1;
	NVIC_EnableIRQ(SERCOM2_IRQn);			
	
	
	
}


void SERCOM2_Handler(void){
	
	if (SERCOM2->I2CM.INTFLAG.bit.SB)
	{
		if (i == (BUF_SIZE-1))
		{ /* NACK should be sent before reading the last byte */
			SERCOM2->I2CM.CTRLB.bit.ACKACT=1;
			while(SERCOM2->I2CM.SYNCBUSY.bit.SYSOP);
			SERCOM2->I2CM.CTRLB.bit.CMD = 0x3;
			while(SERCOM2->I2CM.SYNCBUSY.bit.SYSOP);
			rx_buf[i++] = SERCOM2->I2CM.DATA.reg;
			while(SERCOM2->I2CM.SYNCBUSY.bit.SYSOP);
			rx_done = 1;
		}
		else
		{
			SERCOM2->I2CM.CTRLB.bit.ACKACT=0;
			while(SERCOM2->I2CM.SYNCBUSY.bit.SYSOP);
			rx_buf[i++] = SERCOM2->I2CM.DATA.reg;
			while(SERCOM2->I2CM.SYNCBUSY.bit.SYSOP);
			/* sending ACK after reading each byte */
			SERCOM2->I2CM.CTRLB.bit.CMD = 0x2;
			while(SERCOM2->I2CM.SYNCBUSY.bit.SYSOP);
		}
	}
	
	
}

void i2cRead(void){
	i = 0;
	/* Acknowledge section is set as ACK signal by
	writing 0 in ACKACT bit */
	SERCOM2->I2CM.CTRLB.bit.ACKACT=0;
	while(SERCOM2->I2CM.SYNCBUSY.bit.SYSOP);
	/* slave address with read (1) */
	SERCOM2->I2CM.ADDR.reg = (SLAVE_ADDR << 1) | 1;
	while(!rx_done);
	/*interrupts are cleared */
	SERCOM2->I2CM.INTENCLR.reg = SERCOM_I2CM_INTENCLR_MB | SERCOM_I2CM_INTENCLR_SB;
	
}
