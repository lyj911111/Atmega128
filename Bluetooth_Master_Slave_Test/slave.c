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
 *  <MODE SETTING INFORMATION>
 *  AT+RENEW  : Default Factory setting
 *  AT+RESET  : Send this code after updating your code
 *  AT+NAME   : Set the names of Bluetooth for identifying roles
 *  AT+ROLE1  : Be a Master
 *  AT+ROLE0  : Be a Slave
 *  AT+IMME1  : Setting for preventing to connect automatically with both bluetooth before getting user's command
 *  
 *  
 *  <information of used Bluetooth HM-10>
 *  Master MAC address: 3CA308A10C6A
 *  Slave  MAC address: 3CA30896A09A
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