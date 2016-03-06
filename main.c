/*
 * main.c
 *
 *  Created on: 18 февр. 2016
 *      Author: garik
 *
 *
 *
 *
 *
 *      to do list:
 *
 *      - обработчики датчиков вызывают функцию alarm()
 *      -
 *
 *
 *
 *
 *
 *
 */

#include <avr/io.h>
#include <util/delay.h> 	// подключаем библиотеку задержки
#include <avr/interrupt.h>	// подключаем библиотеку прерываний
#include <stdlib.h>

char dial[]="ATD+380501025052;\r\n";






void init_UART(void)
{
	//	Установка скорости 9600
	UBRRH=0;	//	UBRR=f/(16*band)-1 f=8000000Гц band=9600,
	UBRRL=51;	//	нормальный асинхронный двунаправленный режим работы

//			RXC			-	завершение приёма
//			|TXC		-	завершение передачи
//			||UDRE 		-	отсутствие данных для отправки
//			|||FE		-	ошибка кадра
//			||||DOR		-	ошибка переполнение буфера
//			|||||PE		-	ошибка чётности
//			||||||U2X	-	Двойная скорость
//			|||||||MPCM	-	Многопроцессорный режим
//			76543210
	UCSRA=0b00000000;

//			RXCIE		-	прерывание при приёме данных
//			|TXCIE		-	прерывание при завершение передачи
//			||UDRIE		-	прерывание отсутствие данных для отправки
//			|||RXEN		-	разрешение приёма
//			||||TXEN	-	разрешение передачи
//			|||||UCSZ2	-	UCSZ0:2 размер кадра данных
//			||||||RXB8	-	9 бит принятых данных
//			|||||||TXB8	-	9 бит переданных данных
//			76543210
	UCSRB=0b00011000;	//	разрешен приём и передача по UART

//			URSEL		-	всегда 1
//			|UMSEL		-	режим:1-синхронный 0-асинхронный
//			||UPM1		-	UPM0:1 чётность
//			|||UPM0		-	UPM0:1 чётность
//			||||USBS	-	топ биты: 0-1, 1-2
//			|||||UCSZ1	-	UCSZ0:2 размер кадра данных
//			||||||UCSZ0	-	UCSZ0:2 размер кадра данных
//			|||||||UCPOL-	в синхронном режиме - тактирование
//			76543210
	UCSRC=0b10000110;	//	8-битовая посылка
}

//	UART
void send_Uart(char c)//	Отправка байта
{
	while(!(UCSRA&(1<<UDRE)))	//	Устанавливается, когда регистр свободен
	{}
	UDR = c;
}

void send_Uart_str(char *s)//	Отправка строки
{
	while (*s != 0) send_Uart(*s++);
}

void send_int_Uart(unsigned int c)//	Отправка числа от 0000 до 9999
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

unsigned char getch_Uart(void)//	Получение байта
{
	while(!(UCSRA&(1<<RXC)))	//	Устанавливается, когда регистр свободен
	{}
	return UDR;
}

int main(void){

	DDRC	= 0b00000001;
	PORTC	= 0b00101110;
	DDRD	= 0b10000000;
	PORTD	= 0b00000000;

	init_UART();	//инициализация UART


	 /////*значение для счетного регистра*//
	 TCCR0 = 0;
	 TCCR0 = (0<<WGM11)|(0<<WGM10);
	 TCNT0 = 100;
	 TIFR = (1<<TOV0);
	 TIMSK |= (1<<TOIE0);
	 TCCR0 |= (1<<CS02)|(0<<CS01)|(1<<CS00);
	 ///////////////////////////////////////


void loop() {



  }

sei();							//разрешаем прерывания
while (1){						//бесконечный цикл
	loop();
	}
}





ISR(TIMER0_OVF_vect)			//обработчик прерывания
{
   /*перезапись счетного регистра*/
   TCNT0 = 100;

   	/*опрос геркона №1*/
	if (bit_is_clear(PINC,PC1)) // если кнопка нажата (подключена к gnd, замыкается на vcc)
		{
		_delay_ms (150);
		PORTC |= (1<<0);		//alarm on
		send_Uart_str(dial);	//send AT_dial to BT
		}

//	/*опрос геркона №2*/
//	else if (bit_is_clear(PINC,PC2)) // если кнопка нажата (подключена к gnd, замыкается на vcc)
//		{
//		_delay_ms (150);
//		PORTC |= (1<<0);		//alarm on
//		send_Uart_str(dial);	//send AT_dial to BT
//		}
//


//	/*опрос vibro*/
	else if (bit_is_set(PINC,PC3)) // если кнопка нажата (подключена к gnd, замыкается на vcc)
		{
		_delay_ms (150);
		PORTC |= (1<<0);		//alarm on
		send_Uart_str(dial);	//send AT_dial to BT
		}



//	/*опрос кнопки сброса*/
//	else if (bit_is_set(PINC,PC5)) // если кнопка нажата (подключена к gnd, замыкается на vcc)
//		{
//		_delay_ms (150);
//		PORTC &= (~(1<<0));		//alarm off
//		}
}
