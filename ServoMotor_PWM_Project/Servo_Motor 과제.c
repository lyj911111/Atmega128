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


ISR(TIMER0_COMP_vect)  // ���ͷ�Ʈ �ֱ� (0.1ms)
{
	
	count++;	// 1�� �������� 0.1ms�� ����.
	
	if(count / 200)	 // 200�� ���� �ٽ� count�� 0����
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
	// ����� �⺻����.
	
	DDRB = 0xFF;  // B��Ʈ ������� (PB4 ��Ʈ ���ԵǾ�����)

	DDRD = 0x00 ; // D��Ʈ ��� �Է¼��� SW���
	DDRE = 0x00 ; // E��Ʈ ��� �Է¼���

	PORTD = 0x0C ;  // 1���� input pull-up
	PORTE = 0x80 ;  // 1���� input pull-up

	//=================================//
	// Ÿ�̸� ����
	// 100us �� ������ֱ�

	TCCR0 = 0b00001010;     //   0 0 [10] 1[010] <=  ���� �������� 0 ,CTC(�񱳸�ġ), 8����.
	TIMSK = 0x02;		 	// (�񱳸�ġ) Ÿ�̸� ���ͷ�Ʈ ����ũ bit2���� 1��
	OCR0 = 199;				// �񱳸�ġ�� ��� �Ͽ� ���ͷ�Ʈ
	// (1/16)*8����*(1+199) = 100um (=0.1ms) ���ͷ�Ʈ �ֱ�.

	// ================================//
	// ���ͷ�Ʈ ���� 2,3�� ����ġ ���.
	
	EICRA = 0xA0;  		// 2,3�� �������� �ϰ�������
	EIMSK = 0b00001100;	// 2,3�� ���ͷ�Ʈ ��� ����ũ
	SREG |= 0x80;

	while(1);
}
