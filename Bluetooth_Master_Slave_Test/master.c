/*
 *  
 *  Test for Master/Slave mode communication
 *
 *  Created: 2018-09-05
 *  Author : Jay Lee
 *
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "USART.h"

/*
 *
 *  BlueTooth Master mode
 *  Get the data from PC and toss to Atmega128 to forward the data to slave bluetooth
 *  
 *  Master MAC address: 7C010A7C0E8E
 *  Slave MAC address: 7C010A7C183B
 */

char getValue[30];
char buffer[30];
volatile int i = 0;
volatile char recive_complete = 0;

// Receive the data from Serial communication of PC
ISR(USART0_RX_vect)
{
    buffer[i] = UDR0;
    
    if(buffer[i++] == '\n' || buffer[i-1] == '\r')
    {
        buffer[i-1] = '\0';
        i = 0;
        recive_complete = 1;
    }

}

int main(void)
{

    // receive the data from PC
    USART0_init(9600);
    // transmit the date to Master bluetooth
    USART1_init(9600);
    
    sei();
    
    // Send setting value of Master bluetooth 
    USART1_puts("AT+CON7C010A7C183B\r\n");
    
    while (1) 
    {
        if(recive_complete == 1)
        {
            USART1_puts(buffer);
            USART1_puts("\r\n");
            recive_complete = 0;
        }
    }
}