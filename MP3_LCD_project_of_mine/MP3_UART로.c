/*
 * 06.20.MP3_speaker.c
 *
 * Created: 모니터에서 받은 수신을 스피커로 송신.
 * Author : lwj
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "USART.h"		// USART에 대한 함수 불러오기

#define CMD_NEXT	0x01		//	다음 곡
#define CMD_PRE		0x02		//  이전 곡
#define CMD_PLAY	0x0d		//	재생
#define CMD_PAUSE	0x0E		// 일시정지
#define CMD_STOP	0x16		// 정지
#define CMD_TF		0x09
#define CMD_VOL_UP		0x04	// 볼륨 증가
#define CMD_VOL_DOWN	0x05	// 볼륨 감소

//입력 커맨드 리스트 1번~7번까지.
char cmd_list[10] = {CMD_PLAY, CMD_STOP, CMD_PAUSE, CMD_NEXT, CMD_PRE ,CMD_VOL_UP, CMD_VOL_DOWN};

char cmd;	//	사용자의 입력을 받기 위한.

void SendCommand(unsigned char cmd);	// 키보드 -> 스피커로 송신 함수
void DisplayMenu(void);					// 모니터 display 함수

ISR(USART0_RX_vect)	// UART 0번 수신에 대한 서비스 루틴 (수신이 오면 걸림)
{
	DisplayMenu();			// 화면에 Display해줌 (모두 puts함수)
	cmd = UART0_Getch();	// 키보드 입력을 받음.
}

ISR(USART1_RX_vect)	// UART 1번 수신에 대한 서비스 루틴 (수신이 오면 걸림)
{
	SendCommand(cmd_list[cmd - '0' - 1]);	// 받은 명령을 송신함. 

}

int main(void)
{
	UCSR0B = 0x98;	//	[1001 1000] UART 0번 사용 (모니터와 인터럽트 통신)
	UCSR0C = 0x06;
	UBRR0L = 103;

	UCSR1B = 0x98;	//  [1001 1000] UART 1번 사용 (스피커와 인터럽트 통신)
	UCSR1C = 0x06;
	UBRR1L = 103;

	_delay_ms(200);

	SendCommand(CMD_TF);

	sei();	// 인터럽트 사용

	while(1){

	}
}

void SendCommand(unsigned char cmd)
{
	unsigned int checksum = 0;
	char temp[20];

	UART1_Putch(0x7E);
	UART1_Putch(0xFF);
	UART1_Putch(0x06);
	UART1_Putch(cmd);
	UART1_Putch(0x00);
	UART1_Putch(0x00);

	if(cmd != CMD_TF){
		UART1_Putch(0x00);
		checksum = 0 - (0xFF + 0x06 + cmd);
	}
	else{
		UART1_Putch(0x02);
		checksum = 0 - (0xFF + 0x06 + cmd + 0x02);
	}
	
	UART1_Putch((char)(checksum >> 8));	//	checksom 16bit 인데, [xxxx xxxx][xxxx xxxx]
									  //					상위비트 8개를 >> 하위비트로 옮김 (1byte(char)으로 전송을 위해)
	UART1_Putch(checksum & 0xFF);	// 그리고 나머지 checksom 하위비트 8개 마저 보냄. 이렇게 총 2byte를 보내는 게지.
	UART1_Putch(0xEF);			// 0xEF : [1110 1111]	// 데이터 보낸것을 끝냄을 알림.

}

void DisplayMenu(void)
{
	UART0_Puts("\r\n===========================");
	UART0_Puts("\r\n= 1 : Play Music          =");
	UART0_Puts("\r\n= 2 : Stop Music          =");
	UART0_Puts("\r\n= 3 : Pause Music         =");
	UART0_Puts("\r\n= 4 : Next Music          =");
	UART0_Puts("\r\n= 5 : Previous Music      =");
	UART0_Puts("\r\n= 6 : Volume up		      =");
	UART0_Puts("\r\n= 7 : Volume down	      =");
	UART0_Puts("\r\n===========================");
	UART0_Puts("\r\n\r\nCommand > ");
}
