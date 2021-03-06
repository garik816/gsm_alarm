/*
 * main.c
 *
 *  Created on: 18 ����. 2016
 *      Author: g@rik
 *
 *		ATmega8
 *		���������� ����� 8 Mgz
 *
 *
 *      to do list:
 *
 *      - �������� ���������, ����� ������ � ������������ ��������		//����
 *      - �������� ��������� �� �����									//����
 *		- �������� ��������� ���������������� + �����					//��� ��������
 *      -
 *      -
 *      - ���������														//���� - �������� (22.03.16)
 *      -
 *      - ��� ������� ����� ����� ���������?							// ���� - ����� goto
 *      -
 *      - ������������ ����� � ��������� SMS
 *      -
 *      -
 *      -
 *      -
 *
 *
 *
 *
 */

#include <avr/io.h>
#include <util/delay.h>							// ���������� ���������� ��������
#include <avr/interrupt.h>						// ���������� ���������� ����������
#include <stdlib.h>




volatile char status;							//�������� ��������
#define PROTECT		0						//���� ��������� ������� ������
#define WARNING		1						//���� ��������� ������������
#define ALARM		2						//���� ���������� ������� �������� ����������
#define SECRET_SET	3						//���� ������� ��������� ������

volatile char sensor;
#define HERCON		0
#define VIBRO		1
#define RESET		2



#define Bit_is_set(val, bit)   ((val & (1<<(bit)))!=0)
#define Bit_is_clear(val, bit)   ((val & (1<<(bit)))==0)
#define Bit_set(val, bit)   (val) |= ((1<<(bit)))
#define Bit_reset(val, bit)   (val) &= (~(1<<(bit)))





int init_UART(void)
{
	                                                    // ��������� �������� 9600
	UBRRH=0;                                            // UBRR=f/(16*band)-1 f=8000000�� band=9600,
	UBRRL=51;                                           // ���������� ����������� ��������������� ����� ������

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
	UCSRB=0b00011000;	                                // �������� ���� � �������� �� UART

//			URSEL		-	������ 1
//			|UMSEL		-	�����:1-���������� 0-�����������
//			||UPM1		-	UPM0:1 ��������
//			|||UPM0		-	UPM0:1 ��������
//			||||USBS	-	��� ����: 0-1, 1-2
//			|||||UCSZ1	-	UCSZ0:2 ������ ����� ������
//			||||||UCSZ0	-	UCSZ0:2 ������ ����� ������
//			|||||||UCPOL-	� ���������� ������ - ������������
//			76543210

	UCSRC=0b10000110;				// 8-������� �������
	return(1);
}

void send_Uart(unsigned char c)				// �������� �����
{
	while(!(UCSRA&(1<<UDRE)))			// ���������������, ����� ������� ��������
	{}
	UDR = c;
}

void send_Uart_str(char *s)				// �������� ������
{
	while (*s != 0) send_Uart(*s++);
}

void send_int_Uart(unsigned int c)			// �������� ����� �� 0000 �� 9999
{
	unsigned char temp;
	c=c%10000;
	temp=c/100;
	send_Uart(temp/10+'0');
	send_Uart(temp%10+'0');
	temp=c%100;;
	send_Uart(temp/10+'0');
	send_Uart(temp%10+'0');
}

unsigned int gsm_dial_count=0;
unsigned int call=0;

int main(void)
{
	init_UART();                                        // ������������� UART
	_delay_ms(1000);                                    // �������� 1c

	send_Uart_str("start\n\r");		//debug

	//////////////////////////////////////////////////
	for(int i=0;i<150;i++)                           // ���� ���� ~102 ������� ����� �����������
		{                                           //
			_delay_ms(1000);                        //
			i++;                                    //
		}                                           //
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////////
	start:                                              //		�������� ��� goto
	sensor=0;                                           //		����� ����������
	status=0;											//
	gsm_dial_count=1;									//
	call=0;												//
	cli();                                              //
	//////////////////////////////////////////////////////

	/////  ������������� � ���������� ������     /////////
	DDRC	= 0b00010001;                               //
	PORTC	= 0b00101110;                               //
	DDRD	= 0b10000000;                               //
	PORTD	= 0b00000000;                               //
	//////////////////////////////////////////////////////



//  send_Uart_str("secret - ok\n\r");		//debug
//  send_Uart_str("warning_hercon - ok\n\r");	//debug
//  send_Uart_str("warning_vibro - ok\n\r");	//debug




		////*�������� ��� �������� ��������*////////////// ��� 8 ��, TCCR1B = (1<<CS12)|(0<<CS11)|(0<<CS10), TCNT1 = 34336 - ���������� = 1000��
		TCCR1B = (1<<CS12)|(0<<CS11)|(0<<CS10);         // ����������� �������� (������� ����������)
		TIMSK |= (1<<TOIE1);                            // ��������� ���������� �� ������������ �������
		TCNT1 = 34336;                                  // ���������� ��������� �������� TCNT1
		//////////////////////////////////////////////////

		/////*�������� ��� �������� ��������*///////////// ��� 8 ��, TCCR0 = (1<<CS12)|(0<<CS11)|(1<<CS10), TCNT0 = 60 - ���������� = 25��
		TCCR0 = (1<<CS12)|(0<<CS11)|(1<<CS10);          // ����������� �������� (������� ����������)
		TIMSK |= (1<<TOIE0);                            // ��������� ���������� �� ������������ �������
		TCNT0 = 60;                                     // ���������� ��������� �������� TCNT0
		//////////////////////////////////////////////////



		send_Uart_str("armed\n\r");		//debug


//		/////////   �������� SMS    //////////////////////
//		send_Uart_str("AT+CMGS=\"+380631020961\"\n\r"); // �����
//		_delay_ms(25);
//		send_Uart_str("alarm_ARMED");                   // �����
//		_delay_ms(25);
//		send_Uart(26);                                  //
//		_delay_ms(25);
//		send_Uart_str("\n\r");                          //
//		_delay_ms(25);
//		//////////////////////////////////////////////////




		sei();                                          // ��������� ����������

		for(;;)                                         // ����������� ����
		{
//			if(Bit_is_set(sensor, RESET))           // ���� ��������� ������� ����� - ��������� � ������ ���������
//			{
//				send_Uart_str("reset\n\r");     //debug
//				send_int_Uart(sensor);          //debug
//				send_int_Uart(status);          //debug
//				send_Uart_str("\n\r");          //debug
//				goto start;
//			}

//			send_Uart_str("count: ");
//			send_int_Uart(gsm_dial_count);
//			send_Uart_str(", ");
//			send_Uart_str("status: ");
//			send_int_Uart(status);
//			send_Uart_str(", ");
//			send_Uart_str("sensor: ");
//			send_int_Uart(sensor);
//			send_Uart_str("\n\r");

			if(call==1)
			{
				send_Uart_str("ATD+380679755948;\n\r");
			}


			//////////////////////////////////////////////////
			for(int i=0;i<20;i++)                           // ���� ���� ~10 ������ ����� �����������
				{                                           //
					_delay_ms(1000);                        //
					i++;                                    //
				}                                           //
			//////////////////////////////////////////////////

			if((gsm_dial_count>60)&&(sensor==0))
			{
				goto start;
			}


//		���� �������� �������� SMS





		}
}



ISR(TIMER1_OVF_vect)                             // ������ � ������� (������ 1000��)
{
	TCNT1 = 34336;                               // ���������� �������� ��������
//  PORTC ^= (1<<4);                             //debug

//		//////   ����������� ������������ (�������) //////
//		if(Bit_is_set(sensor, RESET))                   //
//			{                                      	    //
//				Bit_reset(sensor, RESET);               //
//				Bit_set(status, SECRET_SET);			//
//				call=0;									//
//				send_Uart_str("secret\n\r");            // debug
//			}                                           //
//		//////////======================================//

		/////////        ����� ������� �� �����     //////
		if(Bit_is_set(sensor, HERCON))                  //
			{                                           //
				Bit_reset(sensor, HERCON);              //
				Bit_set(status, ALARM);                 //
//				send_Uart_str("hercon\n\r");            // debug
			}                                           //
		//////======================================//////

		///////  ����� ������������ ("0" - ��������) /////
		if(Bit_is_set(sensor, VIBRO))                   //
			{                                           //
				Bit_reset(sensor, VIBRO);               //
				Bit_set(status, WARNING);               //
//				send_Uart_str("vibro\n\r");             // debug
			}                                           //
		//////======================================//////




		/////////////          ��������� ��������      ///////////
//		if(Bit_is_set(status, SECRET_SET))                      //
//			{                                                   //
//				send_Uart_str("SECRET\n\r");                    // debug
//				status = 0;                                     //
//				PORTC &= (~(1<<0));                             //
//			}                                                   //
//                                                                //
		if(Bit_is_set(status, ALARM))                           //
			{                                                   //
//				PORTC ^= (1<<4);                                // debug
//				send_Uart_str("ALARM\n\r");                     // debug
				PORTC |= (1<<0);
				PORTC |= (1<<4);								//
				call=1;											//
//				send_Uart_str("ATD+380501025052;\n\r");			// ������
																//
					if(gsm_dial_count==101)						//
						{										//
							gsm_dial_count=1;					//
						}										//
					gsm_dial_count++;							//
			}                                         			//
																//
		if(Bit_is_set(status, WARNING))                         //
				{                                               //
		//		PORTC ^= (1<<0);                    			// debug
//				send_Uart_str("WARNING\n\r");					// debug
				PORTC |= (1<<0);
				PORTC |= (1<<4);								//
				call=1;											//
//				send_Uart_str("ATD+380501025052;\n\r");			// ������
																//
					if(gsm_dial_count==101)						//
						{										//
							gsm_dial_count=1;					//
						}										//
					gsm_dial_count++;							//
				}	                       			            //
		//////////////////////////////////////////////////////////





}

ISR(TIMER0_OVF_vect)                                    // ����� �������� (������ 25��)
{

	TCNT0 = 60;                                         // ���������� �������� ��������
//	PORTC ^= (1<<0);                                    // debug


//		//////   ����������� ������������ (�������)   ////
//		if (bit_is_set(PINC,PC5))                       //
//				{                                       //
//						Bit_set(sensor, RESET);         //
//				}                                       //
//		//////////////////////////////////////////////////



		//////          ����� ������� �� �����      //////
        if (bit_is_clear(PINC,PC3))                     //
                {                                       //
        				Bit_set(sensor, HERCON);        // ��������� ����
                }                                       //
		//////////////////////////////////////////////////



		//////    ����� ������������ ("0" - ��������) ////
		if (bit_is_clear(PINC,PC1))                     //
				{                                       //
						Bit_set(sensor, VIBRO);         // ��������� ����
				}                                       //
		//////////////////////////////////////////////////



}
