/*
 * 7_segment.c
 *
 * Created: 2018-07-13 오후 3:46:46
 * Author : Lee Won Jae
 *
 * FND자리수는 5ms의 시간을 갖도록
 * 카운트-업 시간도 약 100ms의 시간을 갖도록
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#define FND1	 PORTD |= 0x10
#define FND2	 PORTD |= 0x20
#define FND3	 PORTD |= 0x40
#define FND4	 PORTD |= 0x80
#define FND_PORT PORTA

// FND 0~9, Led-off
char FND[11] = {
				0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001,
				0b10010010, 0b10000010, 0b11111000, 0b10000000, 0b10010000,
				0b11111111
				};
int count = 0;	//	0~9999
int cnt = 0;	//	For Counting speed
int cnt2 = 0;	//	For Blink term

char temp[5];
char blink = 0;	// 0: off , 1:on Mode set
char select = 0; // 0: 자리선택 없음, 1,2,3,4: FND자리
char cnt_up = 0; // 0: count up OFF , 1: count up ON

void InitFND(void);
void CountUp(void);
void Sel_FND(void);

// SW1 카운트 초기값 설정, 첫번째 자리 수 점멸
ISR(INT0_vect)
{cli();
	blink = 1;	// Blink on
	select = 1; // 첫번째 자릿수 점멸
	cnt_up = 0;	// Count OFF
	
sei();}

// SW2 FND자릿수 변경 점멸
ISR(INT1_vect)
{cli();
	blink = 1;	//	Blink on
	cnt_up = 0;	//	Count OFF
	if(++select == 5 )	//	FND자릿수 변경.
		select = 1;
	
sei();}

// SW3 해당 자릿수 카운트 업
ISR(INT2_vect)
{cli();
	blink = 1;
	cnt_up = 0;
	
	if( temp[4-select] == '9' )
		temp[4-select] = '0';
	else
		temp[4-select]++;

	count = atoi(temp);	//	Array to int, 문자열을 int형으로 변경.
	
sei();}

// SW4 설정 완료 후 카운트업 (100ms)
ISR(INT3_vect)
{cli();
	blink = 0;	// Blink OFF
	cnt_up = 1;	// count up ON
	select = 0;
	
sei();}

// 5ms 타이머/카운터 마다 FND shift
ISR(TIMER1_COMPA_vect)
{
	cnt++;	//	Using for Count up Speed
	cnt2++;	//	Using for Blink
	
	FND1;
	FND_PORT = FND[temp[3]-'0'];
	FND2;
	FND_PORT = FND[temp[2]-'0'];
	FND3;
	FND_PORT = FND[temp[1]-'0'];
	FND4;
	FND_PORT = FND[temp[0]-'0'];
}

int main(void)
{
	// Port Resistor set
	DDRA = 0xFF;	// Set FND port output
	DDRD = 0xF0;	// Set FND-ComPort bit no. 4,5,6,7 , Set SW bit no. 0,1,2,3 
	PORTD |= 0x0F;	// Set SW input Pull-up
	
	// External Interrupt Resistor set (0,1,2,3 bit)
	EICRA =	0xAA;	// bit no. 0,1,2,3 Operate at falling edge
	EIMSK = 0xF0;	// bit no. 0,1,2,3 to SW
	
	// Timer/Counter Resistor set, 1A: 5ms
	TCCR1A = 0x80;	//	1A 비교매치 출력 클리어
	TCCR1B = 0x0B;	//	CTC모드, 64분주
	OCR1A = 1249;	//	(1/16)us*64*(1+1249) = 5000us = 5ms
	TIMSK = (OCIE1A << 1);
	
	InitFND();	//	FND 0으로 초기화
	
	sei();
    while (1) 
    {
		sprintf(temp,"%04d\0",count);	//	save count num to array
		Sel_FND();
		
		if(cnt_up == 1 && cnt == 20 )	//	5ms * 20ms = 100ms
		{
			CountUp();
			cnt = 0;
		}
    }
}

void InitFND(void)
{
	FND1;
	FND_PORT = FND[0];
	FND2;
	FND_PORT = FND[0];
	FND3;
	FND_PORT = FND[0];
	FND4;
	FND_PORT = FND[0];
}

void CountUp(void)
{
	if(++count == 10000)
		count = 0;
}

void Sel_FND(void)
{
	
	if(select == 1)	
	{
		FND1;
		if( cnt2 <= 100)	//	5ms * 100 = 500ms (0.5s)
		{
			FND_PORT = FND[10];
			cnt2 = 0;
		}
	}
	else if(select == 2)
	{
		FND2;
		if( cnt2 <= 100)	//	5ms * 100 = 500ms (0.5s)
		{
			FND_PORT = FND[10];
			cnt2 = 0;
		}
	}
	else if(select == 3)
	{
		FND3;
		if( cnt2 <= 100)	//	5ms * 100 = 500ms (0.5s)
		{
			FND_PORT = FND[10];
			cnt2 = 0;
		}
	}
	else if(select == 4)
	{
		FND4;
		if( cnt2 <= 100)	//	5ms * 100 = 500ms (0.5s)
		{
			FND_PORT = FND[10];
			cnt2 = 0;
		}
	}

}