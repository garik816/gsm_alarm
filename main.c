/*
 * main.c
 *
 *  Created on: 18 ����. 2016
 *      Author: garik
 *
 *
 *
 *
 *
 *      to do list:
 *
 *      - ����������� �������� �������� ������� alarm()
 *      -
 *
 *
 *
 *
 *
 *
 */

#include <avr/io.h>
#include <util/delay.h> 	// ���������� ���������� ��������
#include <avr/interrupt.h>	// ���������� ���������� ����������
#include <stdlib.h>

char dial[]="ATD+380501025052;\r\n";






void init_UART(void)
{
	//	��������� �������� 9600
	UBRRH=0;	//	UBRR=f/(16*band)-1 f=8000000�� band=9600,
	UBRRL=51;	//	���������� ����������� ��������������� ����� ������

//			RXC			-	���������� �����
//			|TXC		-	���������� ��������
//			||UDRE 		-	���������� ������ ��� ��������
//			|||FE		-	������ �����
//			||||DOR		-	������ ������������ ������
//			|||||PE		-	������ ��������
//			||||||U2X	-	������� ��������
//			|||||||MPCM	-	����������������� �����
//			76543210
	UCSRA=0b00000000;

//			RXCIE		-	���������� ��� ����� ������
//			|TXCIE		-	���������� ��� ���������� ��������
//			||UDRIE		-	���������� ���������� ������ ��� ��������
//			|||RXEN		-	���������� �����
//			||||TXEN	-	���������� ��������
//			|||||UCSZ2	-	UCSZ0:2 ������ ����� ������
//			||||||RXB8	-	9 ��� �������� ������
//			|||||||TXB8	-	9 ��� ���������� ������
//			76543210
	UCSRB=0b00011000;	//	�������� ���� � �������� �� UART

//			URSEL		-	������ 1
//			|UMSEL		-	�����:1-���������� 0-�����������
//			||UPM1		-	UPM0:1 ��������
//			|||UPM0		-	UPM0:1 ��������
//			||||USBS	-	��� ����: 0-1, 1-2
//			|||||UCSZ1	-	UCSZ0:2 ������ ����� ������
//			||||||UCSZ0	-	UCSZ0:2 ������ ����� ������
//			|||||||UCPOL-	� ���������� ������ - ������������
//			76543210
	UCSRC=0b10000110;	//	8-������� �������
}

//	UART
void send_Uart(char c)//	�������� �����
{
	while(!(UCSRA&(1<<UDRE)))	//	���������������, ����� ������� ��������
	{}
	UDR = c;
}

void send_Uart_str(char *s)//	�������� ������
{
	while (*s != 0) send_Uart(*s++);
}

void send_int_Uart(unsigned int c)//	�������� ����� �� 0000 �� 9999
{
	unsigned char temp;
	c=c%10000;
	temp=c/100;
	send_Uart(temp/10+'0');
	send_Uart(temp%10+'0');
	temp=c%100;
	send_Uart(temp/10+'0');
	send_Uart(temp%10+'0');
}

unsigned char getch_Uart(void)//	��������� �����
{
	while(!(UCSRA&(1<<RXC)))	//	���������������, ����� ������� ��������
	{}
	return UDR;
}

int main(void){

	DDRC	= 0b00000001;
	PORTC	= 0b00101110;
	DDRD	= 0b10000000;
	PORTD	= 0b00000000;

	init_UART();	//������������� UART


	 /////*�������� ��� �������� ��������*//
	 TCCR0 = 0;
	 TCCR0 = (0<<WGM11)|(0<<WGM10);
	 TCNT0 = 100;
	 TIFR = (1<<TOV0);
	 TIMSK |= (1<<TOIE0);
	 TCCR0 |= (1<<CS02)|(0<<CS01)|(1<<CS00);
	 ///////////////////////////////////////


void loop() {



  }

sei();							//��������� ����������
while (1){						//����������� ����
	loop();
	}
}





ISR(TIMER0_OVF_vect)			//���������� ����������
{
   /*���������� �������� ��������*/
   TCNT0 = 100;

   	/*����� ������� �1*/
	if (bit_is_clear(PINC,PC1)) // ���� ������ ������ (���������� � gnd, ���������� �� vcc)
		{
		_delay_ms (150);
		PORTC |= (1<<0);		//alarm on
		send_Uart_str(dial);	//send AT_dial to BT
		}

//	/*����� ������� �2*/
//	else if (bit_is_clear(PINC,PC2)) // ���� ������ ������ (���������� � gnd, ���������� �� vcc)
//		{
//		_delay_ms (150);
//		PORTC |= (1<<0);		//alarm on
//		send_Uart_str(dial);	//send AT_dial to BT
//		}
//


//	/*����� vibro*/
	else if (bit_is_set(PINC,PC3)) // ���� ������ ������ (���������� � gnd, ���������� �� vcc)
		{
		_delay_ms (150);
		PORTC |= (1<<0);		//alarm on
		send_Uart_str(dial);	//send AT_dial to BT
		}



//	/*����� ������ ������*/
//	else if (bit_is_set(PINC,PC5)) // ���� ������ ������ (���������� � gnd, ���������� �� vcc)
//		{
//		_delay_ms (150);
//		PORTC &= (~(1<<0));		//alarm off
//		}
}
