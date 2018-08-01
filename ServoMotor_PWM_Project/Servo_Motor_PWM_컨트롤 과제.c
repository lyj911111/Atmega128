#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Fast PWM 모드에서 Servo 모터 통제, 20ms 주기 만들고나서.
// 20000 = (1/16)*256분주*1249
// 20000us = 20ms

int cnt = 0;

ISR(INT2_vect)  // 2번스위치 인터럽트 사용
{
						// 누를때마다 0.05ms씩 증가
	cnt++;

	if(OCR1B!=4999)		// 증가 후 180도 넘어가면 0도로
	OCR1B += 100;

	if(cnt == 25)
	{
		OCR1B = 999;
		cnt = 0;
	}
	
	
	
}

ISR(INT3_vect)	// 3번스위치 인터럽트 사용
{
					// 누를때마다 0.05ms씩 감소
	cnt--;

	if(OCR1B!=999)		// 감소 후 0도 넘어가면 180도로
	OCR1B -= 100;
	if(cnt == 0)
	{
		OCR1B = 4999;
		cnt = 25;
	}
	
	
}




int main(void)
{
	//=================================
	// 입출력 기본세팅.
	
	DDRB = 0xFF;  // B포트 출력으로

	DDRD = 0x00 ; // D포트 모두 입력설정 SW세팅
	DDRE = 0x00 ; // E포트 모두 입력설정

	PORTD = 0x0C ;  // 1개는 input pull-up
	PORTE = 0x80 ;  // 1개는 input pull-up

	

	//=================================//
	//타이머 세팅 (1로 세팅)  ** 비교매치 (Compare matching)

	OCR1A = 39999; // (1/16)us*8분주*(1+39999) = 20000us = 20ms

	TCCR1A = 0x23; // Fast PWM모드 [0010 0011] (아래것과 같이)
	TCCR1B = 0x1A; // 256분주비 사용 + Fast PWM  [0001 1010]

	//===================================//
	//외부 인터럽트 세팅 (스위치사용)

	EICRA = 0xA0; // 2번,3번 스위치 하강엣지로 사용
	EIMSK = 0b00001100; // 2번,3번 스위치 하강엣지
	SREG |= 0x80; // 인터럽트 사용

	OCR1B = 999 ; // 초기값 0도

	while(1)
	{
		
	}
}