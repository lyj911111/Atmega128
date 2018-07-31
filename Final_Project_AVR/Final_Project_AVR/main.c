/*
 * Final Project.c
 *
 * Created: 2018-07-14 오후 4:25:42
 * Author : Lee Won Jae
 * VER: 0.9.9
 *   조건.
 *   1초 기준 시간 타이머/카운트로 설정
 *   서보모터 타이머/카운트(PWM)활용
 *   ADC를 이용하여 3개 스위치 사용
 *   SW1 : 알람시간 확인, 1초 후 메인 표시 복귀
 *   SW2 : 알람 기능 ON/OFF, 토글 동작
 *   SW3 : 12시제 <-> 24시제 변경, 토글 동작
 *   SW4 : 외부 인터럽트 사용하여 알람동작 정지
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "USART.h"
#include "LCD.h"                                  
#define CMD_PLAY    0x0D
#define CMD_TF		0x09                                    
                                    
typedef unsigned char u_char;

void Clock(void);
void yr_mon_day();
void Disp_yr_mon_day();
void Rx_serial();
void current_display();
void servo_motor_move();
void alarm_mode();
void OnTime_alarm();
void SendCommand(unsigned char);


int sec = 0;
int min = 0;
int hour = 0;
int PMhour_12 = 0;
int hour_12 = 12;
int hour_24 = 12;
int alarm_hr = 0;
int alarm_min = 0;
int alarm_sec = 0;
int alarm_ampm = 0;


int i = 0;
int j = 0;
int k = 0;
int z = 0;
int x = 0;

int Day = 30;
int month = 1;
unsigned int yr = 2020;
int day[] = {32, 29, 32, 31, 32, 31, 32, 32, 31, 32, 31, 32};

int ad_val = 0;
int cnt = 0;
int cnt2 = 0;
int cnt3 = 0;

volatile char rx_complete = 0;
char state = 0;
char ampm = 0;      //  AM = 0, PM = 1, else = 3
char temp[9];
char temp2[12];
char al_temp[16];
char str[100];
char buffer[100];
char flag2 = 0 , flag3 = 0;  //  for analog Switch
char alarm_flag = 0;  // 0: alarm mode OFF ,  1: alarm mode ON
char alarm_open = 0;
char motor_on = 0;    // 1: motor move ,  0: motor stop
char alarm_infi = 0;
char sound_flag = 0;

unsigned char cmd = 0;
unsigned char param = 0;

char debug[100]; //************* Debuggig ****************


//  Receive USART Interrupt
ISR(USART0_RX_vect) 
{
    buffer[k] = UDR0;
    
    if(buffer[k++] == '\n' ) //  Get data until getting 'Enter key'
    {
        buffer[k-1] = '\0'; //  put '\0' on the place of '\n'
        k = 0;
        rx_complete = 1;
    }   
}

// 1000ms(=1s)
ISR(TIMER1_COMPA_vect)
{cli();
	
	if(state == 1 || alarm_open == 1  )      //  State of Alarm mode showing on Display
    {										 //  Alarm Mode ON and OFF
		if(++cnt2 == 3) //  holding
		{
			alarm_open = 0;
			cnt2 = 0;
			ADCSRA |= 0b00001000;
		}
    }
	else if(alarm_open == 2 )
	{
		if(++cnt2 == 3) //  holding
		{
			alarm_open = 0;
			cnt2 = 0;
			ADCSRA |= 0b00001000;
		}
	}


    if(flag3 == 1)      //  AM and PM shift flag
    { 
        if(++cnt == 2)  //  Prevent of Chattering
        {
            cnt = 0;
            flag3 = 0;
            ADCSRA |= 0b00001000;   //  ADC ON!
        }        
    }

    
    if(++sec==60)
    {
        sec=0;
        if(++min==60)
        {
            min=0;
            if(++hour_12 == 13 )      // (AM)Over at 12:59:59
            {
                hour_12 = 1;
                ampm = 1;
            }  
              
            if(++PMhour_12 == 12 )    // (PM)Over at 11:59:59
            {
                PMhour_12 = 0;
                ampm = 0;
            }     

            if(++hour_24 == 24)
            {
                hour_24 = 0;
                ++Day;
            }
        }
    }
    if(alarm_flag == 1)	//	if alarm is On, Motor is Toggle
    {
        if(motor_on == 1 && (sec%2==1))
        {
            if(++cnt3 == 1)
            {
                cnt3 = 0;
                motor_on = 0;
            }
        }
        else if(motor_on == 0 && (sec%2==0))
        {
            if(++cnt3 == 1)
            {
                cnt3 = 0;
                motor_on = 1;
            }
        }
    }

    if(sound_flag == 1)
    {
        SendCommand(CMD_PLAY);
        sound_flag = 0;
    }
    
sei();}

//External Interrupt for Switch
ISR(INT6_vect)
{cli();
    alarm_infi = 0;	//	Alarm OFF  
sei();}

ISR(ADC_vect)
{
   ad_val = (int)ADCL + ((int)ADCH << 8); //  Resistor for storing of result of converting
   
   if(ad_val < 50)  //  analog value : 0~2 (SW3)
   {
       flag3 = 1;
       if(ampm == 0 || ampm == 1)  //  12 -> 24
            ampm = 3;
       else if(ampm == 3 && ( hour_24 < 13 ))  // 24 -> 12
       {
           hour = hour_24;            // AM
           ampm = 0;
       }
       else if(ampm == 3 && ( hour_24 >= 13))
       {
           hour = hour_24;            // PM
           ampm = 1;
       }
       ADCSRA &= (0b11110111);  //   ADC OFF
   }
   else if(ad_val > 60 && ad_val < 120) //  analog value : 88~93 (SW2)
   {
       if(alarm_flag == 0)
       {
           alarm_flag = 1;      //  Alarm Mode ON!
		   alarm_open = 1;
       }
       else if(alarm_flag == 1 )
	   {
		   alarm_flag = 0;		//	Alarm Mode OFF!
		   alarm_open = 2;
	   }
              
       ADCSRA &= (0b11110111);  //   ADC OFF
   }
   else if(ad_val > 130 && ad_val < 700)  // analog value : 168~171 (SW1)
   {
       state = 1;
       ADCSRA &= (0b11110111);  //   ADC OFF
   }
   else    // analog value : 1023 
   {
       return;
   }
}
/*
void UART1_Putch(char ch)
{
    while(!(UCSR1A & 0x20));

    UDR1 = ch;
}
*/

int main(void)
{
   // Port set 
   DDRC = 0xFF;   //   set output for LCD

   
   // Using Analog Port by 'PE6' as a switch

   // Set External Interrupt Resistor for 'Switch'
   DDRE = (0 << 6);     // Set input SW
   PORTE = (1 << 6) ;    // Set input-Pull-up, PWM port
   EICRB = (1 << ISC61);// Set at Falling-edge
   EIMSK |= (1 << INT6); // Enable bit 6 interrupt 
   
   // Timer/Count 1 Resistor set (Comparative match, 'CTC mode')
   TCCR1A = 0x00;   //   OC1A output clear
   TCCR1B = 0x0D;   //   1024 prescaler, CTC mode                                   **디버깅후 0x0D로 수정하기**
   OCR1A = 15624;   //   (1/16)us * 1024 * (1+15624) = 1000000us = 1000ms = 1s      **디버깅후 15624로 수정하기**
   TIMSK |= ( 1 << OCIE1A);   //   Timer/Counter Output Comparative A match ENable
   
   // Timer/Count 3 Resistor set for fast 'PWM Mode'
   // OCR1A : 20ms, OCR1B : adjustable for PWM signal
   DDRE |= 0x08;    //   set PWM port 'PE4'
   TCCR3A = 0x82;   //   3A output Clear, Fast PWM
   TCCR3B = 0x1A;   //  Set Prescaler 8
   OCR3A = 1500;     //  initial "0 degree" of Motor     0 ~ 180 degree (range 999 ~ 4999)
   ICR3 = 39999;    // (1/16)us * 8 * (1+39999) = 20000us = 20ms
   
   // Set USART0 initializing
   USART0_init(9600);
   USART1_init(9600);
                                                    
   // A/D Converter Resistor set
   ADMUX = 0x40 ;   //  Use ADC0 Port
   ADCSRA = 0xEF ;          //  128 prescaler
   
   // Set LCD Default
   LCD_init();	
   LCD_String("2018-07-14");
   Commend(LINE2);
   LCD_String("AM 12:00:00");
   
   _delay_ms(200);
   SendCommand(CMD_TF);         //  Set MP3 Player

   
   sei();   //  Enable Interrupt resistor

   while (1) 
   {
            // 디버깅용 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        
       sprintf(debug,"%d , %d, ", sec, min ); //  ***********************debugging
       USART0_puts(debug);
       USART0_puts("\n\r");
        
            // 디버깅용 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        Rx_serial();
		yr_mon_day();
		current_display();
		alarm_mode();
        
	   if(alarm_infi == 1)
			servo_motor_move();
            
		OnTime_alarm();
   }
}


void Clock(void)
{
    Commend(LINE2);
    
    if(ampm == 0)
    {
        LCD_String("AM");
    }
    else if(ampm == 1)
    {
        LCD_String("PM");
    }
    else if(ampm == 3)
        LCD_String("  ");    // when set 24 hour system

    Commend(0xC3);
    
    if(ampm == 0)
    {
        hour = hour_12;
    }        
    else if(ampm == 1)
    {
        hour = PMhour_12;
    }
    else
        hour = hour_24;
    
    sprintf(temp, "%02d:%02d:%02d" ,hour,min,sec);
    LCD_String(temp);
}

void yr_mon_day()
{
    if( day[j] == Day )
    {
        Day = 1;
        month++;
        j++;
    }
    
    if( (yr % 4 == 0) || (yr % 100 == 0) || (yr % 400 == 0) )   // for leap year
    {
        day[1] = 30;
    }
    
    if(day[1] == 31)
    {
        Day = 1;
        month++;
        j++;
    }
    
    if( month == 13 )
    {
        month = 1;
        yr++;
    }    
}

void Disp_yr_mon_day()
{
    Commend(LINE1);
    sprintf(temp2, "%04d-%02d-%02d" ,yr,month,Day);
    LCD_String(temp2);
    Commend(0x8D);
    if(alarm_flag == 1)         //  Alarm ON and OFF Display
    {
        LCD_String(" ON");
    }
    else
        LCD_String("OFF");
}

void Rx_serial()
{
    if(rx_complete == 1)
    {
        if(buffer[0] == '$')
        {
            if(buffer[1] == 'C')    //  Calender
            {
                yr = 2000 + (buffer[2]-'0') * 10 + (buffer[3]-'0');
                month = (atoi(&buffer[4]) - atoi(&buffer[6])) / 100;
                Day = (atoi(&buffer[6]));
            }
            else if(buffer[1] == 'T')   //  Time
            {
                if(buffer[2] == 'A')
                {
                    ampm = 0;
                    hour_12 = (buffer[3]-'0') * 10 + (buffer[4]-'0');
                }                
                else if(buffer[2] == 'P')
                {
                    ampm = 1;
                    PMhour_12 = (buffer[3]-'0') * 10 + (buffer[4]-'0');
                }                    
                min = (atoi(&buffer[5]) - atoi(&buffer[7])) / 100;
                sec = (atoi(&buffer[7])); 
            }
            else if(buffer[1] == 'A')   //  Alarm
            {
                if(buffer[2] == 'A')
                {
                    alarm_ampm = 0;
                }
                else if(buffer[2] == 'P')
                {
                    alarm_ampm = 1;
                }
                alarm_hr = (buffer[3]-'0') * 10 + (buffer[4]-'0');
                alarm_min = (atoi(&buffer[5]) - atoi(&buffer[7])) / 100;
                alarm_sec = (atoi(&buffer[7])); 
            }
        }
        rx_complete = 0;
    }       
}

void alarm_disp()
{
    Commend(LINE1);
    LCD_String("Alarm Mode");
    
    Commend(LINE2);
    if(alarm_ampm == 0)
    {
        LCD_String("AM");
    }
    else if(alarm_ampm == 1)
    {
        LCD_String("PM");
    }
    Commend(0xC3);
    sprintf(al_temp, "%02d:%02d:%02d" ,alarm_hr,alarm_min,alarm_sec);
    LCD_String(al_temp);
}


void current_display()
{
    if(state == 0)                  // Routine Display.
    {
        Disp_yr_mon_day();
        Clock();
    }
    else if(state == 1)             //  Show current alarm Display for a second.
    {
        alarm_disp();
        
        if(cnt2 == 2)
        {
            cnt2 = 0;
            state = 0;
            ADCSRA |= 0b00001000;   //  ADC ON!*/
        }
    }          
}

void servo_motor_move()
{
    static int just_one=0;
	if(motor_on == 0 && just_one)		//	after 1 second,
	{
		OCR3A = 1500;		//	move to 0 degree
        sound_flag = 1;
        just_one=0;
	}
	else if(motor_on == 1 && !just_one)
	{
		OCR3A = 4999;       //  move to 180 degree
        sound_flag = 1;
        just_one=1;
	}
}

void alarm_mode()
{
	
	if((alarm_ampm == ampm && alarm_hr == hour && alarm_min == min && sec == 0) && (alarm_flag == 1))
	{
		alarm_infi = 1;
		sound_flag = 1;
	}
}

void OnTime_alarm()
{
	if(alarm_flag == 1)
	{
        if(min == 0 && hour == 1 && (sec > 0 && sec < 3))
        {
            servo_motor_move();
        }
        else if(min == 0 && hour != 1 && (sec > 0 && sec <= hour*2) )
        {
            servo_motor_move();      
        }
	}
}

void SendCommand(unsigned char cmd)
{
    unsigned int checksum = 0;
    USART1_putch(0x7E);
    USART1_putch(0xFF);
    USART1_putch(0x06);
    USART1_putch(cmd);
    USART1_putch(0x00);
    USART1_putch(0x00);

    if(cmd != CMD_TF){
        USART1_putch(0x00);
        checksum = 0 - (0xFF + 0x06 + cmd);
    }
    else{
        USART1_putch(0x02);
        checksum = 0 - (0xFF + 0x06 + cmd + 0x02);
    }
    
    USART1_putch((char)(checksum >> 8));
    USART1_putch(checksum & 0xFF);
    USART1_putch(0xEF);
}
