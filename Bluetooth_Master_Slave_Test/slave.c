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
 *  BlueTooth slave mode
 *	Get the data from Master BlueTooth and turn on the LED 1,2,3,4,5
 *
 *  Master MAC address: 7C010A7C0E8E
 *  Slave MAC address: 7C010A7C183B
 **/

char getValue[30];
char buffer[30];
volatile int i = 0;
volatile char recive_complete = 0;

// Receive the signal by interrupt from slave bluetooth
ISR(USART1_RX_vect)
{
    buffer[i] = UDR1;
    
    if(buffer[i++] == '\n' || buffer[i-1] == '\r')
    {
        buffer[i-1] = '\0';
        i = 0;
        recive_complete = 1;
    }

}

int main(void)
{
    DDRB = 0xFF;    // LED Test 

    USART1_init(9600);
    
    sei();
    
    while (1) 
    {
        if(recive_complete == 1)
        {
            if(buffer[0]=='1')
            {
                PORTB = 0x01;
            }
            else if(buffer[0]=='2')
            {
                PORTB = 0x02;
            }
            else if(buffer[0]=='3')
            {
                PORTB = 0x04;
            }
            else if(buffer[0]=='4')
            {
                PORTB = 0x08;
            }
            else if(buffer[0]=='5')
            {
                PORTB = 0x10;
            }
            recive_complete = 0;
        }
    }
}