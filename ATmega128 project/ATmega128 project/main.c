#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

// LCD 관련
void init_LCD(); //초기화 해주는 함수
void cmd(unsigned char command); // 명령을 입력해주는 함수

#define Function 0x30
#define Function_Set 0x38  //8bit mode, 2line, 5x8 dot
#define Display_ON 0x0c
#define Clear_Display 0x01
#define Entry_Mode_Set 0x06

#define Line_1 0x80 //이건 이해가 잘안됨
#define Line_2 0xc0

void input_LCD_char(unsigned char c);
void input_LCD_string(char* arr);
// timer/count이용 LCD input


void init_timer_input_LCD();
int count_LCD = 0;

int value;

//inpormation message
char EM [] = "Emergency stop";
//char OM [] = "please one more input number 1~9 at you want stop";
char IM [] = "please input number 1~9 at you want stop";
char IM_2[] ="please input no matter which except 1~9 that you want operate machine";

//interrunpt (external, time)관련
void init_Ex_interrupt();

#define Stop 0
#define Go 1
int status = Go;

//timer/counter 관련
void TC_Start();
void TC_Stop();

//ultra sonic sensor
void USE_USS(); // use ultra sonic sensor
double count = 0;

//USART
void init_USART1();
char c = 0;
void tx_1(char c);
void sort_tx1(char * arr);

//motor
void init_PWM();
void cc();
void motor_stop();
int main()
{
	init_USART1();
	DDRC = 0xff;
	DDRF = 0xff;
	DDRA = 0x0d;
	DDRB = 0xff;
	DDRD = 0xff;
	//int value;
	//char string[50];
	init_Ex_interrupt();
	init_LCD();
	init_PWM();
	//init_timer_input_LCD();

	while(1)
	{
		if(status == Stop )
		{
			PORTA &= 0x00;
			PORTF &= 0x00;
			PORTC &= 0x00;
			PORTB &= 0x00;
			PORTD &= 0x00;
		}
		
		else if(status == Go)
		{
			//cmd(Clear_Display);
			USE_USS(); //초음파 센서 사용
			value = (count* 16) / 58; // cm 환산 (소수점 버림)
			if(value > 400)
			value = 400;
			else if (value < 2)
			value = 0;
			
			init_timer_input_LCD();

			OCR1A = value * 0.61;
			OCR1B = value * 0.61;
			cc();

			//motor_stop();
			count = 0;
		}
	}
}
void cmd(unsigned char command)
{
	//RS = PC		0 PIN		0x01
	//RW = PC		1 PIN		0x02
	//E  = PC		2 PIN		0x04
	_delay_ms(1);
	PORTC = 0x00;
	_delay_us(0.04);
	PORTC = 0x04;
	_delay_us(0.2);
	PORTF = 0xff & command;
	_delay_us(0.02);
	PORTC = 0x00;
	_delay_ms(1);
	
}

void init_LCD()
{
	_delay_ms(1);
	_delay_ms(20);
	cmd(Function);
	_delay_ms(5);
	cmd(Function);
	_delay_us(101);
	cmd(Function);

	cmd(Function_Set);
	_delay_us(40);
	cmd(Display_ON);
	_delay_us(40);
	cmd(Clear_Display);
	_delay_ms(1.6);
	cmd(Entry_Mode_Set);
	_delay_ms(1);
}

void input_LCD_char(unsigned char c)
{
	//RS = PC		0 PIN		0x01
	//RW = PC		1 PIN		0x02
	//E  = PC		2 PIN		0x04
	_delay_ms(1);
	PORTC = 0x01;
	_delay_us(0.04);
	PORTC = 0x05; //E,RS HIGH
	_delay_us(0.2);
	PORTF = c;
	_delay_us(0.02);
	PORTC = 0x00;
	_delay_ms(1);
}

void input_LCD_string(char* arr)
{
	int i = 0 ;
	cmd(Line_1);
	while(1)
	{
		if (arr[i] == '\0')
		return;
		else
		{
			input_LCD_char(arr[i]);
			i++;
		}
		if (i > 16)
		break;
	}
	cmd(Line_2);
	while(1)
	{
		if (arr[i] == '\0')
		return;
		else
		{
			input_LCD_char(arr[i]);
			i++;
		}
	}
}

void init_Ex_interrupt()
{
	SREG |= 0x80; // interrupt enable
	EIMSK = 0x10; // 4pin interrupt enable
	EICRB |= 0x03; // 4pin rising edge
}

ISR (INT4_vect)
{

	_delay_ms(50);
	EIFR = 0x10;
	if ((PINE & 0x10) != 0x10)
	return;
	
	cmd (Clear_Display);
	if (status == Go)
	{
		cmd (Clear_Display);
		status = Stop;
		input_LCD_string(EM);
	}
	
	else if(status == Stop)
	{
		status = Go;
	}
}

void TC_Start()
{
	TIMSK = 0x01; // time interrupt ON
	TCCR0 = 0x01; // normal mode, NO prescaler
	TCNT0 = 0;  // TCNT0 초기화
}

ISR (TIMER0_OVF_vect)
{
	count++;
}

void TC_Stop()
{
	TIMSK = 0x00; // time interrupt OFF
	TCCR0 = 0x00; // NO clock source
	TCNT0 = 0;  // TCNT0 초기화
}

void USE_USS()
{
	PORTA = 0x01;
	_delay_us(10); // 10us 간 trigger ON
	PORTA = 0x00;
	while((PINA & 0x02)!= 0x02); // echo pin이 ON 이 될때까지 기다린다
	TC_Start();
	while((PINA & 0x02) == 0x02); //echo pin이 OFF 가 될때까지 기다린다
	TC_Stop();
}

void init_USART1()
{
	UCSR1A = 0x00; //인터럽트로 할거면 00으로 하는거같음
	UCSR1C = 0x06;
	UCSR1B = 0x98;//송,수신 모드
	UBRR1H = 0;
	UBRR1L = 8; //115.2k Baud Rate ,16MHz 설정 datasheet 참조
}

void tx_1(char c)
{
	while((UCSR1A & 0x20) != 0x20);
	
	UDR1 = c;
}

void sort_tx1(char * arr)
{
	int i = 0;
	while(1)
	{
		if (arr[i] == '\0')
		return;
		else
		tx_1(arr[i]);
		i++;
	}
}

ISR(USART1_RX_vect)
{
	if (UDR1 == '\0' || UDR1 == 0x0d || UDR1 == 0x0a || UDR1 == 00 )
	return;
	c = (UDR1);
	switch (c) {
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		cmd (Clear_Display);
		status = Stop;
		input_LCD_string(EM);
		sort_tx1(IM_2);
		tx_1('\n');
		break;
		default:
		cmd (Clear_Display);
		status = Go;
		sort_tx1(IM);
		tx_1('\n');
		break;
	}
}

void init_PWM() // Timer/counter 로 만들어내려함 timer/counter seting
{
	TCCR1A |= 0xa2; // OCN1A ,OCN1B matching clear ,bottom set fastPWM
	TCCR1B |= 0x0c; // fastPWM 256 prescaler
	TCNT1 = 0;
}
//각각의 좌우 PWM 조절하면 좌회전 우회전 가능하다.
void cc()
{
	PORTD = 0xa1;
	PORTB = 0x60;
	//PORTA = 0x04;
}

//void motor_stop()
//{
//PORTD = 0x00;
//PORTB = 0x00;
//PORTA = 0x00;
//}

void init_timer_input_LCD()
{
	TIMSK = 0x40; //TC2 interrupt on
	TCNT2 = 0;
	TCCR2 = 0x04; //prescaler 256 약 4ms 소요
	//counting 25회 약 0.1초  0.1초마다 LCD 업데이트 되도록 할 예정
}

ISR (TIMER2_OVF_vect)
{
	if (status == Stop) //클럭은 계속 증가하겠지만 max 4ms 차이 이므로 무시하기로함
	return;
	int num = 0 ;
	count_LCD++;
	TCNT2 = 0;
	
	if(count_LCD > 25)
	{
		init_LCD();
		char string[50];
		sprintf(string,"%d",value);
		count_LCD = 0;
		input_LCD_string(string);
		input_LCD_char('c');
		input_LCD_char('m');
		while(string[num] != '\0')
		string[num++] = 0;
	}
}