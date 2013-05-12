#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//iButton is on pin D0.

#define drivePin DDRD |= (1<<0)
#define driveLow PORTD &= ~(1<<0)
#define floatPin DDRD &= ~(1<<0)
#define driveStatus DDRD |= (1<<6)
#define statusOn PORTD |= (1<<6)
#define statusOff PORTD &= ~(1<<6)



void send1() //Turn off interrupts before use!
{
	drivePin;
	driveLow;
	_delay_us(6);
	floatPin;
	_delay_us(64);
	return;
}

void send0() //Turn off interrupts before use!
{
	drivePin;
	driveLow;
	_delay_us(60);
	floatPin;
	_delay_us(10);
	return;
}

int resetPresence()
{
	cli();
	drivePin;
	driveLow;
	_delay_us(510);
	floatPin;
	_delay_us(5);
	if(!(PIND & (1<<0)))
	{
		sei();
		return 0;
	}
	_delay_us(60);
	if(PIND & (1<<0))
	{
		sei();
		return 0;
	}
	else
	{
		sei();
		return 1;
	}
}

int main()
{
	CLKPR = 0x80;
	CLKPR = 0x00; //16MHz, 16 cycles per microsecond
	floatPin;
	driveStatus;
	statusOn;
	_delay_ms(250);
	statusOff;
	_delay_ms(250);
	statusOn;
	_delay_ms(250);
	statusOff;
	int status = 0;
	while(1)
	{
		status = resetPresence();
		if(status == 0)
		{
			statusOff;
		}
		else
		{
			statusOn;
			_delay_ms(250);
			statusOff;
		}
		_delay_ms(100);
	}
	return 0;
}
