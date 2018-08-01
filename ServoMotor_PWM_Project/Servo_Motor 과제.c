#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int click = 0;
int count = 0;

ISR(INT2_vect) // increase degree
{
	if(++click == 26)
		click=0;

}

ISR(INT3_vect)  // decrease degree
{
	if(--click == -1)
		click=25;
}


ISR(TIMER0_COMP_vect)  // 인터럽트 주기 (0.1ms)
{
	
	count++;	// 1번 돌때마다 0.1ms씩 증가.
	
	if(count / 200)	 // 200번 돌면 다시 count는 0으로
	{
		count = 0;
		PORTB = 0xFF;
	}
	else if( count > click+5)
	{
		PORTB = 0x00;
	}
	

}

int main(void)
{
	//=================================
	// 입출력 기본세팅.
	
	DDRB = 0xFF;  // B포트 출력으로 (PB4 비트 포함되어있음)

	DDRD = 0x00 ; // D포트 모두 입력설정 SW사용
	DDRE = 0x00 ; // E포트 모두 입력설정

	PORTD = 0x0C ;  // 1개는 input pull-up
	PORTE = 0x80 ;  // 1개는 input pull-up

	//=================================//
	// 타이머 세팅
	// 100us 로 만들어주기

	TCCR0 = 0b00001010;     //   0 0 [10] 1[010] <=  값이 같아지면 0 ,CTC(비교매치), 8분주.
	TIMSK = 0x02;		 	// (비교매치) 타이머 인터럽트 마스크 bit2번을 1로
	OCR0 = 199;				// 비교매치를 사용 하여 인터럽트
	// (1/16)*8분주*(1+199) = 100um (=0.1ms) 인터럽트 주기.

	// ================================//
	// 인터럽트 세팅 2,3번 스위치 사용.
	
	EICRA = 0xA0;  		// 2,3번 레지스터 하강엣지로
	EIMSK = 0b00001100;	// 2,3번 인터럽트 사용 마스크
	SREG |= 0x80;

	while(1);
}
