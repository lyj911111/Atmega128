/*
 * test 6 14.c
 *
 * Created: 2018-06-14 오후 9:46:02
 * Author : lwj
 *
 *  스위치를 눌러서 LED를 이동시키는 예제, 또한 스위치의 채터링 방지. 
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

unsigned char led = 0x01;

int cnt = 0;


ISR(INT2_vect)
{
	if(led == 0x80)
	led = 0x01;
	else
	led <<= 1;

	TCCR0 = 0x07;  //  16.4ms 타이머를 살리고 외부
	EIMSK = 0;		// 인터럽트 하강엣지가 들어오면 안받는다.

}

ISR(INT3_vect)
{
	if(led == 0x01)
	led = 0x80;
	else
	led >>= 1;

	TCCR0 = 0x07;  //  16.4ms 타이머를 살리고 외부
	EIMSK = 0;		// 인터럽트 하강엣지가 들어오면 안받는다.

}

ISR(TIMER0_OVF_vect)
{
	if(++cnt == 100)  // 체터링을 얼마나 늘리는가 cnt.
	{
		cnt = 0;
		TCCR0 = 0x00;
		
		EIMSK =  0b00001100;
		//		EIFR |= 0b00001100;  // 원래 자동으로 인터럽트가 끝나면 클리어가 되는데, 해당 비트를 강제로 1로 클리어
	}
}

int main(void)
{
	
	//=================================
	// 입출력 기본세팅.
	
	DDRB = 0xFF;  // B포트 출력으로 LED출력

	DDRD = 0x00 ; // D포트 모두 입력설정 SW세팅
	DDRE = 0x00 ; // E포트 모두 입력설정

	PORTD = 0x0C ;  // 1개는 input pull-up
	PORTE = 0x80 ;  // 1개는 input pull-up

	//=================================//
	
	
	EICRA = 0b10100000; // 하강엣지
	EIMSK = 0b00001100;

	TCCR0 = 0x00; // 타이머 off 타이머는 분주비가 설정이 되지 않으면 카운트 세지 않음.
	TIMSK = 0x01;
	TCNT0 = 0;

	SREG |= 0x80;

	while(1)
	{
		PORTB = led;
	}

}
