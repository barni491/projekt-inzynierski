/*
 * CFile1.c
 *
 * Created: 05.10.2015 16:24:45
 *  Author: Bartosz
 */ 
#define F_CPU 16000000UL
#define BAUD 9600
#include <avr/io.h>
#include <util/setbaud.h>




void UART_Init()
{
	/* Set baud rate */
	UBRR0H = UBRRH_VALUE;	
	UBRR0L = UBRRL_VALUE;
	/*Unable receiver and transmitter*/
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/*Set frame format : 8data, 1stop bit*/
	UCSR0C = (3<<UCSZ00);
	UCSR0A &= ~(_BV(U2X0));
	//UCSR0B |= (1<<TXCIE0);
	UCSR0B |= (1<<RXCIE0);
	
	UBRR1H = UBRRH_VALUE;
	UBRR1L = UBRRL_VALUE;
	/*Unable receiver and transmitter*/
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	/*Set frame format : 8data, 1stop bit*/
	UCSR1C = (3<<UCSZ00);
	UCSR1A &= ~(_BV(U2X0));
	//UCSR0B |= (1<<TXCIE0);
  UCSR1B |= (1<<RXCIE0);
	
	
}


void UART_StopReceiver(){
	
	UCSR0B &= ~(1<<RXEN0);
}

void UART_StartReceiver(){
	
	UCSR0B |= (1<<RXEN0);
}



void UART_Transmit(unsigned char data)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

uint8_t  UART0_Recive(){
	
	while(!(UCSR0A & (1<<RXC0)));
		return UDR0;
	
}
	
uint8_t  UART1_Recive(){
	
	while(!(UCSR1A & (1<<RXC1)));
	return UDR1;
	
}	


void UART_SendString(volatile unsigned char* text){
	while(*text){
		UART_Transmit(*text++);		
	}	
}