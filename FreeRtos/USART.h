/*
 * USART.h
 *
 * Created: 05-07-2019 17:31:31
 *  Author: abc
 */ 


#ifndef USART_H_
#define USART_H_

void uart_putc(char c);
void uart_puts(char *s);
void InitUart();
void printFloat(double number, uint8_t digits);
void uart_puti(int i);






#endif /* USART_H_ */