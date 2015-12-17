/*
 * IncFile1.h
 *
 * Created: 05.10.2015 16:16:10
 *  Author: Bartosz
 */ 

void UART_Init();
void UART_Transmit(unsigned char data);
void UART_SendString(char* text);
unsigned char  UART0_Recive();
unsigned char  UART1_Recive();

void UART_StopReceiver();

void UART_StartReceiver();