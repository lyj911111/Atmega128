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
 */



char getValue[30];
char buffer[30];
volatile int i = 0;
volatile char recive_complete = 0;

// Receive the data from Serial communication of PC
ISR(USART0_RX_vect)
{
    buffer[i] = UDR0;
    
    if(buffer[i++] == '\n' )
    {
        buffer[i-1] = '\0';
        i = 0;
        recive_complete = 1;
    }

}

int main(void)
{

    // Receive the data from PC
    USART0_init(9600);
    // Transmit the date to Master Bluetooth
    USART1_init(9600);
    
    sei();
    
    // Send Mac address for connecting with Slave Bluetooth
    USART1_puts("AT+CON3CA30896A09A\r\n");
    
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