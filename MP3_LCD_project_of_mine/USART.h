/*
 * USART.h
 *
 * Created: 모니터에서 받은 수신을 스피커로 송신.
 *  Author: lwj
 */ 
 #include <avr/io.h>
 #include <util/delay.h>
 #include <avr/interrupt.h>

#ifndef	 USART.h	// 만약 한곳에서 사용하고 있으면 다른곳에서 사용금지.
#define  USART.h



void UART0_Putch(char ch)		//	0번 UART 통신 (모니터에 display)
{
	while(!(UCSR0A & 0x20));

	UDR0 = ch;
}

char UART0_Getch(void)
{
	while(!(UCSR0A & 0x80));

	return UDR0;
}

void UART0_Puts(char str[])
{
	int i=0;

	while(str[i] != 0)
	UART0_Putch(str[i++]);
}

void UART1_Putch(char ch)		// 1번 UART 통신 (스피커에 신호)
{
	while(!(UCSR1A & 0x20));

	UDR1 = ch;
}

#endif

 