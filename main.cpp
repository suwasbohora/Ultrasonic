 /* Ultrasonic_Sensor.cpp
 *
 * Created: 8/7/2017 12:06:09 PM
 * Author : suwas
  */

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <avr/lcd4bits.h>

#define  Trigger_pin	PIND0	/* Trigger pin */

int main(void)
{
	char string[10];
	long count;
	double distance;
    DDRB = 0xff;
    DDRD = 0x01;
	PORTD = 0xff;
	DDRA = 0xff;
	DDRD |= 1<<PIND5;
	DDRC &=~1<<PINC2;
    sei();			/* Enable global interrupt */
    TIMSK = (1 << TOIE1);	/* Enable Timer1 overflow interrupts */
    TCCR1A = 0;		/* Set all bit to zero Normal operation */
   
    lcd_init();
    while(1)
    {
		/* Give 10us trigger pulse on trig. pin to HC-SR04 */
		PORTD |= (1 << Trigger_pin);
		_delay_us(10);
		PORTD &= (~(1 << Trigger_pin));
		
		TCNT1 = 0;	/* Clear Timer counter */
		TCCR1B = 0x41;	/* Capture on rising edge, No prescaler*/
		TIFR = 1<<ICF1;	/* Clear ICP flag (Input Capture flag) */
		TIFR = 1<<TOV1;	/* Clear Timer Overflow flag */

		/*Calculate width of Echo by Input Capture (ICP) */
		
		while ((TIFR & (1 << ICF1)) == 0);/* Wait for rising edge */
		TCNT1 = 0;	/* Clear Timer counter */
		TCCR1B = 0x01;	/* Capture on falling edge, No prescaler */
		TIFR = 1<<ICF1;	/* Clear ICP flag (Input Capture flag) */
		TIFR = 1<<TOV1;	/* Clear Timer Overflow flag */
		TimerOverflow = 0;/* Clear Timer overflow count */

		while ((TIFR & (1 << ICF1)) == 0);/* Wait for falling edge */
		count = ICR1 + (65535 * TimerOverflow);	/* Take count */
		/* 16MHz Timer freq, sound speed =343 m/s */
		distance =(double)count /1000;
		dtostrf(distance, 2, 2, string);/* distance to string */
		strcat(string, " cm   ");	/* Concat unit i.e.cm */
		_delay_ms(200);
		
	}
}
