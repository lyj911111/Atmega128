
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char FND[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90}; // 0 1 2 3 4 5 6 7 8 9
int num[4]={0,0,0,0} ; // 자릿수 변수 초기값 0으로

int cnt=0;
int i=0;		
int state = 0;

ISR(INT2_vect)	// 1번 스위치 up
{
	state = 1;
}

ISR(INT3_vect)	// 2번 스위치 down
{
	state = 2;
}

ISR(INT6_vect)	// 3번 스위치 0000초기화
{
	state = 3;
}

ISR(INT7_vect)	// 4번 스위치 9999초기화
{
	state = 4;
}

ISR(TIMER1_OVF_vect)	// 5ms 인터럽트
{
	
	TCNT1 = 64286; // 오버플로우 초기값 리셋 

	
	
	PORTA = FND[num[i]]; // FND출력 단자
	
	if(i==3)	// Com단자를 위한 인덱스 값. 0번~>4번~>0번->...
		i=0;
	else
		i++;

	if(PORTC != 0x08)	// com단자 1,2,4,8 까지
		PORTC <<= 1;
	else
		PORTC = 0x01;	 // 첫번째 com단자 비트옮겨가며.

	if(state == 1)
	{
		if(++cnt == 20 )  			// 5ms * 20 = 100ms
		{
			cnt = 0;				//	초기화

			if(++num[0]==10)		//	자릿수별 카운트
			{						//	num[0]   1의자리 
				num[0]=0;			// 	num[1]  10의자리 
				if(++num[1]==10)	//	num[2] 100의자리
				{					//	num[3]1000의자리
					num[1]=0;
					if(++num[2]==10)
					{
						num[2]=0;
						if(++num[3]==10)
						{
							num[3]=0;
						}
					}
				}
			}
		}
	}
	if(state == 2)
	{
		if(++cnt == 20 )  			// 5ms * 20 = 100ms
		{
			cnt = 0;				//	초기화

			if(--num[0]==-1)		//	자릿수별 카운트
			{						//	num[0]   1의자리
				num[0]=9;			// 	num[1]  10의자리
				if(--num[1]==-1)	//	num[2] 100의자리
				{					//	num[3]1000의자리
					num[1]=9;
					if(--num[2]==-1)
					{
						num[2]=9;
						if(--num[3]==-1)
						{
							num[3]=9;
						}
					}
				}
			}
		}
	}
	if(state == 3)
	{
		for(int j=0 ; j<4; j++)
		{
			num[j]=0;
		}
	}
	if(state == 4)
	{
		for(int j=0 ; j<4; j++)
		{
			num[j]=9;
		}
	}

		
}

int main(void)
{
	
	//=================================
	// 입출력 기본세팅.
	
	DDRA = 0xFF;  // F포트 출력으로 FND 출력 포트설정.
	DDRC = 0x0F;  // C포트 0,1,2,3 출력 (Com단자 출력)

	DDRD = 0x00 ; // D포트 모두 입력설정 SW세팅
	DDRE = 0x00 ; // E포트 모두 입력설정

	PORTD = 0x0C ;  // 1개는 input pull-up
	PORTE = 0x80 ;  // 1개는 input pull-up

	//=================================//
	// 외부인터럽 세팅
	
	EICRA = 0xA0; 		// 2,3번 하강엣지 [1010 0000] 스위치 사용
	EICRB = 0xA0;		// 6,7번 하강엣지  
	EIMSK = 0b11001100; // 2,3,6,7번 스위치 마스크

	//=================================//
	// 타이머 세팅 1번카운트. 노말모드

	TCCR1B = 0x03; // 64분주만 사용.
	TCNT1 = 64286; // 5ms를 만들기 위해

	TIMSK |= 1 << TOIE1;	// 오버플로 인터럽트 사용

	PORTC = 0x08;			// Com포트 첫번째 초기값 지정

	sei();	 				// 인터럽트 사용
	
	while(1)
	{

	}
}
