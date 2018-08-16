/*
		���������� ���� �Ƴ��α� ��ȣ�� �޾Ƽ� LED ���
		�ø��� â���� �� ���� Ȯ����.
*/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void UART0_Putch(char ch)		//	0�� UART �۽� (����Ϳ� display)
{
	while(!(UCSR0A & 0x20));	// ���ο� �����͸� ���� �غ�, (UDR0�� ���� ��������� 1, ��������0)
					// Default���� 1�� �Ǿ�����.
	UDR0 = ch;			// ���� ������, ch���� UDR0 �� ����(�۽�). (����Ϳ� ���) 
}
/*
char UART0_Getch(void)			//	0�� UART ���� AVR�� ���� ����.
{
	while(!(UCSR0A & 0x80));	// ���Ź���(UDR0)�� ������ 0, ���� ���� ���ſϷᰡ �Ǹ� 1�� �ٲ�.
								// 7��bit (RXC0)�� 0->1�� �Ǹ� ���ͷ�Ʈ �߻�.
	return UDR0;			// UDR0�� �� ��ü�� �Լ����� Return��Ŵ.
}

*/
void UART0_Puts(char str[])		// 0�� UART �۽�, AVR�� ���ڿ�(String)�� �۽���.
{
	int i=0;

	while(str[i] != 0)		// NULL���ڰ� �ö����� ��� �迭�� ���� �־��.
		UART0_Putch(str[i++]);	// �迭�� ���� ��� ���� (NULL�� �ö�����)
}

int main(void)
{
	int ad_val;

	char temp[50];

	//=================================
	// ����� �⺻����.
	
	DDRB = 0xFF;  // LED�����Ʈ ����.
	//DDRC = 0x0F;  // C��Ʈ 0,1,2,3 ��� (Com���� ���)

	DDRD = 0x00 ; // D��Ʈ ��� �Է¼��� SW����
	//DDRE = 0x00 ; // E��Ʈ ��� �Է¼���

	PORTD = 0x0C ;  // 1���� input pull-up
	//PORTE = 0x80 ;  // 1���� input pull-up

	//=================================//
	// 

	UCSR0B = 0x08;  //  �۽ű� Enable
	UCSR0C = 0x06;	//	8bit�� ��� 
	UBRR0L = 103;

	//=================================//
	// 
	ADMUX = 0x40 ;	// [0100 0000] AVCC���� ���� �̿�.
	ADCSRA = 0x87 ; // [1000 0111] ADC enable, 128���ֺ� ���
	_delay_ms (5);


	while(1)
	{
		ADCSRA = 0xC7 ; // [1100 0111] ADC enable, ���Ϻ�ȯ��� ��Ʈ1�� AD��ȯ ����, 128���ֺ�
		// �����϶�� �˸� ��ȣ
		
		while( (ADCSRA & 0x40) == 0x40) ;	// 4����Ʈ AD��ȭ�� �Ϸ�Ǹ� �µ�

		ad_val = (int)ADCL + ( (int)ADCH << 8 ) ; // 16��Ʈ���� ��� ����.

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
