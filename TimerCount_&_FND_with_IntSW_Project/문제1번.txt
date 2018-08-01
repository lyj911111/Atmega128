#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char led = 0x01;

int cnt=0;
int state =0 ;

ISR(INT2_vect) // left
{
	state = 1;
}

ISR(INT3_vect) // right
{
	state = 2;
}

ISR(INT6_vect) 
{
	if(state == 1)
	{
		state =2;
	}
	else if(state == 2)
	{
		state =1;
	}
}

ISR(INT7_vect)
{
	state = 3;
}

ISR(TIMER0_OVF_vect) // 0.1ms마다 작동.
{
	TCNT0 = 56; // 오버 플로 초기값 재 세팅

	cnt++;

	if(cnt==1000)
	{
		if(state==1)
		{
			if(led == 0x80)
				led = 0x01;
			else
				led <<= 1;
		}
		else if(state==2)
		{
			if(led == 0x01)
				led = 0x80;
			else
				led >>= 1;
		}
		else
		{
			PORTB ^= led;
		}

		cnt=0;
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
	// 외부인터럽 세팅
	
	EICRA = 0xA0; 		// 2,3번 하강엣지 [1010 0000] 스위치 사용
	EICRB = 0xA0;		// 6,7번 하강엣지 [1010 0000] 스위치 사용
	EIMSK = 0b11001100; 	// 2,3번 스위치 마스크

	// =================================//
	// 타이머 세팅

	TCCR0 = 0x02;	// 타이머 0.1ms로 맞추기 normal모드 , 8분주
	TCNT0 = 56;		// 0.1ms로 만듦.

	TIMSK = 0x01;	// 0번 타이머(8비트)를 사용하겠다.
	SREG |= 0x80;

	while(1)
	{

			PORTB = led;	// led on
	}
}