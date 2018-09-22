/*
MP3 Player control Project
author : Won Jae Lee
date   : 18.06.27
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*   [AVR128 Port connection with LCD]
RS : PC0   DB4: PC4
R/W: PC1   DB5: PC5
E  : PC2   DB6: PC6
DB7: PC7
*/
//   Switch Usage: [ PE7: up , PD3: selection  , PD2: down ]

typedef unsigned char u_char;

#define DISP_CLEAR   0x01   // display initialize
#define CUR_HOME   0x02   // Cursor home
#define ENTRYMODE   0x06   // EntryMode (Move the cursor right direction)
#define ON_OFF_CON   0x0C   // [D,C,B]=>[1,0,0] Display Words, no cursor, no blink
#define FUNC_SET   0x28   // Function set [0010 1000] [DL:0, N:1, F:0,] DL=0, 4비트전송설정, N=1,5x7dot 표시 라인 2번째

#define LINE2      0xC0   // Move to 2nd line
#define ENH   PORTC |= 0x04    // Enable pin PC2 to high (bit no.2) +[0000 0100]
#define ENL   PORTC &= 0xFB   // Enable pin PC2 to LOW            &[1111 1011]
#define CMD_NEXT   0x01      //   다음 곡
#define CMD_PRE      0x02      //  이전 곡
#define CMD_PLAY   0x0d      //   재생
#define CMD_PAUSE   0x0E      // 일시정지
#define CMD_STOP   0x16      // 정지
#define CMD_TF      0x09
#define CMD_VOL_UP     0x04   // 볼륨 증가
#define CMD_VOL_DOWN   0x05   // 볼륨 감소

void init_LCD(void);         // To initialize the LCD
void disp_data(u_char data);   // To forward the command data for displaying of LCD
void command_data(u_char data);   // To forward the data to AVR for setting of LCD
void LCD_string(u_char str[]);   // To input the string data
void busy(void);               // wait 2ms
void UART1_Putch(char ch);
void SendCommand(unsigned char cmd);

int cnt = 0;
int j = 1;
int change_brk = 0x80;   // first line origin coordinate of LCD
int  cmd = 0;
u_char mp3_list[8][10] = { "[       ]",
"1.Play ",
"2.Pause",
"3.Stop ",
"4.Next ",
"5.Prev ",
"6.VolUp",
"7.VolDn" };

char cmd_list[7] = { CMD_PLAY, CMD_PAUSE, CMD_STOP, CMD_NEXT, CMD_PRE, CMD_VOL_UP, CMD_VOL_DOWN };


ISR(INT2_vect)   // Down SW
{
   
   if (change_brk == 0x80)
   {
      SendCommand(cmd_list[j - 1]);
   }
   else if (change_brk == 0xC0)
   {
      SendCommand(cmd_list[j]);
   }

   EIMSK = 0x00;   // prevent the chattering of switch
   TCCR0 = 0x07;
   TCNT0 = 0;
   TIMSK = 0x01;   
}

ISR(INT5_vect)   // Selection SW
{   command_data(DISP_CLEAR);

   if (change_brk == 0xC0)
   {
      change_brk = 0x80;
   }
   else if (change_brk == 0x80)
   {
      if (--j == 0)
      {
         j = 1;
      }
   }

   command_data(change_brk);      // put the cursor at home position
   LCD_string(mp3_list[0]);      // bracket with blank

   command_data(0x81);         // move to right direction from original position
   LCD_string(mp3_list[j]);     // 1st line update
   command_data(0xC1);         // move to 2번째줄 2번째
   LCD_string(mp3_list[j + 1]);   // 2nd line update


   EIMSK = 0x00;   // prevent the chattering of switch
   TCCR0 = 0x07;
   TCNT0 = 0;
   TIMSK = 0x01;

   
}

ISR(INT7_vect)   // Up SW
{

      command_data(DISP_CLEAR);

   if (change_brk == 0x80)      //    괄호가 위에 있으면, 아래로 내리고,
   {
      change_brk = 0xC0;
   }
   else if (change_brk == 0xC0)   //   괄호가 아래에 있으면 화면을 아래로 바꿔라
   {
      if (++j == 7)
      {
         j = 6;
      }
   }

   command_data(change_brk);
   LCD_string(mp3_list[0]);      // bracket with blank

   command_data(0x81);         // move to right direction from original position
   LCD_string(mp3_list[j]);     // 1st line update
   command_data(0xC1);         // move to 2번째줄 2번째
   LCD_string(mp3_list[j + 1]);   // 2nd line update


   EIMSK = 0x00;   //   pause the External interrupt (SW) for preventing Chattering
   TCCR0 = 0x07;   //   Reset prescaler 
   TCNT0 = 0;      //   initial value of Timer/Counter
   TIMSK = 0x01;   //   Enable Timer/Counter of Overflow mode
}


ISR(TIMER0_OVF_vect)   // prevent the chattering of switching button using Timer/counter
{
   if (++cnt == 40)      //   extend the time because of fast speed, 16.4ms * 40 = 656ms
   {
      cnt = 0;
      EIMSK = 0xA4;   // 10001100  reactivate the External interrupt resistor
      TCCR0 = 0x00;   //   inactivate the Timer/counter interrupt resistor
   }
}


int main(void)
{
   DDRC = 0xFF;      //   Set all of C port as output to LCD
   init_LCD();   //   initializing LCD
   // Set USART
   UCSR1B = 0x18;   //  [1001 1000] UART 1번 사용 (스피커와 인터럽트 통신)
   UCSR1C = 0x06;
   UBRR1L = 103;

   // set basic Resistor
   DDRD = 0b00001000;   // Set all of D port as input for SW except the PD3 (this port is used for MP3 output port)
   DDRE = 0x00;         // Set all of E port as input for SW
   PORTD = 0b00001100;  // Set the SW as input pull-up with bit no.3
   PORTE = 0b10100000;  // Set the SW as input pull-up with bit no.7

   // Set External Interrupt resistor for SW
   EICRA = 0x20;      // Use falling edge of SW no. 2 
   EICRB = 0x88;      // Use falling edge of SW no. 5, 7
   EIMSK = 0xA4;   // Use external interrupt no.2,5,7

                                          // Set Timer/Counter Resister
   TCCR0 = 0x07;   // Set Normal Mode(Overflow) and ratio of prescaler as 1024, count Speed as 16.4ms
   TCNT0 = 0;      // Set initial value of Overflow mode
   TIMSK = 0x00;   // Set Overflow Interrupt enable


   // Set LCD
   LCD_string(mp3_list[0]);      // making default bracket [ ]
   command_data(0x81);         // move to right direction from original position
   LCD_string(mp3_list[1]);
   command_data(0xC1);         // 두번째줄 2번째
   LCD_string(mp3_list[2]);

   SREG |=0x80;   // activate the interrupt
   while (1)
   {
   }
}

// Initializing the LCD
void init_LCD(void)
{
   PORTC = 0x00;   // Clear of data
   _delay_ms(15);   //   wait for 30ms until the power approaching 4.5V
   PORTC = 0x20;   // Initializing
   _delay_ms(5);
   PORTC = 0x20;
   _delay_us(100);
   PORTC = 0x20;

   command_data(FUNC_SET);
   command_data(ON_OFF_CON);
   command_data(DISP_CLEAR);
   command_data(ENTRYMODE);
}

void disp_data(u_char data)   //   forward the data about display to AVR per 4 bits
{
   busy();

   PORTC = 0x01;               //   display data clear
   PORTC = PORTC | (data & 0xF0);   // forward High 4 bits of data
   _delay_us(1);
   ENH;
   _delay_us(1);
   ENL;

   PORTC = 0x01;               // display data clear
   PORTC |= (data << 4);         // forward Low 4 bits of data
   _delay_us(1);
   ENH;
   _delay_us(1);
   ENL;
}

void command_data(u_char data)   //   forward the bundle of data to AVR per 4 bits
{
   busy();

   PORTC = 0x00;               // All clear of data
   PORTC = PORTC | (data & 0xF0);   // forward High 4 bits of data
   _delay_us(1);
   ENH;
   _delay_us(1);
   ENL;

   PORTC = 0x00;               // All clear of data
   PORTC |= (data << 4);         // forward Low 4 bits of data   
   _delay_us(1);
   ENH;
   _delay_us(1);
   ENL;
}

void LCD_string(u_char str[])
{
   int i = 0;
   while (str[i] != 0)
   {
      disp_data(str[i++]);
   }
}

void busy(void)
{
   _delay_ms(2);
}
///////////////////////

void UART1_Putch(char ch)
{
   while (!(UCSR1A & 0x20));

   UDR1 = ch;
}

void SendCommand(unsigned char cmd)
{
   unsigned int checksum = 0;

   UART1_Putch(0x7E);
   UART1_Putch(0xFF);
   UART1_Putch(0x06);
   UART1_Putch(cmd);
   UART1_Putch(0x00);
   UART1_Putch(0x00);

   if (cmd != CMD_TF) {
      UART1_Putch(0x00);
      checksum = 0 - (0xFF + 0x06 + cmd);
   }
   else {
      UART1_Putch(0x02);
      checksum = 0 - (0xFF + 0x06 + cmd + 0x02);
   }

   UART1_Putch((char)(checksum >> 8));
   UART1_Putch(checksum & 0xFF);
   UART1_Putch(0xEF);

}