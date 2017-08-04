#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t timer_clock;

ISR(TIM1_OVF_vect)
{
    TCNT1 = 12;
    timer_clock ++;
}

void timer1_begin() {
    TCCR1 = _BV(CS13) | _BV(CS11) | _BV(CS10);   // PWM/Comparator disabled. Prescaler CK/1024
    TIMSK |= _BV(TOIE1);                         // Enable interrupt

    TCNT1 = 12;
    timer_clock = 0;
}

