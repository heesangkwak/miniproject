///*
 //* Test_UART.c
 //*
 //* Created: 2020-08-09 오후 4:28:46
 //* Author : heesang
 //*/ 
//
#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include <avr/interrupt.h>

#define Function 0x30
#define Function_Set 0x38  //8bit mode, 2line, 5x8 dot
#define Display_ON 0x0C
#define Clear_Display 0x01
#define Entry_Mode_Set 0x06

#define NULL 0
#define Line_1 0x80
#define Line_2 0xc0

void init_USART();
void tx(char c);
char rx();
char input;
void LCD_initialize();
void CMD(unsigned char function);
void input_data(unsigned char c);
void input_string(unsigned char* arr);

void init_external_interrupt();
int main(void)
{
	DDRF = 0xff;
	DDRC = 0xff;	
	init_USART();
	LCD_initialize();
	init_external_interrupt();
	SREG = 0x80;
	
	while(1);
}

void init_USART()
{
	UCSR1A = 0x00;
	UCSR0B = 0x98; // 송수신 모드 설정 
	UCSR0C = 0x06; // disable parity bit , 8bit mode 
	//baud rate setting   
	UBRR0H = 0;
	UBRR0L = 103;
} 

ISR (USART0_RX_vect)
{
//	if ( UDR0 == 0x0d || UDR0 == 0x0a )
//	if (UDR0 == '\0' || UDR0 == 0x0d || UDR0 == 0x0a || UDR0 == 00 )
//		return;
	input = UDR0;
	//CMD (Clear_Display);
	input_data(input);
	tx(input);
}

void tx(char c)
{
	while((UCSR0A & 0x20) != 0x20); 
	UDR0 = c;
}

void CMD(unsigned char function)
{
	_delay_ms(1); //이거 없으면 안됨 왜인지는 잘모르겠음
	PORTC = 0x00; //RS, RW, E =0
	_delay_us(0.06); // 0.06us 정도로 잡아야하지만 지금 테스트
	PORTC = 0x04; //E set
	_delay_us(0.2); // 150ns = 0.15us

	PORTF = function & 0xff;
	_delay_us(0.1); // 0.09us;
	PORTC = 0x00;
	_delay_us(0.02); //0.01us;
}


void LCD_initialize()
{
	_delay_ms(30);
	CMD (Function);
	_delay_ms(10);
	CMD (Function);
	_delay_us(200);
	CMD (Function);

	_delay_ms(1);

	CMD (Function_Set);
	_delay_us(40);
	CMD (Display_ON);
	_delay_us(40);
	CMD (Clear_Display);
	_delay_us(1.53);
	CMD (Entry_Mode_Set);
	_delay_ms(1);
}

void input_data(unsigned char c)
{
	_delay_ms(1);
	PORTC = 0x01; //RS=1, RW, E =0
	_delay_us(0.06);
	PORTC = 0x05; //E set ,RS set
	_delay_us(0.2); // 150ns = 0.15us

	PORTF = c & 0xff;
	_delay_us(0.1);
	PORTC = 0x00; //E = 0 RW = 0 RS = 0
	_delay_us(1);
}

void input_string(unsigned char* arr)
{
	//CMD(Clear_Display);
	int i =0;
	while (1)
	{
		if (i > 15)
		break;

		if(arr[i] == NULL)
		return;

		input_data(arr[i]);
		i++;
	}
	CMD(Line_2);
	while(1)
	{
		if(arr[i] == NULL)
		return;

		input_data(arr[i]);
		i++;
	}
}

ISR(INT4_vect)
{
	_delay_ms(30);
	if ((PINE & 0x10) != 0x10)
		return;
	CMD(Clear_Display);
}

void init_external_interrupt()
{
	EICRB = 0x03; // int4 rising edge
	EIMSK = 0x10; // int4 interrupt active 	
}