/*
 *  EEPROM 쓰기 및 읽기 테스트.
 *  
 *  Created: 2018-08-29 10:39:42 PM
 *  Author : Jay Lee
 *
 *  메모리 4k 이전까지 사용. (메모리 초과, 침범시 심각한 오류 방생)
 *  (ATmega 128 EEPROM 허용 메모리 4k까지, 
 *  
 *  Atmega 128 의 메모리 구조
 *  1. Flash 메모리 : [.text] (작성한 코드), [.bss, .data] (전역변수 지역변수 함수 등을 저장)
 *  2. SRAM 메모리  : 전원 off 되면 소멸, flash에서 [.bss , .data]를 받아서 저장. stack영역과 heap영역을 할당.
 *  3. EEPROM 메모리: 전원이 차단되어도 소멸되지 않음, 최대 4k byte까지 저장 (atmega 128기준), 그러나 다시 다운로드시 소멸.
 *  4. AVU (연산기 코어) : 중간에서 연산함.
 *  
 *  Atmel Studio 7의 simulator로 실행방법.
 *  -> ctrl + shift + P     tool에서 simulator설정.
 *  Alt + F5                디버깅 환경으로 진입
 *  Ctrl + shift +F5        디버깅 끝냄.
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "USART.h"

// EEPROM 에 데이터 쓰기. datasheet p23참고
void EEPROM_write(unsigned int uiAddress, unsigned int ucData)
{
    while(EECR & (1 << EEWE));   //p21 EECR(EEPROM Control Register), EEWE가 1로 설정되었는지 판단.(EEPROM Write Enable)
    
    EEAR = uiAddress;           //  EEPROM Address Register
    EEDR = ucData;              //  EEPROM Data Register
    EECR |= (1 << EEMWE);       //  EEPROM Master Write Enable
    EECR |= (1 << EEWE);        //  Start EEPROM write by setting EEWE
}

// EEPROM 에 데이터 읽기. 
unsigned char EEPROM_read(unsigned int uiAddress)
{
    while(EECR & (1 << EEWE)); // p24 이전 write가 되기 전까지 대기
    
    EEAR = uiAddress;           //  setup address register
    EECR |= (1 << EERE);        //  Enable이 되면, data를 받음.
    
    return EEDR;
}

int main()
{
    unsigned char i = 0;
    
    DDRB = 0xFF;    // LED Test
    PORTB = 0x00;   // initial LED off
    
    
// ※ 값을 EEPROM에 저장하고, LED를 테스트하고, 주석처리해도 값이 그데로 남아있음.
    //EEPROM_write(0x100, 0xAA);
    
    i = EEPROM_read(0x100);
    
    while(1)
    {
        PORTB = i;
    }
    
    
    return 0;
}