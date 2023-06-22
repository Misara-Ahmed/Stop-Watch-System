/*
 * Stop_Watch_System.c
 *
 *  Created on: Sep 25, 2022
 *      Author: Misara
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Defining global variables.
unsigned char sec = 0;
unsigned char min = 0;
unsigned char hr = 0;

void Timer1_Init(void)
{
	// Setting the timer to be compare mode & Setting the clock and 64 prescaler.
	TCCR1A = 0x0C;
	TCCR1B |= 0x0B;
	TCCR1B &= 0xEB;
	TCNT1 = 0; // Initial start for the timer.
	OCR1A = 15625; // Setting compare value.
	TIMSK |= (1<<OCIE1A); // Enabling the compare mode timer.
}
ISR(TIMER1_COMPA_vect)
{
	sec++;
	if (sec == 60)
	{
		sec = 0;
		min++;
		if(min == 60)
		{
			min = 0;
			hr++;
			if(hr == 24)
			{
				hr = 0;
			}
		}
	}
}

void INT0_Init(void)
{
	DDRD &= 0xFB; // Making pin2 in port D as input.
	PORTD |= (1<<2); // Activating the internal pull up resistor.
	SREG |= (1<<7); // Enabling the I-bit for all interrupts.
	// Setting the interrupt with the falling edge.
	MCUCR &= ~(1<<ISC00);
	MCUCR |= (1<<ISC01);
	GICR |= (1<<6); // Enabling interrupt 0.
}
ISR(INT0_vect)
{
	// Returning the timer to its initial state.
	sec = 0;
	min = 0;
	hr = 0;
}

void INT1_Init(void)
{
	DDRD &= 0xF7; // Making pin3 in port D as input.
	MCUCR |= (1<<ISC11) | (1<<ISC10); // Setting the interrupt with the rising edge.
	GICR |= (1<<7); // Enabling interrupt 1.
}
ISR(INT1_vect)
{
	// Closing the clock source to stop the timer.
	TCCR1B &= ~(1<<CS10) & ~(1<<CS11) & ~(1<<CS12);
}

void INT2_Init(void)
{
	DDRD &= 0xFB; // Making pin2 in port B as input.
	PORTB |= (1<<2); // Activating the internal pull up resistor.
	MCUCSR &= ~(1<<ISC2); // Setting the interrupt with the falling edge.
	GICR |= (1<<5); // Enabling interrupt 2.
}
ISR(INT2_vect)
{
	// Opening the clock source to make timer resume.
	TCCR1B |= 0x0B;
	TCCR1B &= 0xEB;
}

void SevenSegment_Init()
{
	DDRC |= 0x0F; // Setting the first 4 pins in PORT C to be output pins.
	PORTC = 0;
	DDRA |= 0x3F; // Setting the first 6 pins in PORT A to be output pins.
	PORTA &= 0xC0;
}

void display(unsigned char num)
{
	switch (num){
	case 0: // displaying on the 1st 7-segment.
		PORTA = 0xFE;
		PORTC = (sec % 10) ;
		break;
	case 1: // displaying on the 2nd 7-segment.
		PORTA = 0xFD;
		PORTC = (sec/10) ;
		break;
	case 2: // displaying on the 3rd 7-segment.
		PORTA = 0xFB;
		PORTC = (min % 10) ;
		break;
	case 3: // displaying on the 4th first 7-segment.
		PORTA = 0xF7;
		PORTC = (min/10) ;
		break;
	case 4: // displaying on the 5th first 7-segment.
		PORTA = 0xEF;
		PORTC = (hr % 10) ;
		break;
	case 5: // displaying on the 6th first 7-segment.
		PORTA = 0xDF;
		PORTC = (hr/10) ;
		break;
	}
}

int main()
{
	// Initializing the Stop Watch.
	SevenSegment_Init();
	Timer1_Init();
	INT0_Init();
	INT1_Init();
	INT2_Init();
	while(1)
	{
		// Always displaying on the 7-segments.
		display(0);
		_delay_ms(3);
		display(1);
		_delay_ms(3);
		display(2);
		_delay_ms(3);
		display(3);
		_delay_ms(3);
		display(4);
		_delay_ms(3);
		display(5);
		_delay_ms(3);
	}
}

