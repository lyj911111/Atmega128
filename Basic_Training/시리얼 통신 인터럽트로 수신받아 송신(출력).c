
#include <avr/io.h>
#include <avr/interrupt.h>

void UART_Puts(char str[]);
void UART_Putch(char ch);

char buffer[100];			// 버퍼를 저장
char index;					// 버퍼가 다 들어왔다고 알림 
volatile char rx_complete;	// 버퍼가 올때마다 index++, 다들어오면 rx를 1로 만듦.
							// volatile을 적는 이유: rx_complete의 0과 1의 변화를 유효하지 않다고 컴파일이 생각하는데 이를 무시

ISR(USART0_RX_vect)	// 수신에 대한 서비스 루틴 (수신이 오면 걸림)
{
	buffer[index] = UDR0;	//	키보드에서 받은 값을 index배열에 저장.

	if(buffer[index++] == '\r')	// 엔터가 나올때까지 배열에 값을 넣음.
	{
		buffer[index - 1] = 0;	// 맨끝에서 '\r'이 들어간 자리에 0을 넣음
		index = 0;				// index배열 초기화
		rx_complete = 1;		// if문을 위해 1로 바꿈
	}
}

int main(void)
{
	DDRB = 0xFF; 	// LED on 설정.

	UCSR0B = 0x98;  //  수신완료 인터럽트를 씀
	UCSR0C = 0x06;	//	
	UBRR0L = 103;	
	
	index = 0;			//변수 초기화
	rx_complete = 0;	

	sei();	// 인터럽트 사용

	while(1)
	{
		if(rx_complete == 1)	
		{
			UART_Puts(buffer);
			rx_complete = 0;
		}
	}
}

void UART_Puts(char str[])
{
	int i=0;

	while(str[i] != 0)	// [0xXX,0xXX,0xXX,...']... 아스키 코드 문장이 나오고 NULL 이 나올때까지
	{
		UART_Putch(str[i++]); // NULL이 나오기 전까지 아스키 코드를 계속 넣겠다
		
	}
	
}

void UART_Putch(char ch)	//이 자체로 데이터를 보낸는용 (가공x)
{
	while(!(UCSR0A & 0x20));
				// 비어있으면 넣어라
	UDR0 = ch;  // 송수신 겸용 데이터 레지스터 
}
