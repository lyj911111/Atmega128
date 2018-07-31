

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _LCD_H_
#define _LCD_H_

typedef unsigned char u_char;

// LCD define
#define FUNCSET   0x28
#define ENTMODE   0x06
#define ALLCLR    0x01
#define DISPON    0x0C
#define LINE1     0x80
#define LINE2     0xC0
#define ENH      PORTC |= 0x04   //   + 0000 0100
#define ENL      PORTC &= 0xFB   //   & 1111 1011

void LCD_init(void);
void Commend(u_char byte);
void Data(u_char byte);
void LCD_String(char str[]);
void Busy();

void LCD_init(void)
{
    DDRC = 0xFF;	// Set Port C for LCD
    PORTC = 0x00;	// Initializing value
    _delay_ms(15);
    PORTC = 0x20;
    _delay_ms(5);
    PORTC = 0x20;
    _delay_us(100);
    PORTC = 0x20;
    // Set initializing so far

    Commend(0x20);
    Commend(FUNCSET);
    Commend(DISPON);
    Commend(ALLCLR);
    Commend(ENTMODE);
}

void Commend(u_char byte)	// Forward data from Upper bit to Lower bit.
{  
    Busy();
    
    // Upper 4 bit instruction.
    PORTC = 0x00;
    PORTC = PORTC | (byte & 0xF0);	// Set &-mask with upper 4 bit
    _delay_us(1);
    ENH;	// enable = 1   (Refer to Timing Diagram)
    _delay_us(1);
    ENL;	// enable = 0
    // ** If Enable resistor move from HIGH to LOW with Valid Data,then Operate the Display of LCD with data

    // Lower 4 bit instruction.
    PORTC = 0x00;
    PORTC |= (byte << 4);
    _delay_us(1);
    ENH;
    _delay_us(1);
    ENL;
}

void Data(u_char byte)
{
    Busy();
    PORTC = 0x01; // LCD clear. Place first Address of DDRAM
    PORTC |= (byte & 0xF0);	//  Forward Upper 4 bit
    _delay_us(1);
    ENH;
    _delay_us(1);
    ENL;

    PORTC = 0x01;
    PORTC |= (byte << 4);	//	Forward Lower 4 bit
    _delay_us(1);
    ENH;
    _delay_us(1);
    ENL;
}

void LCD_String(char str[])
{
    int i = 0;
    while(str[i]!=0)
    {
        Data(str[i++]);
    }
}

void Busy()
{
    _delay_ms(2);
}
#endif
