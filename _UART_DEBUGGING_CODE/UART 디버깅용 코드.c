/*
 * USART_TEST.c
 *
 * Created: 2018-07-18 오전 11:10:19
 * Author : lwj
 */ 

/*
 *  디버깅시 사용.
 *  알고싶은 변수를 시리얼 모니터에 출력
 *
 */
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void USART0_Tx(unsigned char data)
{
    while( !(UCSR0A & (1<<UDRE0)) );
    UDR0 = data;
}


unsigned char USART0_Rx(void)
{
    while( !(UCSR0A & (1<<RXC0)) );
    return UDR0;
}

void USART0_Tx_String(char * str)
{
    for(int i =0; str[i]; i++)
    {
        USART0_Tx(str[i]);
    }
}

int main(void)
{
    int cnt=0;
    
    UCSR0A = 0x00;
    UCSR0B = 0x18;
    UCSR0C = 0x06; 
    UBRR0L = 103;   //  Baud Rate : 9600
    
    char str[] = "Test using UART0 by Book";
    char count[100];
    
    while (1) 
    {
        cnt++;                  //  cnt대신 내가 알고싶은 변수를 넣음.
        count[0] = cnt;
        sprintf(count,"%d",cnt);	// 값의 변화를 확인할 수 있음.
        
        USART0_Tx_String(str);
        USART0_Tx_String("\r\n");
        USART0_Tx_String(count);
        USART0_Tx_String("\r\n");
        _delay_ms(1000);
    }
}

