#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIM1_OVF_vect)
{
	TCNT1 = 12;
	// Do counting here
}

timer1_begin() {
	TCCR1 &= ~(_BV(COM1A1) | _BV(COM1A0));	// Comparator A Mode, COM1A1 = 0 COM1A0 = 0 (disabled)
	TCCR1 &= ~(_BV(COM1B1) | _BV(COM1B0));	// Comparator B Mode, COM1B1 = 0 COM1B0 = 0 (disabled)
	TCCR1 |= _BV(CS13) | _BV(CS12)	// Prescaler
	TCCR1 &= ~(_BV(CS11) | _BV(CS10));
	
	TIMSK |= _BV(TOIE1);	// Enable interrupt
	
	TCNT1 = 12;
}