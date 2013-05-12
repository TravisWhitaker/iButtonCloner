#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

int main()
{
	CLKPR = 0x80;
	CLKPR = 0x00; //16MHz, 16 cycles per microsecond
	DDRD |= (1<<6);
	DDRD &= ~(1<<0);
	PORTD |= (1<<6);
	_delay_ms(250);
	PORTD &= ~(1<<6);
	_delay_ms(250);
	PORTD |= (1<<6);
	_delay_ms(250);
	PORTD &= ~(1<<6);
	while(1)
	{
		DDRD |= (1<<0);
		PORTD &= ~(1<<0);
		_delay_us(510);
		DDRD &= ~(1<<0);
		PORTD &= ~(1<<0);
		_delay_us(70);
		if(PIND & (1<<0))
		{
			PORTD &= ~(1<<6);
		}
		else
		{
			PORTD |= (1<<6);
			_delay_ms(250);
			PORTD &= ~(1<<6);
		}
	_delay_ms(100);
	}
	return 0;
}
