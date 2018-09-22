/*
 * LCD_MP3_Professor.c
 *
 * Created: 2018-07-08 오후 1:44:03
 * Author : lwj
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define FUNCSET	0x28	//	function set
#define ENTMODE	0x06	//	Entry Mode set
#define ALLCLR	0x01	//	ALL CLEAR
#define DISPON	0x0C	//	Display on
#define CURHOME	0x02	//	Cursor home position
#define LINE2	0xC0	//	2nd line Mode
#define ENH		PORTC |= 0x04	//	Enable high	0000 0100	2번째 bit가 Enable
#define ENL		PORTC &= 0xFB	//	Enable Low	1111 1011

#define CMD_NEXT	0x01
#define CMD_PRE		0x02
#define CMD_PLAY	0x0D
#define CMD_PAUSE	0x0E
#define CMD_STOP	0x16
#define CMD_TF		0x09

typedef unsigned char u_char;

char menu[5][10] = {"1.PLAY","2.PAUSE","3.STOP","4.NEXT","5.PRE."};
char cmd_list[10] = {CMD_PLAY, CMD_PAUSE, CMD_STOP, CMD_NEXT, CMD_PRE};

int cur_pos = 0;	//	0: Upward [] , 1: Downward []
int menu_pos = 0;	//	0 에서 3 까지,
int old_pos = 9, old_menu = 9;


int sel_menu = 0;
char tx = 0;

ISR(INT4_vect)		//	위로 올려주기.
{
	if(cur_pos == 1)	
	{
		cur_pos = 0;
	}
	else
	{
		if(menu_pos != 0)
			menu_pos--;
	}
}

ISR(INT5_vect)
{
	sel_menu = menu_pos + cur_pos;
	tx = 1;
}

ISR(INT6_vect)		//	아래로 내려주기
{
	if(cur_pos == 0)
	{
		cur_pos = 1;
	}
	else
	{
		if(menu_pos != 3)
			menu_pos++;
	}
}

void PortInit (void)	//	LCD 포트 초기화
{
	DDRC = 0xFF;		//	C포트 LCD로 사용
	DDRE = 0x00;		//	SW입력으로 사용
	PORTE = 0xFF;		//	input pull-up
}

void EXIntInit(void)
{
	EICRB = 0b00101010;	//	외부인터럽트 4~6까지 Falling edge로 사용.
	EIMSK = 0b01110000;	//	Enable
}

void TimerInit (void)	//	타이머/카운트 초기화
{
	TCCR1B = 0x05;		//	Prescaler : 1024
	TIMSK  = 0x04;		//	Timer1 OverFlow, Interrupt Enable.
}

void UART1_Putch(char ch)
{
	while((UCSR1A & 0x20) != 0x20);
	UDR1 = ch;
}

void UART_Puts(char buf[], int len)
{
	int i;

	for(i=0 ; i<len; i++)
	{
		UART1_Putch(buf[i]);
	}
}

void ADCInit(void)		//	오디오 초기화???
{
	ADMUX = 0b01000000;
	ADCSRA = 0b10101111;
}

void UARTInit(void)
{
	UCSR1A = 0x00;
	UCSR1B = (TXEN1 << 1);	//	송신 Enable (송신 가능)
	UCSR1C = 0x06;

	UBRR1H = 0;
	UBRR1L = 103;
}

void LCD_Init(void)
{
	DDRC = 0xFF;
	PORTC = 0x00;
	_delay_ms(15);
	Command(0x20);
	_delay_ms(5);
	Command(0x20);
	_delay_us(100);
	Command(0x20);
	Command(FUNCSET);
	Command(DISPON);
	Command(ALLCLR);
	Command(ENTMODE);
}

void Command(u_char byte)
{
	busy();
	PORTC = 0x00;
	PORTC |= (byte & 0xF0);	// 상위 비트 4개 먼저 보냄.
	_delay_ms(1);
	ENH;
	_delay_ms(1);
	ENL;

	PORTC = 0x00;
	PORTC |= (byte << 4);	// 하위 비트 4개 나중에 보냄.
	_delay_ms(1);
	ENH;
	_delay_ms(1);
	ENL;
}

void Data(u_char byte)
{
	busy();
	PORTC = 0x01;			//	0x01 은 All Clear의 뜻.
	PORTC |= (byte & 0xF0);	// 상위 비트 4개 먼저 보냄.
	_delay_ms(1);
	ENH;
	_delay_ms(1);
	ENL;

	PORTC = 0x01;
	PORTC |= (byte << 4);	// 하위 비트 4개 나중에 보냄.
	_delay_ms(1);
	ENH;
	_delay_ms(1);
	ENL;
}

void busy(void)
{
	_delay_ms(2);
}

void LCD_String(char str[])
{
	char i=0;
	while(str[i] != 0)
		Data(str[i]++);
}

void DispMenu(void)
{
	Command(CURHOME);
	if(cur_pos == 0) LCD_String("[");	//	cur_pos 가 0일때 "[" 하고 아닐경우 " "
	else LCD_String(" ");
	LCD_String(menu[menu_pos]);
	if(cur_pos == 0) LCD_String("]");
	else LCD_String(" ");

	Command(LINE2);
	if(cur_pos == 1) LCD_String("[");
	else LCD_String(" ");
	LCD_String(menu[menu_pos+1]);
	if(cur_pos == 1) LCD_String("]");
	else LCD_String(" ");
}

void SendCommand (u_char cmd, unsigned int param)
{
	unsigned int checksum = 0;

	UART1_Putch(0x7E);
	UART1_Putch(0xFF);
	UART1_Putch(0x06);
	UART1_Putch(cmd);
	UART1_Putch(0x00);
	UART1_Putch((char)(param >> 8));
	UART1_Putch((char)(param & 0xFF));

	checksum = 0 - (0xFF + 0x06 + cmd + (char)(param >> 8) + (char)(param & 0xFF));

	UART1_Putch((char)(checksum >> 8));
	UART1_Putch(checksum & 0xFF);
	UART1_Putch(0xEF);
}

int main(void)
{
	PortInit();
	EXIntInit();
	//TimerInit();
	UARTInit();
	//ADCInit();
	LCD_Init();

	SREG |= 0x80;

	SendCommand(CMD_TF , 0x02);

	while (1)
	{
		if(old_pos != cur_pos  ||  old_menu != menu_pos)
		{
			DispMenu();
			old_pos = cur_pos;
			old_menu = menu_pos;
		}

		if ( tx == 1)
		{
			tx = 0;
			SendCommand(cmd_list[sel_menu], 0);
		}
	}
}