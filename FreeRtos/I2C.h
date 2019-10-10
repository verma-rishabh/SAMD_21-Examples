/*
 * CFile1.c
 *
 * Created: 05-07-2019 17:21:32
 *  Author: abc
 */ 
#ifndef I2C_H
#define I2C _H

#define SLAVE_ADDR 0x68

uint8_t rx_buf[14];

void i2cInit(void);
void i2cRead(void);
int isBusIdleWIRE( void );
int isBusOwnerWIRE( void );
int sendDataMasterWIRE(uint8_t data);
int startTransmissionWIRE(uint8_t address, uint8_t flag);
int start_write_transmission(void);
int write_i2c(int8_t data);
void end_transmission(void);
uint8_t readDataWIRE( void );
uint8_t requestFrom(uint8_t address, uint8_t quantity);
void i2cInit();
#endif // ASF_H