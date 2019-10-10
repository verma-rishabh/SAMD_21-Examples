#include <tgmath.h>
#include <stdlib.h>
#include "sam.h"
#include "USART.h"



void uart_putc(char c)
{
	while (SERCOM5->USART.INTFLAG.bit.DRE!=1);
	SERCOM5->USART.DATA.reg = c;
}

void uart_puts(char *s){
	while(*s !='\0'){
		uart_putc(*s);
		s++;
	}
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
	// while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
	GCLK->CLKCTRL.bit.GEN=0x00;
	//	 while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
	GCLK->CLKCTRL.bit.CLKEN=1;
	//	while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
	SERCOM5->USART.CTRLA.bit.MODE=1;
	SERCOM5->USART.CTRLA.bit.CMODE=0;
	SERCOM5->USART.CTRLA.bit.DORD=0x01;
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









void printFloat(double number, uint8_t digits)
{
	//int n = 0;

	
	// Handle negative numbers
	if (number < 0.0)
	{
		uart_putc('-');
		number = -number;
	}

	// Round correctly so that print(1.999, 2) prints as "2.00"
	double rounding = 0.5;
	for (uint8_t i=0; i<digits; ++i)
	rounding /= 10.0;

	number += rounding;

	// Extract the integer part of the number and print it
	int int_part = (int)number;
	double remainder = number - (double)int_part;
	uart_puti(int_part);

	// Print the decimal point, but only if there are digits beyond
	if (digits > 0) {
		uart_putc('.');
	}

	// Extract digits from the remainder one at a time
	while (digits-- > 0)
	{
		remainder *= 10.0;
		int toPrint = (int)(remainder);
		uart_puti(toPrint);
		remainder -= toPrint;
	}

	
}

void uart_puti(int i){
	char s[10];
	itoa(i,s,10);
	int j=0;
	while(s[j] !='\0'){
		uart_putc(s[j]);
		j++;
	}
}