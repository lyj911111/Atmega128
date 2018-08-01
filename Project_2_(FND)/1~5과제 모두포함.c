
#define	F_CPU 16000000L

/*
	2018.06.01
	made by Won Jae, Lee
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define setbit(x,y)     (x) = (x) | (0x01 << (y)) // 0x01을 y번 1칸씩 왼쪽으로 밀어줌.
#define resetbit(x,y)	(x) = (x) & (0x00 << (y)) // 0x00을 y번 1칸씩 왼쪽으로 밀어줌.

//*************************************************************************************************//
//*************************************************************************************************//
#define num5;  // <===    현재 컴파일 하고자 하는 문제입력 !! num1, num2, num3, num4, num5 중 입력.
//*************************************************************************************************//
//*************************************************************************************************//

////////////////////////////////////////////////////////////////////// 문제1번.

#ifdef num1;
int main(void)
{
	DDRB = 0xFF;  //  B포트를 모두 출력으로.

	while (1)
	{
		setbit(PORTB,0); // B포트의 bit y번 을 1로 출력
		_delay_ms(1000);

		resetbit(PORTB,0); // B포트의 bit y번 을 0으로 출력
		_delay_ms(1000);
	}
}
#endif

////////////////////////////////////////////////////////////////////// 문제2번.

#ifdef num2;
int state = 0;  // 처음 상태

ISR(INT2_vect)  // 첫번째 스위치를 누를때 (오른쪽 순차 점멸)
{
	state = 1;
}

ISR(INT3_vect)  // 두번째 스위치를 누를때 (왼쪽 순차 점멸)
{
	state = 2;
}

ISR(INT6_vect)  // 세번째 스위치를 누를때 (방향 전환, 정지산태에서 동작 무시)
{
	if(state==1)      // state 1일때 2로 , state 2일때 1으로 바꿔줌
	state=2;
	else if(state==2)
	state=1;
}

ISR(INT7_vect)  // 네번째 스위치를 누를때 (정지)
{
	state = 4; 
}

int main(void)
{
	DDRB = 0xFF; 		 // B포트 모두 출력 설정. LED출력

	DDRD = 0x00;  		// D포트를 모두 입력으로, (스위치설정)
	PORTD = 0b00001000; // D 3번 bit만 INPUT PULLUP 으로 사용.

	DDRE = 0x00;  		// E포트를 모두 입력으로, (스위치설정)
	PORTE = 0b10000000; // E 7번 bit만 INPUT PULLUP 으로 사용.

	int led = 0x01;  // bit0 led값 초기화 0000 0001
	int led2 = 0x80; // bit7 led값 초기화 1000 0000

	EIMSK = 0b11001100; // 사용할 interrupt를 1로 마스크, 2,3,6,7을 1로 마스크 설정.
	EICRA = 0b10100000; // 0~3의 bit의 외부 interrupt 설정, D포트 2,3번을 하강엣지로 사용
	EICRB = 0b10100000; // 4~7의 bit의 외부 interrupt 설정. E포트 6,7번을 하강엣지로 사용
	SREG = SREG | 0x80; // Global Interrupt 1로, while문 위에설정.

	while(1)
	{
		if(state == 1)   // 상태1일때, 오른쪽으로
		{
			PORTB = led2; // 7번 bit on으로 초기화.
			if(led2 == 0x01) // 초기화 과정
			led2 = 0x80;
			else
			led2 >>= 1;  // 오른쪽으로 비트 이동
		}
		if(state == 2)  // 상태2일때, 왼쪽으로
		{
			PORTB = led;  // 1번 bit on으로 초기화.
			if(led == 0x80)  // 초기화 과정
			led = 0x01;
			else
			led <<= 1;  // 왼쪽으로 비트 이동.
		}
		
		if(state == 4)
		{
			PORTB = PORTB & 0xff; // &마스크를 씌우고 1만 남김
		}
		_delay_ms (100);
		
	}
}
#endif

////////////////////////////////////////////////////////////////////// 문제3번.

#ifdef num3
int main(void)
{
	unsigned char num[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90}; // 0 1 2 3 4 5 6 7 8 9
	
	DDRA = 0xff;  //  D포트를 모두 출력으로, FND를 위한 출력설정.
	DDRC = 0x0f;  //  C포트의 0,1,2,3  com단자를 출력설정.

	int cnt=0; int cnt10=0; int cnt100=0; int cnt1000=0;  //  초기화

	while(1)                // 초세기 카운터
	{
		if(++cnt == 10)
		{
			cnt = 0;
			if(++cnt10 == 10)
			{
				cnt10 = 0;
				if(++cnt100 == 10)
				{
					cnt100 = 0;
					if(++cnt1000 == 10)
					{
						cnt1000 = 0;
					}
				}
			}
		}
		
		for(int i=0; i<50 ; i++)  // 원하는 속도 입력. (20ms * x )
		{
			PORTC = 0x01 ; 		// 0번째 com포트 on
			PORTA = num[cnt] ; 	// 1의자리 FND
			_delay_ms(5);
			
			
			PORTC = PORTC << 1 ;  // 1번째 com포트 on
			PORTA = num[cnt10] ;  // 10의자리 FND
			_delay_ms(5);
			
			
			PORTC = PORTC << 1 ;	// 2번째 com포트 on
			PORTA = num[cnt100] ;	// 100의자리 FND
			_delay_ms(5);
			
			PORTC = PORTC << 1 ;	// 3번째 com포트 on
			PORTA = num[cnt1000] ;  // 1000의 자리 FND
			_delay_ms(5);

		}
		
	}
	
}

#endif

///////////////////////////////////////////////////////////////////////////// 문제4번.

#ifdef num4

int state = 0;  // 처음 상태

ISR(INT2_vect)  // 첫번째 스위치를 누를때 (카운트 업)
{
	state = 1;
}

ISR(INT3_vect)  // 두번째 스위치를 누를때 (카운트 다운)
{
	state = 2;
}

ISR(INT6_vect)  // 세번째 스위치를 누를때 (0000 초기화)
{
	state = 3;
}

ISR(INT7_vect)  // 네번째 스위치를 누를때 (9999 초기화)
{
	state = 4;
}

int main(void)
{
	unsigned char num[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90}; // 0 1 2 3 4 5 6 7 8 9
	
	DDRA = 0xff;  //  D포트를 모두 출력으로, FND를 위한 출력설정.
	DDRC = 0x0f;  //  C포트의 0,1,2,3  com단자를 출력설정.

	int cnt=0; int cnt10=0; int cnt100=0; int cnt1000=0;  		//  카운트 업 초기화
	int cnt_n=9; int cnt10_n=9; int cnt100_n=9; int cnt1000_n=9; // 카운트 다운 초기화

	DDRD = 0x00;  		// D포트를 모두 입력으로, (스위치설정)
	PORTD = 0b00001000; // D 3번 bit만 INPUT PULLUP 으로 사용.

	DDRE = 0x00;  		// E포트를 모두 입력으로, (스위치설정)
	PORTE = 0b10000000; // E 7번 bit만 INPUT PULLUP 으로 사용.
	
	EIMSK = 0b11001100; // 사용할 interrupt를 1로 마스크, 2,3,6,7을 1로 마스크 설정.
	EICRA = 0b10100000; // 0~3의 bit의 외부 interrupt 설정, D포트 2,3번을 하강엣지로 사용
	EICRB = 0b10100000; // 4~7의 bit의 외부 interrupt 설정. E포트 6,7번을 하강엣지로 사용
	SREG = SREG | 0x80; // Global Interrupt 1로, while문 위에설정.

	while(1)
	{
		// 스위치 1번을 눌렀을 때
		if(state == 1)   // 0부터 9999까지 상승.
		{
			if(++cnt == 10)
			{
				cnt = 0;
				if(++cnt10 == 10)
				{
					cnt10 = 0;
					if(++cnt100 == 10)
					{
						cnt100 = 0;
						if(++cnt1000 == 10)
						{
							cnt1000 = 0;
						}
					}
				}
			}

			PORTC = 0x01 ; 		// 0번째 com포트 on
			PORTA = num[cnt] ; 	// 1의자리 FND
			_delay_ms(5);
			
			
			PORTC = PORTC << 1 ;  // 1번째 com포트 on
			PORTA = num[cnt10] ;  // 10의자리 FND
			_delay_ms(5);
			
			
			PORTC = PORTC << 1 ;	// 2번째 com포트 on
			PORTA = num[cnt100] ;	// 100의자리 FND
			_delay_ms(5);
			
			PORTC = PORTC << 1 ;	// 3번째 com포트 on
			PORTA = num[cnt1000] ;  // 1000의 자리 FND
			_delay_ms(5);

		}
		
		// 스위치 2번을 눌렀을 때
		if(state == 2)
		{
			
			if(--cnt_n == -1)   //  9999 에서 0으로 카운트 다운
			{
				cnt_n = 9;
				if(--cnt10_n == -1)
				{
					cnt10_n = 9;
					if(--cnt100_n == -1)
					{
						cnt100_n = 9;
						if(--cnt1000_n == -1)
						{
							cnt1000_n = 9;
						}
					}
				}
			}
			PORTC = 0x01 ; 			// 0번째 com포트 on
			PORTA = num[cnt_n] ; 	// 1의자리 FND
			_delay_ms(5);
			
			
			PORTC = PORTC << 1 ;  // 1번째 com포트 on
			PORTA = num[cnt10_n] ;  // 10의자리 FND
			_delay_ms(5);
			
			
			PORTC = PORTC << 1 ;	// 2번째 com포트 on
			PORTA = num[cnt100_n] ;	// 100의자리 FND
			_delay_ms(5);
			
			PORTC = PORTC << 1 ;	// 3번째 com포트 on
			PORTA = num[cnt1000_n] ;  // 1000의 자리 FND
			_delay_ms(5);
		}

		// 스위치 3번 눌렀을때, 0000으로 초기화
		if(state == 3)
		{
			PORTC = 0x01 ; 		// 0번째 com포트 on
			PORTA = 0xc0 ; 		// 1의자리 FND
			_delay_ms(5);


			PORTC = PORTC << 1 ;  // 1번째 com포트 on
			PORTA = 0xc0 ;  	  // 10의자리 FND
			_delay_ms(5);


			PORTC = PORTC << 1 ;	// 2번째 com포트 on
			PORTA = 0xc0 ;			// 100의자리 FND
			_delay_ms(5);

			PORTC = PORTC << 1 ;	// 3번째 com포트 on
			PORTA = 0xc0 ;  		// 1000의 자리 FND
			_delay_ms(5);
		}

		// 스위치 4번 눌렀을때, 9999으로 초기화
		if(state == 4)
		{
			PORTC = 0x01 ; 			// 0번째 com포트 on
			PORTA = 0x90 ; 			// 1의자리 FND
			_delay_ms(5);
		
		
			PORTC = PORTC << 1 ;    // 1번째 com포트 on
			PORTA = 0x90 ;  		// 10의자리 FND
			_delay_ms(5);
		
		
			PORTC = PORTC << 1 ;	// 2번째 com포트 on
			PORTA = 0x90 ;			// 100의자리 FND
			_delay_ms(5);
		
			PORTC = PORTC << 1 ;	// 3번째 com포트 on
			PORTA = 0x90 ;  		// 1000의 자리 FND
			_delay_ms(5);
		}
	}
}

#endif 

///////////////////////////////////////////////////////////////////  문제5번.

#ifdef num5

int state = 0;  // 처음 상태
int blink=1; 	// 커서의 자릿수
int cnt=0; int cnt10=0; int cnt100=0; int cnt1000=0;  //  카운트 업 초기화

ISR(INT2_vect)  // 초기값 설정/ 첫번째 자리수 점멸.
{
	state = 1;
	blink = 1;  // 첫번째 스위치를 누르면 깜빡임 위치를 첫번째로 옮김.
}

ISR(INT3_vect)  // 자리수 변경 / 1->2->3->4 처음 자릿수 , 변경된 자릿수 점멸
{
	state = 2;
	blink++;		// 2번 스위치를 누를 때마다 자릿수 위치 증가.
	if(blink == 5)	// 1~4 자릿수  깜빡임 위치
		blink = 1;	 

}

ISR(INT6_vect)  	// 해당 자리수 카운트 업
{
	state = 3;
	if(blink == 1)  //  1번째 자리 카운트업
	{
		cnt++;
		if(cnt==10)
			cnt = 0;
	}
	else if(blink == 2)  // 2번째 자리 카운트업
	{
		cnt10++;
		if(cnt10==10)
			cnt10 = 0;
	}
	else if(blink == 3)  // 3번째 자리 카운트업
	{
		cnt100++;
		if(cnt100==10)
			cnt100 = 0;
	}
	else if(blink == 4)  // 4번째 자리 카운트업
	{
		cnt1000++;
		if(cnt1000==10)
			cnt1000 = 0;
	}
}

ISR(INT7_vect)  // 설정 완료 후 카운트 업, 0.1초 시간 지연.
{
	state = 4;
}

int main(void)
{
	unsigned char num[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90}; // 0 1 2 3 4 5 6 7 8 9
	
	DDRA = 0xff;  		//  D포트를 모두 출력으로, FND를 위한 출력설정.
	DDRC = 0x0f;  		//  C포트의 0,1,2,3  com단자를 출력설정.

	DDRD = 0x00;  		// D포트를 모두 입력으로, (스위치설정)
	PORTD = 0b00001000; // D 3번 bit만 INPUT PULLUP 으로 사용.

	DDRE = 0x00;  		// E포트를 모두 입력으로, (스위치설정)
	PORTE = 0b10000000; // E 7번 bit만 INPUT PULLUP 으로 사용.
	
	EIMSK = 0b11001100; // 사용할 interrupt를 1로 마스크, 2,3,6,7을 1로 마스크 설정.
	EICRA = 0b10100000; // 0~3의 bit의 외부 interrupt 설정, D포트 2,3번을 하강엣지로 사용
	EICRB = 0b10100000; // 4~7의 bit의 외부 interrupt 설정. E포트 6,7번을 하강엣지로 사용
	SREG = SREG | 0x80; // Global Interrupt 1로, while문 위에설정.

	while(1)
	{
		if(state==0)			// 초기값 0 0 0 0 으로 세팅. 단지 Display
		{
			PORTC = 0x01 ; 		// 0번째 com포트 on
			PORTA = 0xc0 ; 		// 1의자리 FND
			_delay_ms(5);


			PORTC = PORTC << 1 ;  // 1번째 com포트 on
			PORTA = 0xc0 ;  	  // 10의자리 FND
			_delay_ms(5);


			PORTC = PORTC << 1 ;	// 2번째 com포트 on
			PORTA = 0xc0 ;			// 100의자리 FND
			_delay_ms(5);

			PORTC = PORTC << 1 ;	// 3번째 com포트 on
			PORTA = 0xc0 ;  		// 1000의 자리 FND
			_delay_ms(5);
		}


		if (state == 1 )  				// 스위치 1번 누를때 첫번째 자릿수 깜빡 깜빡.
		{
			
			for(int i=0; i<25; i++)
			{
				PORTC = 0x01 ; 			// 0번째 com포트 on
				PORTA = num[cnt] ; 		// 1의자리 FND
				_delay_ms(5);
				PORTC = 0x00;


				PORTC = 0x02 ;     	 	 // 1번째 com포트 on
				PORTA = num[cnt10] ;  	 // 10의자리 FND
				_delay_ms(5);
				PORTC = 0x00;


				PORTC = 0x04 ;			// 2번째 com포트 on
				PORTA = num[cnt100] ;	// 100의자리 FND
				_delay_ms(5);
				PORTC = 0x00;

				PORTC = 0x08 ;			// 3번째 com포트 on
				PORTA = num[cnt1000] ;  // 1000의 자리 FND
				_delay_ms(5);
				PORTC = 0x00 ;
			}
			
			for(int i=0; i<25; i++)
			{
				PORTC = 0x00 ; 			// 0번째 com포트  OFF **
				PORTA = num[cnt] ; 		// 1의자리 FND
				_delay_ms(5);
				PORTC = 0x00;


				PORTC = 0x02 ;     		  // 1번째 com포트 on
				PORTA = num[cnt10] ;  	  // 10의자리 FND
				_delay_ms(5);
				PORTC = 0x00;


				PORTC = 0x04 ;			// 2번째 com포트 on
				PORTA = num[cnt100] ;	// 100의자리 FND
				_delay_ms(5);
				PORTC = 0x00;

				PORTC = 0x08 ;			// 3번째 com포트 on
				PORTA = num[cnt1000] ;  // 1000의 자리 FND
				_delay_ms(5);
				PORTC = 0x00 ;
			}
		}

		if(state == 2)  // 스위치 2번을 눌렀을때, 자리수 변경. 1->2->3->4->1 자리수로
		{
			if(blink == 1)   // 첫번째 커서 깜빡임.
			{
				for(int i=0; i<25; i++)
				{
					PORTC = 0x01 ; 		// 0번째 com포트 on
					PORTA = num[cnt] ; 	// 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x02 ;     	  // 1번째 com포트 on
					PORTA = num[cnt10] ;  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x04 ;			// 2번째 com포트 on
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x08 ;			// 3번째 com포트 on
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
				
				for(int i=0; i<25; i++)
				{
					PORTC = 0x00 ; 			// 0번째 com포트  OFF **
					PORTA = num[cnt] ; 		// 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x02 ;     		  // 1번째 com포트 on
					PORTA = num[cnt10] ;  	  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x04 ;			// 2번째 com포트 on
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x08 ;			// 3번째 com포트 on
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
			}
			else if(blink == 2)  // 두번째 커서 깜빡임.
			{
				for(int i=0; i<25; i++)
				{
					PORTC = 0x01 ; 		// 0번째 com포트 on
					PORTA = num[cnt] ; 	// 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x02 ;     	  // 1번째 com포트 on
					PORTA = num[cnt10] ;  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x04 ;			// 2번째 com포트 on
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x08 ;			// 3번째 com포트 on
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
				
				for(int i=0; i<25; i++)
				{
					PORTC = 0x01 ; 			// 0번째 com포트  
					PORTA = num[cnt] ; 		// 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x00 ;     		  // 1번째 com포트 OFF **
					PORTA = num[cnt10] ;  	  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x04 ;			// 2번째 com포트 on
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x08 ;			// 3번째 com포트 on
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
			}
			
			else if(blink == 3)  // 세번째 커서 깜빡임.
			{
				for(int i=0; i<25; i++)
				{
					PORTC = 0x01 ; 		// 0번째 com포트 on
					PORTA = num[cnt] ; 	// 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x02 ;     	  // 1번째 com포트 on
					PORTA = num[cnt10] ;  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x04 ;			// 2번째 com포트 on
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x08 ;			// 3번째 com포트 on
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
				
				for(int i=0; i<25; i++)
				{
					PORTC = 0x01 ; 			// 0번째 com포트 on
					PORTA = num[cnt] ; 		// 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x02 ;     		  // 1번째 com포트 on
					PORTA = num[cnt10] ;  	  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x00 ;			// 2번째 com포트 OFF **
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x08 ;			// 3번째 com포트 on
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
			}

			else if(blink == 4)  // 네번째 커서 깜빡임.
			{
				for(int i=0; i<25; i++)
				{
					PORTC = 0x01 ; 		// 0번째 com포트 on
					PORTA = num[cnt] ;  // 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x02 ;     	  // 1번째 com포트 on
					PORTA = num[cnt10] ;  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x04 ;			// 2번째 com포트 on
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x08 ;			// 3번째 com포트 on
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
				
				for(int i=0; i<25; i++)
				{
					PORTC = 0x01 ; 			// 0번째 com포트 on
					PORTA = num[cnt] ; 		// 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x02 ;     		  // 1번째 com포트 on
					PORTA = num[cnt10] ;  	  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x04 ;			// 2번째 com포트 on
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x00 ;			// 3번째 com포트 OFF **
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
			}
		}

		if (state == 3)  // 세번째 스위치를 누를때 마다, 해당 자릿수 카운트 업 0~>9 까지.
		{

			if(blink == 1)   // 첫번째 커서 깜빡임.
			{
				for(int i=0; i<25; i++)
				{
					PORTC = 0x01 ; 		// 0번째 com포트 on
					PORTA = num[cnt] ; 	// 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x02 ;     	  // 1번째 com포트 on
					PORTA = num[cnt10] ;  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x04 ;			// 2번째 com포트 on
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x08 ;			// 3번째 com포트 on
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
				
				for(int i=0; i<25; i++)
				{
					PORTC = 0x00 ; 			// 0번째 com포트  OFF **
					PORTA = num[cnt] ; 		// 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x02 ;     		  // 1번째 com포트 on
					PORTA = num[cnt10] ;  	  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x04 ;			// 2번째 com포트 on
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x08 ;			// 3번째 com포트 on
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
			}
			else if(blink == 2)  // 두번째 커서 깜빡임.
			{
				for(int i=0; i<25; i++)
				{
					PORTC = 0x01 ; 		// 0번째 com포트 on
					PORTA = num[cnt] ; 	// 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x02 ;     	  // 1번째 com포트 on
					PORTA = num[cnt10] ;  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x04 ;			// 2번째 com포트 on
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x08 ;			// 3번째 com포트 on
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
				
				for(int i=0; i<25; i++)
				{
					PORTC = 0x01 ; 			// 0번째 com포트
					PORTA = num[cnt] ; 		// 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x00 ;     		  // 1번째 com포트 OFF **
					PORTA = num[cnt10] ;  	  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x04 ;			// 2번째 com포트 on
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x08 ;			// 3번째 com포트 on
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
			}
			
			else if(blink == 3)  // 세번째 커서 깜빡임.
			{
				for(int i=0; i<25; i++)
				{
					PORTC = 0x01 ; 		// 0번째 com포트 on
					PORTA = num[cnt] ; 	// 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x02 ;     	  // 1번째 com포트 on
					PORTA = num[cnt10] ;  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x04 ;			// 2번째 com포트 on
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x08 ;			// 3번째 com포트 on
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
				
				for(int i=0; i<25; i++)
				{
					PORTC = 0x01 ; 			// 0번째 com포트 on
					PORTA = num[cnt] ; 		// 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x02 ;     		  // 1번째 com포트 on
					PORTA = num[cnt10] ;  	  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x00 ;			// 2번째 com포트 OFF **
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x08 ;			// 3번째 com포트 on
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
			}

			else if(blink == 4)  // 네번째 커서 깜빡임.
			{
				for(int i=0; i<25; i++)
				{
					PORTC = 0x01 ; 		// 0번째 com포트 on
					PORTA = num[cnt] ;  // 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x02 ;     	  // 1번째 com포트 on
					PORTA = num[cnt10] ;  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x04 ;			// 2번째 com포트 on
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x08 ;			// 3번째 com포트 on
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
				
				for(int i=0; i<25; i++)
				{
					PORTC = 0x01 ; 			// 0번째 com포트 on
					PORTA = num[cnt] ; 		// 1의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x02 ;     		  // 1번째 com포트 on
					PORTA = num[cnt10] ;  	  // 10의자리 FND
					_delay_ms(5);
					PORTC = 0x00;


					PORTC = 0x04 ;			// 2번째 com포트 on
					PORTA = num[cnt100] ;	// 100의자리 FND
					_delay_ms(5);
					PORTC = 0x00;

					PORTC = 0x00 ;			// 3번째 com포트 OFF **
					PORTA = num[cnt1000] ;  // 1000의 자리 FND
					_delay_ms(5);
					PORTC = 0x00 ;
				}
			}

		}

		if(state == 4)
		{
			if(++cnt == 10)    //  카운트 시작.
			{
				cnt = 0;
				if(++cnt10 == 10)
				{
					cnt10 = 0;
					if(++cnt100 == 10)
					{
						cnt100 = 0;
						if(++cnt1000 == 10)
						{
							cnt1000 = 0;
						}
					}
				}
			}

			for(int i=0; i<5; i++)  // 0.1초 시간 지연으로 카운터 시작.
			{
				PORTC = 0x01 ; 		// 0번째 com포트 on
				PORTA = num[cnt] ;  // 1의자리 FND
				_delay_ms(5);
				PORTC = 0x00;


				PORTC = 0x02 ;     	  // 1번째 com포트 on
				PORTA = num[cnt10] ;  // 10의자리 FND
				_delay_ms(5);
				PORTC = 0x00;


				PORTC = 0x04 ;			// 2번째 com포트 on
				PORTA = num[cnt100] ;	// 100의자리 FND
				_delay_ms(5);
				PORTC = 0x00;

				PORTC = 0x08 ;			// 3번째 com포트 on
				PORTA = num[cnt1000] ;  // 1000의 자리 FND
				_delay_ms(5);
				PORTC = 0x00 ;
			}

		}
		
	}
	
}
#endif