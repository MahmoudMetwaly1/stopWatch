/*
======================================================
   Name       : stop watch
   Author     : Mahmoud Metwally Hassan
   Description: stop watch
=======================================================
 */

#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
unsigned char sec = 0, minutes = 0 , hours = 0;

/* TIMER1 Interrupt Service Routine */
ISR(TIMER1_COMPA_vect)
{
	/* the greater number of second is 59 then Overflow
	 * the minutes increment
	 * the greater number of minutes is 59 then Overflow
	 * the hours increment until reach 24 hours
	 */

	if(sec == 59)    // check if second = 59 (true) make second over flow
	{
		sec = 0;   // set sec = 0
		if(minutes == 59)    // check if minutes = 59 (true) make minutes over flow
		{
			minutes = 0;//set minutes = 0
			if(hours == 24)
			{
				TCCR1B &= (~(1<<CS12)) & (~(1<<CS11)) & (~(1<<CS10)); // stop the timer by make clock = 0  CS12=0 CS11=0  CS10=0
			}
			else
			{
				hours++;      // increment hours
			}
		}
		else   // if second over flow and minutes not equal 59
		{
			minutes++;  // increment the minutes
		}
	}
	else    // if second not equal 59
	{
		sec++; // increment second
	}

}

/* Enable Timer1 compare mode interrupt */
void TIMER1_INT_COMP_MODE(void)
{
	/*  Timer1 control register
		 * 1. Non PWM mode FOC1A=1
		 * 2. CTC Mode WGM12=1
		 * 3. clock = F_CPU/1024 CS12=1 CS10=1
		 */
	TCCR1A = (1<<FOC1A);
	TCCR1B = (1<< CS12) | (1<<CS10) | (1<<WGM12);
	TCNT1 = 0;            //set timer initial value = 0
	OCR1A = 977;          //set compare value
	TIMSK = (1<<OCIE1A);  // Enable Timer Compare Interrupt
	SREG |= (1<<7);		  // Enable Global interrupt in Mc
}


/* External INT0 Interrupt Service Routine */
ISR(INT0_vect)
{
	/* reset the stop watch */
	TCCR1B &= (~(1<<CS12)) & (~(1<<CS11)) & (~(1<<CS10)); // stop the timer by make clock = 0  CS12=0 CS11=0  CS10=0
	sec = 0;        // Set second = 0
	minutes = 0;    // Set minutes = 0
	hours = 0;      // Set hours = 0
	TIMER1_INT_COMP_MODE(); // Start Timer1 Again
}

/* External INT1 Interrupt Service Routine */
ISR(INT1_vect)
{
	/* stop Timer1 */
	TCCR1B &= (~(1<<CS12)) & (~(1<<CS11)) & (~(1<<CS10)); // stop the timer by make clock = 0  CS12=0 CS11=0  CS10=0

}

/* External INT2 Interrupt Service Routine */
ISR(INT2_vect)
{
	/* Resume the timer */
	TIMER1_INT_COMP_MODE(); // Start Timer1 Again

}

/* External INT0 Enable and configuration function */
void INT0_falling_edge(void)
{
	DDRD &= ~(1<<2);       // Configure INT0/PD2 as input pin
	PORTD |= (1<<2);       // Enable Internal Pull Up in PD2
	MCUCR |= (1<<ISC01);   // Trigger INT0 with the Falling edge
	MCUCR &= ~(1<<ISC00);  // Trigger INT0 with the falling edge
	GICR |= (1<<INT0);     // Enable external interrupt pin INT0
	SREG |= (1<<7);        // Enable Global interrupt in MC
}

/* External INT1 Enable and configuration function */
void INT1_rising_edge(void)
{
	DDRD &= ~(1<<3);                    // configure INT1/PD3 AS input pin
	MCUCR |= (1<<ISC10) | (1<<ISC11);   // Trigger INT1 With the rising edge
	GICR |= (1<<INT1);                  // Enable external interrupt Pin INT1
	SREG |= (1<<7);                     // Enable Global Interrupt In MC
}

/* External INT2 Enable and configuration function */
void INT2_falling_edge(void)
{
	DDRB &= ~(1<<2);           // configure INT1/PB2 AS input pin
	PORTB |= (1<<2);           // Enable internal pull up in PB2
	MCUCSR &= ~(1<<ISC2);      // trigger INT2 with falling edge
	GICR |= (1<<INT2);         // Enable external Interrupt pin INT2
	SREG |= (1<<7);            // Enable Global Interrupt IN MC

}

/* 7-segment Display */
void seven_segmant_display(unsigned char num)
{
	PORTC = (PORTC & 0xf0) | (num & 0x0f);

}


int main(void)
{
	TIMER1_INT_COMP_MODE();  // Enable Timer1
	INT0_falling_edge();     // Enable INT0
	INT1_rising_edge();      // Enable INT1
	INT2_falling_edge();     // Enable INT2
	DDRC |= 0x0f;  // Configure the first 4-pins in PORTC as output pins
	DDRA |= 0x3f;  // Configure the first 6-pins in PORTA as output pins
	PORTA &= 0xC0; // Turn off the 6 7-segment at the beginning by clear the first 6-bits in PORTA
	seven_segmant_display(0);   //Display the initial value of 7-segment
	while(1)
	{

		PORTA = (PORTA & 0xC0) | 0x20;   // Turn on the first 7-segment only
		seven_segmant_display(sec % 10); //Display the first Digit of second
		_delay_ms(2);

		PORTA = (PORTA & 0xC0) | 0x10;   // Turn on the second 7-segment only
		seven_segmant_display(sec / 10);  //Display the second Digit of second
		_delay_ms(2);

		PORTA = (PORTA & 0xC0) | 0x08;   // Turn on the third 7-segment only
		seven_segmant_display(minutes % 10); //Display the first Digit of minutes
		_delay_ms(2);

		PORTA = (PORTA & 0xC0) | 0x04;   // Turn on the fourth 7-segment only
		seven_segmant_display(minutes / 10);  //Display the second Digit of minutes
		_delay_ms(2);

		PORTA = (PORTA & 0xC0) | 0x02;   // Turn on the fifth 7-segment only
		seven_segmant_display(hours % 10); //Display the first Digit of hours
		_delay_ms(2);

		PORTA = (PORTA & 0xC0) | 0x01;   // Turn on the sixth 7-segment only
		seven_segmant_display(hours / 10); // Display the second Digit of hours
		_delay_ms(2);


	}
}

