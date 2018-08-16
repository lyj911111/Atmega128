/*
		가변저항을 통해 아날로그 신호를 받아서 LED 출력
		시리얼 창으로 그 값을 확인함.
*/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void UART0_Putch(char ch)		//	0번 UART 송신 (모니터에 display)
{
	while(!(UCSR0A & 0x20));	// 새로운 데이터를 받을 준비, (UDR0에 값이 비어있으면 1, 차있으면0)
					// Default값은 1로 되어있음.
	UDR0 = ch;			// 값이 있을때, ch값을 UDR0 에 넣음(송신). (모니터에 출력) 
}
/*
char UART0_Getch(void)			//	0번 UART 수신 AVR이 값을 받음.
{
	while(!(UCSR0A & 0x80));	// 수신버퍼(UDR0)가 없으면 0, 값이 들어와 수신완료가 되면 1로 바뀜.
								// 7번bit (RXC0)가 0->1이 되면 인터럽트 발생.
	return UDR0;			// UDR0의 값 자체를 함수에서 Return시킴.
}

*/
void UART0_Puts(char str[])		// 0번 UART 송신, AVR이 문자열(String)을 송신함.
{
	int i=0;

	while(str[i] != 0)		// NULL문자가 올때까지 계속 배열에 값을 넣어라.
		UART0_Putch(str[i++]);	// 배열의 값을 계속 증가 (NULL이 올때까지)
}

int main(void)
{
	int ad_val;

	char temp[50];

	//=================================
	// 입출력 기본세팅.
	
	DDRB = 0xFF;  // LED출력포트 설정.
	//DDRC = 0x0F;  // C포트 0,1,2,3 출력 (Com단자 출력)

	DDRD = 0x00 ; // D포트 모두 입력설정 SW세팅
	//DDRE = 0x00 ; // E포트 모두 입력설정

	PORTD = 0x0C ;  // 1개는 input pull-up
	//PORTE = 0x80 ;  // 1개는 input pull-up

	//=================================//
	// 

	UCSR0B = 0x08;  //  송신기 Enable
	UCSR0C = 0x06;	//	8bit를 사용 
	UBRR0L = 103;

	//=================================//
	// 
	ADMUX = 0x40 ;	// [0100 0000] AVCC단자 전압 이용.
	ADCSRA = 0x87 ; // [1000 0111] ADC enable, 128분주비 사용
	_delay_ms (5);


	while(1)
	{
		ADCSRA = 0xC7 ; // [1100 0111] ADC enable, 단일변환모드 비트1시 AD변환 시작, 128분주비
		// 시작하라고 알림 신호
		
		while( (ADCSRA & 0x40) == 0x40) ;	// 4번비트 AD변화가 완료되면 셋됨

		ad_val = (int)ADCL + ( (int)ADCH << 8 ) ; // 16비트값을 모두 저장.

		if(ad_val > 50 && ad_val < 300)
		{
			PORTB = 0b00000001;
		}
		else if(ad_val > 301 && ad_val < 700)
		{
			PORTB = 0b00010000;
		}
		else
			PORTB = 0b1000000;

		sprintf(temp, "\rADC value: %04d", ad_val);
		UART0_Puts(temp);
	}
}
