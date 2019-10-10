#include "sam.h"
#include "I2C.h"






int isBusIdleWIRE( void )
{
	return SERCOM1->I2CM.STATUS.bit.BUSSTATE == 0x01;
}

int isBusOwnerWIRE( void )
{
	return SERCOM1->I2CM.STATUS.bit.BUSSTATE == 0x02;
}



int sendDataMasterWIRE(uint8_t data)
{
	//Send data
	SERCOM1->I2CM.DATA.bit.DATA = data;

	//Wait transmission successful
	while(!SERCOM1->I2CM.INTFLAG.bit.MB) {

		// If a bus error occurs, the MB bit may never be set.
		// Check the bus error bit and bail if it's set.
		if (SERCOM1->I2CM.STATUS.bit.BUSERR) {
			return 0;
		}
	}

	//Problems on line? nack received?
	if(SERCOM1->I2CM.STATUS.bit.RXNACK)
	return 0;
	else
	return 1;
}


int startTransmissionWIRE(uint8_t address, uint8_t flag)
{
	//SERCOM1->I2CM.STATUS.bit.BUSSTATE = 1 ;
	//while ( SERCOM1->I2CM.SYNCBUSY.bit.SYSOP != 0 )
	// 7-bits address + 1-bits R/W
	address = (address << 0x1ul) | flag;

	// Wait idle or owner bus mode
	while ( !isBusIdleWIRE() && !isBusOwnerWIRE() );

	// Send start and address
	SERCOM1->I2CM.ADDR.bit.ADDR = address;

	// Address Transmitted
	if ( flag == 0) // Write mode
	{
		while( !SERCOM1->I2CM.INTFLAG.bit.MB )
		{
			// Wait transmission complete
		}
	}
	else  // Read mode
	{
		while( !SERCOM1->I2CM.INTFLAG.bit.SB )
		{
			// If the slave NACKS the address, the MB bit will be set.
			// In that case, send a stop condition and return 0.
			if (SERCOM1->I2CM.INTFLAG.bit.MB) {
				SERCOM1->I2CM.CTRLB.bit.CMD = 3; // Stop condition
				return 0;
			}
			// Wait transmission complete
		}

		// Clean the 'Slave on Bus' flag, for further usage.
		//SERCOM1->I2CM.INTFLAG.bit.SB = 0x1ul;
	}


	//ACK received (0: ACK, 1: NACK)
	if(SERCOM1->I2CM.STATUS.bit.RXNACK)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


int start_write_transmission(void){
	if ( !startTransmissionWIRE( SLAVE_ADDR, 0) )
	{
		SERCOM1->I2CM.CTRLB.bit.CMD = 0x03;
		while(SERCOM1->I2CM.SYNCBUSY.bit.SYSOP);
		
		return 2 ;  // Address error
	}
	return 0;
}

int write_i2c(int8_t data){
	// Trying to send data
	if ( !sendDataMasterWIRE(data))
	{
		SERCOM1->I2CM.CTRLB.bit.CMD = 0x03;
		while(SERCOM1->I2CM.SYNCBUSY.bit.SYSOP);
		return 3 ;  // Nack or error
	}
	return 0;
}

void end_transmission(void){
	SERCOM1->I2CM.CTRLB.bit.CMD = 0x03;
	while(SERCOM1->I2CM.SYNCBUSY.bit.SYSOP);

}


uint8_t readDataWIRE( void )
{
	while( SERCOM1->I2CM.INTFLAG.bit.SB == 0 )
	{
		// Waiting complete receive
	}

	return SERCOM1->I2CM.DATA.bit.DATA ;
}

uint8_t requestFrom(uint8_t address, uint8_t quantity)
{
	if(quantity == 0)
	{
		return 0;
	}

	uint8_t byteRead = 0;
	if(startTransmissionWIRE(address, 1))
	{
		// Read first data
		rx_buf[0]=readDataWIRE();
		while(SERCOM1->I2CM.SYNCBUSY.bit.SYSOP);
		// Connected to slave
		for (byteRead = 1; byteRead < quantity; ++byteRead)
		{
			SERCOM1->I2CM.CTRLB.bit.ACKACT=0;                          // Prepare Acknowledge
			while(SERCOM1->I2CM.SYNCBUSY.bit.SYSOP);
			SERCOM1->I2CM.CTRLB.bit.CMD = 0x2; // Prepare the ACK command for the slave
			while(SERCOM1->I2CM.SYNCBUSY.bit.SYSOP);
			rx_buf[byteRead]=readDataWIRE();          // Read data and send the ACK
		}
		SERCOM1->I2CM.CTRLB.bit.ACKACT=1;					// Prepare NACK to stop slave transmission
		//SERCOM1->readDataWIRE();                               // Clear data register to send NACK
		while(SERCOM1->I2CM.SYNCBUSY.bit.SYSOP);
		SERCOM1->I2CM.CTRLB.bit.CMD = 0x3;
		while(SERCOM1->I2CM.SYNCBUSY.bit.SYSOP);
	}

	return byteRead;
}



void i2cInit(){
	
	PM->APBCMASK.bit.SERCOM1_=1;
	PORT->Group[0].PINCFG[16].bit.PMUXEN=1;
	PORT->Group[0].PMUX[16>>1].bit.PMUXE=PORT_PMUX_PMUXE_C_Val;
	PORT->Group[0].PINCFG[17].bit.PMUXEN=1;
	PORT->Group[0].PMUX[17>>1].bit.PMUXO =PORT_PMUX_PMUXO_C_Val;
	
	GCLK->CLKCTRL.bit.ID=GCLK_CLKCTRL_ID_SERCOM1_CORE_Val;
	// while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
	GCLK->CLKCTRL.bit.GEN=0;
	// while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
	GCLK->CLKCTRL.bit.CLKEN=1;
	//while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
	
	SERCOM1->I2CM.CTRLA.bit.ENABLE=0;
	while(SERCOM1->I2CM.SYNCBUSY.bit.SYSOP);
	SERCOM1->I2CM.CTRLA.bit.MODE=0x05;
	while(SERCOM1->I2CM.SYNCBUSY.bit.SYSOP);
	SERCOM1->I2CM.BAUD.bit.BAUD=48;
	while(SERCOM1->I2CM.SYNCBUSY.bit.SYSOP);
	SERCOM1->I2CM.CTRLA.bit.RUNSTDBY=1;
	while(SERCOM1->I2CM.SYNCBUSY.bit.SYSOP);
	SERCOM1->I2CM.CTRLA.bit.ENABLE=1;
	while(SERCOM1->I2CM.SYNCBUSY.bit.SYSOP);
	SERCOM1->I2CM.STATUS.bit.BUSSTATE=0x01;
	while(SERCOM1->I2CM.SYNCBUSY.bit.SYSOP);
	NVIC_EnableIRQ(SERCOM1_IRQn);
	
	
	
}
