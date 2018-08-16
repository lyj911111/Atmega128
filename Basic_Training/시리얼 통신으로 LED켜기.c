
#include <avr/io.h>
#include <avr/interrupt.h>

ISR(USART0_RX_vect)	// 수신에 대한 서비스 루틴 (수신이 오면 걸림)
{
	PORTB = UDR0-'0';	//	키보드에서 입력된 값이 적힘(아스키 코드로) LED출력
}

int main(void)
{
	DDRB = 0xFF; 	// LED on 설정.

	UCSR0B = 0x98;  //  수신완료 인터럽트를 씀
	UCSR0C = 0x06;	//	
	UBRR0L = 103;	

	sei();

	while(1);

}
